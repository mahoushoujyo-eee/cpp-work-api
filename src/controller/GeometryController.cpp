#include "GeometryController.h"
#include <iostream>
#include <sstream>

void GeometryController::setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

Json::Value GeometryController::parseJson(const std::string& body) {
    Json::Value json;
    Json::Reader reader;
    
    if (!reader.parse(body, json)) {
        throw std::invalid_argument("Invalid JSON format");
    }
    
    return json;
}

std::string GeometryController::serializeJson(const Json::Value& json) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, json);
}

void GeometryController::sendErrorResponse(httplib::Response& res, int statusCode, const std::string& message) {
    setCorsHeaders(res);
    
    Json::Value error;
    error["error"] = message;
    error["status"] = statusCode;
    
    res.status = statusCode;
    res.set_content(serializeJson(error), "application/json");
}

void GeometryController::sendSuccessResponse(httplib::Response& res, const Json::Value& data) {
    setCorsHeaders(res);
    
    Json::Value response;
    response["success"] = true;
    response["data"] = data;
    
    res.status = 200;
    res.set_content(serializeJson(response), "application/json");
}

GeometryController::GeometryController() {
    
}

GeometryController::GeometryController(const DatabaseConfig& dbConfig) : service(std::make_shared<GeometryService>(dbConfig)) {
    
}

void GeometryController::setService(std::shared_ptr<GeometryService> servicePtr) {
    service = std::move(servicePtr);
}

void GeometryController::registerRoutes(httplib::Server& server) {
    server.Options(".*", [this](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        res.status = 200;
    });
    
    server.Get("/health", [this](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        
        Json::Value health;
        health["status"] = "ok";
        health["service"] = service->isServiceAvailable();
        health["database"] = service->isDatabaseAvailable();
        health["timestamp"] = std::time(nullptr);
        
        sendSuccessResponse(res, health);
    });
    
    server.Get("/primitives", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "\n=== GET /primitives 请求开始 ===" << std::endl;
        std::cout << "请求时间: " << std::time(nullptr) << std::endl;
        
        std::cout << "查询参数: ";
        bool hasParams = false;
        
        for (const auto& param : req.params) {
            std::cout << param.first << "=" << param.second << " ";
            hasParams = true;
        }
        
        if (!hasParams) {
            std::cout << "无查询参数（获取所有图元）";
        }
        std::cout << std::endl;
        
        try {
            Json::Value result = service->getAllPrimitivesAsJson();
            std::cout << "返回图元数量: " << result.size() << std::endl;
            sendSuccessResponse(res, result);
        } catch (const std::exception& e) {
            sendErrorResponse(res, 500, e.what());
        }
        
        std::cout << "=== GET /primitives 请求结束 ===\n" << std::endl;
    });
    
    server.Get(R"(/primitives/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int id = std::stoi(req.matches[1]);
            auto primitive = service->getPrimitiveById(id);
            
            if (primitive) {
                sendSuccessResponse(res, primitive->toJson());
            } else {
                sendErrorResponse(res, 404, "Primitive not found");
            }
        } catch (const std::exception& e) {
            sendErrorResponse(res, 400, e.what());
        }
    });
    
    server.Post("/primitives", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            Json::Value data = parseJson(req.body);
            auto primitive = service->createPrimitive(data);
            
            if (primitive) {
                sendSuccessResponse(res, primitive->toJson());
            } else {
                sendErrorResponse(res, 400, "Failed to create primitive");
            }
        } catch (const std::exception& e) {
            sendErrorResponse(res, 400, e.what());
        }
    });
    
    server.Put(R"(/primitives/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int id = std::stoi(req.matches[1]);
            Json::Value data = parseJson(req.body);
            
            if (service->updatePrimitive(id, data)) {
                auto primitive = service->getPrimitiveById(id);
                sendSuccessResponse(res, primitive->toJson());
            } else {
                sendErrorResponse(res, 404, "Primitive not found or update failed");
            }
        } catch (const std::exception& e) {
            sendErrorResponse(res, 400, e.what());
        }
    });
    
    server.Delete(R"(/primitives/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int id = std::stoi(req.matches[1]);
            
            if (service->deletePrimitive(id)) {
                Json::Value result;
                result["message"] = "Primitive deleted successfully";
                sendSuccessResponse(res, result);
            } else {
                sendErrorResponse(res, 404, "Primitive not found");
            }
        } catch (const std::exception& e) {
            sendErrorResponse(res, 400, e.what());
        }
    });
    
    server.Delete("/primitives", [this](const httplib::Request&, httplib::Response& res) {
        try {
            if (service->deleteAllPrimitives()) {
                Json::Value result;
                result["message"] = "All primitives deleted successfully";
                sendSuccessResponse(res, result);
            } else {
                sendErrorResponse(res, 500, "Failed to delete all primitives");
            }
        } catch (const std::exception& e) {
            sendErrorResponse(res, 500, e.what());
        }
    });
    
    server.Get("/statistics", [this](const httplib::Request&, httplib::Response& res) {
        try {
            Json::Value stats = service->getStatistics();
            sendSuccessResponse(res, stats);
        } catch (const std::exception& e) {
            sendErrorResponse(res, 500, e.what());
        }
    });
    
    server.Get(R"(/primitives/type/(.+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string type = req.matches[1];
            auto primitives = service->getPrimitivesByType(type);
            
            Json::Value result(Json::arrayValue);
            for (const auto& primitive : primitives) {
                result.append(primitive->toJson());
            }
            
            sendSuccessResponse(res, result);
        } catch (const std::exception& e) {
            sendErrorResponse(res, 500, e.what());
        }
    });
}

void GeometryController::start(const std::string& host, int port) {
    std::cout << "Starting Geometry API Server on " << host << ":" << port << std::endl;
    
    if (!server.listen(host.c_str(), port)) {
        std::cerr << "Failed to start server on " << host << ":" << port << std::endl;
        throw std::runtime_error("Failed to start server on " + host + ":" + std::to_string(port));
    }
}

void GeometryController::stop() {
    server.stop();
}