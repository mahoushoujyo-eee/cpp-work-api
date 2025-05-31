#include "httplib.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <jsoncpp/json/json.h>
#include <sstream>
#include <memory>
#include <algorithm>

// 几何图元基类
class GeometryPrimitive {
public:
    std::string id;
    std::string type;
    double x, y;
    std::string color;
    
    GeometryPrimitive(const std::string& id, const std::string& type, double x, double y, const std::string& color)
        : id(id), type(type), x(x), y(y), color(color) {}
    
    virtual ~GeometryPrimitive() = default;
    virtual Json::Value toJson() const {
        Json::Value json;
        json["id"] = id;
        json["type"] = type;
        json["x"] = x;
        json["y"] = y;
        json["color"] = color;
        return json;
    }
};

// 圆形
class Circle : public GeometryPrimitive {
public:
    double radius;
    
    Circle(const std::string& id, double x, double y, double radius, const std::string& color)
        : GeometryPrimitive(id, "circle", x, y, color), radius(radius) {}
    
    Json::Value toJson() const override {
        Json::Value json = GeometryPrimitive::toJson();
        json["radius"] = radius;
        return json;
    }
};

// 矩形
class Rectangle : public GeometryPrimitive {
public:
    double width, height;
    
    Rectangle(const std::string& id, double x, double y, double width, double height, const std::string& color)
        : GeometryPrimitive(id, "rectangle", x, y, color), width(width), height(height) {}
    
    Json::Value toJson() const override {
        Json::Value json = GeometryPrimitive::toJson();
        json["width"] = width;
        json["height"] = height;
        return json;
    }
};

// 三角形
class Triangle : public GeometryPrimitive {
public:
    double x2, y2, x3, y3;
    
    Triangle(const std::string& id, double x1, double y1, double x2, double y2, double x3, double y3, const std::string& color)
        : GeometryPrimitive(id, "triangle", x1, y1, color), x2(x2), y2(y2), x3(x3), y3(y3) {}
    
    Json::Value toJson() const override {
        Json::Value json = GeometryPrimitive::toJson();
        json["x2"] = x2;
        json["y2"] = y2;
        json["x3"] = x3;
        json["y3"] = y3;
        return json;
    }
};

// 几何图元管理器
class GeometryManager {
private:
    std::map<std::string, std::shared_ptr<GeometryPrimitive>> primitives;
    int nextId = 1;
    
public:
    std::string generateId() {
        return "geo_" + std::to_string(nextId++);
    }
    
    void addPrimitive(std::shared_ptr<GeometryPrimitive> primitive) {
        primitives[primitive->id] = primitive;
    }
    
    std::shared_ptr<GeometryPrimitive> getPrimitive(const std::string& id) {
        auto it = primitives.find(id);
        return (it != primitives.end()) ? it->second : nullptr;
    }
    
    std::vector<std::shared_ptr<GeometryPrimitive>> getAllPrimitives() {
        std::vector<std::shared_ptr<GeometryPrimitive>> result;
        for (const auto& pair : primitives) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    bool deletePrimitive(const std::string& id) {
        return primitives.erase(id) > 0;
    }
    
    void clear() {
        primitives.clear();
    }
};

// 全局几何图元管理器
GeometryManager geometryManager;

// JSON解析辅助函数
Json::Value parseJson(const std::string& jsonString) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(jsonString, root);
    return root;
}

// JSON序列化辅助函数
std::string jsonToString(const Json::Value& json) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, json);
}

// CORS处理函数
void setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

int main() {
    httplib::Server server;
    
    // 处理CORS预检请求
    server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        return;
    });
    
    // 获取所有几何图元
    server.Get("/api/primitives", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        
        Json::Value jsonArray(Json::arrayValue);
        auto primitives = geometryManager.getAllPrimitives();
        
        for (const auto& primitive : primitives) {
            jsonArray.append(primitive->toJson());
        }
        
        res.set_content(jsonToString(jsonArray), "application/json");
    });
    
    // 获取单个几何图元
    server.Get(R"(/api/primitives/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        
        std::string id = req.matches[1];
        auto primitive = geometryManager.getPrimitive(id);
        
        if (primitive) {
            res.set_content(jsonToString(primitive->toJson()), "application/json");
        } else {
            res.status = 404;
            Json::Value error;
            error["error"] = "Primitive not found";
            res.set_content(jsonToString(error), "application/json");
        }
    });
    
    // 创建新的几何图元
    server.Post("/api/primitives", [](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        
        try {
            Json::Value json = parseJson(req.body);
            std::string type = json["type"].asString();
            std::string id = geometryManager.generateId();
            
            std::shared_ptr<GeometryPrimitive> primitive;
            
            if (type == "circle") {
                primitive = std::make_shared<Circle>(
                    id,
                    json["x"].asDouble(),
                    json["y"].asDouble(),
                    json["radius"].asDouble(),
                    json["color"].asString()
                );
            } else if (type == "rectangle") {
                primitive = std::make_shared<Rectangle>(
                    id,
                    json["x"].asDouble(),
                    json["y"].asDouble(),
                    json["width"].asDouble(),
                    json["height"].asDouble(),
                    json["color"].asString()
                );
            } else if (type == "triangle") {
                primitive = std::make_shared<Triangle>(
                    id,
                    json["x"].asDouble(),
                    json["y"].asDouble(),
                    json["x2"].asDouble(),
                    json["y2"].asDouble(),
                    json["x3"].asDouble(),
                    json["y3"].asDouble(),
                    json["color"].asString()
                );
            } else {
                res.status = 400;
                Json::Value error;
                error["error"] = "Unsupported primitive type";
                res.set_content(jsonToString(error), "application/json");
                return;
            }
            
            geometryManager.addPrimitive(primitive);
            res.status = 201;
            res.set_content(jsonToString(primitive->toJson()), "application/json");
            
        } catch (const std::exception& e) {
            res.status = 400;
            Json::Value error;
            error["error"] = "Invalid JSON data";
            res.set_content(jsonToString(error), "application/json");
        }
    });
    
    // 更新几何图元
    server.Put(R"(/api/primitives/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        
        std::string id = req.matches[1];
        auto existingPrimitive = geometryManager.getPrimitive(id);
        
        if (!existingPrimitive) {
            res.status = 404;
            Json::Value error;
            error["error"] = "Primitive not found";
            res.set_content(jsonToString(error), "application/json");
            return;
        }
        
        try {
            Json::Value json = parseJson(req.body);
            std::string type = json["type"].asString();
            
            std::shared_ptr<GeometryPrimitive> primitive;
            
            if (type == "circle") {
                primitive = std::make_shared<Circle>(
                    id,
                    json["x"].asDouble(),
                    json["y"].asDouble(),
                    json["radius"].asDouble(),
                    json["color"].asString()
                );
            } else if (type == "rectangle") {
                primitive = std::make_shared<Rectangle>(
                    id,
                    json["x"].asDouble(),
                    json["y"].asDouble(),
                    json["width"].asDouble(),
                    json["height"].asDouble(),
                    json["color"].asString()
                );
            } else if (type == "triangle") {
                primitive = std::make_shared<Triangle>(
                    id,
                    json["x"].asDouble(),
                    json["y"].asDouble(),
                    json["x2"].asDouble(),
                    json["y2"].asDouble(),
                    json["x3"].asDouble(),
                    json["y3"].asDouble(),
                    json["color"].asString()
                );
            } else {
                res.status = 400;
                Json::Value error;
                error["error"] = "Unsupported primitive type";
                res.set_content(jsonToString(error), "application/json");
                return;
            }
            
            geometryManager.addPrimitive(primitive);
            res.set_content(jsonToString(primitive->toJson()), "application/json");
            
        } catch (const std::exception& e) {
            res.status = 400;
            Json::Value error;
            error["error"] = "Invalid JSON data";
            res.set_content(jsonToString(error), "application/json");
        }
    });
    
    // 删除几何图元
    server.Delete(R"(/api/primitives/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        
        std::string id = req.matches[1];
        
        if (geometryManager.deletePrimitive(id)) {
            res.status = 204;
        } else {
            res.status = 404;
            Json::Value error;
            error["error"] = "Primitive not found";
            res.set_content(jsonToString(error), "application/json");
        }
    });
    
    // 清空所有几何图元
    server.Delete("/api/primitives", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        geometryManager.clear();
        res.status = 204;
    });
    
    // 健康检查端点
    server.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        Json::Value health;
        health["status"] = "ok";
        health["service"] = "Geometry Primitives Manager";
        health["version"] = "1.0.0";
        res.set_content(jsonToString(health), "application/json");
    });
    
    std::cout << "几何图元管理器HTTP服务启动中..." << std::endl;
    std::cout << "服务地址: http://localhost:8080" << std::endl;
    std::cout << "API端点:" << std::endl;
    std::cout << "  GET    /api/health           - 健康检查" << std::endl;
    std::cout << "  GET    /api/primitives       - 获取所有图元" << std::endl;
    std::cout << "  GET    /api/primitives/{id}  - 获取单个图元" << std::endl;
    std::cout << "  POST   /api/primitives       - 创建新图元" << std::endl;
    std::cout << "  PUT    /api/primitives/{id}  - 更新图元" << std::endl;
    std::cout << "  DELETE /api/primitives/{id}  - 删除图元" << std::endl;
    std::cout << "  DELETE /api/primitives       - 清空所有图元" << std::endl;
    
    // 启动服务器
    if (!server.listen("0.0.0.0", 8080)) {
        std::cerr << "服务器启动失败!" << std::endl;
        return -1;
    }
    
    return 0;
}