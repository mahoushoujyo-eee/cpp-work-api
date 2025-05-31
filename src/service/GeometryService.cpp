#include "GeometryService.h"
#include "../models/Shape2D.h"
#include "../models/Shape3D.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// 验证图元数据
bool GeometryService::validatePrimitiveData(const Json::Value& data) {
    // 检查基本字段：type, name, color
    if (!data.isMember("type") || !data.isMember("name") || !data.isMember("color")) {
        return false;
    }
    
    // 检查位置信息 - 支持两种格式
    bool hasPosition = false;
    if (data.isMember("position") && data["position"].isObject()) {
        const Json::Value& pos = data["position"];
        hasPosition = pos.isMember("x") && pos.isMember("y");
    } else if (data.isMember("x") && data.isMember("y")) {
        hasPosition = true;
    }
    
    if (!hasPosition) {
        return false;
    }
    
    std::string type = data["type"].asString();
    
    if (type == "Rectangle") {
        return data.isMember("width") && data.isMember("height") &&
               data["width"].isDouble() && data["height"].isDouble() &&
               data["width"].asDouble() > 0 && data["height"].asDouble() > 0;
    } else if (type == "Circle") {
        return data.isMember("radius") && data["radius"].isDouble() &&
               data["radius"].asDouble() > 0;
    } else if (type == "Cuboid") {
        // 支持两种格式：length/width/height 或 width/height/depth
        bool hasLengthFormat = data.isMember("length") && data.isMember("width") && data.isMember("height") &&
                              data["length"].isDouble() && data["width"].isDouble() && data["height"].isDouble() &&
                              data["length"].asDouble() > 0 && data["width"].asDouble() > 0 && data["height"].asDouble() > 0;
        
        bool hasDepthFormat = data.isMember("width") && data.isMember("height") && data.isMember("depth") &&
                             data["width"].isDouble() && data["height"].isDouble() && data["depth"].isDouble() &&
                             data["width"].asDouble() > 0 && data["height"].asDouble() > 0 && data["depth"].asDouble() > 0;
        
        return hasLengthFormat || hasDepthFormat;
    } else if (type == "Cylinder") {
        return data.isMember("radius") && data.isMember("height") &&
               data["radius"].isDouble() && data["height"].isDouble() &&
               data["radius"].asDouble() > 0 && data["height"].asDouble() > 0;
    }
    
    return false;
}

// 从JSON创建图元对象
std::shared_ptr<GeometryPrimitive> GeometryService::createPrimitiveFromJson(const Json::Value& data) {
    if (!validatePrimitiveData(data)) {
        return nullptr;
    }
    
    std::string type = data["type"].asString();
    std::string name = data["name"].asString();
    std::string color = data["color"].asString();
    
    // 解析位置 - 支持两种格式：直接的x,y,z字段或position对象
    double x, y, z = 0.0;
    if (data.isMember("position") && data["position"].isObject()) {
        // 使用position对象格式
        const Json::Value& pos = data["position"];
        x = pos["x"].asDouble();
        y = pos["y"].asDouble();
        z = pos.isMember("z") ? pos["z"].asDouble() : 0.0;
    } else {
        // 使用直接的x, y, z字段
        x = data["x"].asDouble();
        y = data["y"].asDouble();
        z = data.isMember("z") ? data["z"].asDouble() : 0.0;
    }
    Position position(x, y, z);
    
    int id = data.isMember("id") ? data["id"].asInt() : -1;
    
    if (type == "Rectangle") {
        double width = data["width"].asDouble();
        double height = data["height"].asDouble();
        auto rect = std::make_shared<Rectangle>(name, position, color, width, height);
        if (id != -1) rect->setId(id);
        return rect;
    } else if (type == "Circle") {
        double radius = data["radius"].asDouble();
        auto circle = std::make_shared<Circle>(name, position, color, radius);
        if (id != -1) circle->setId(id);
        return circle;
    } else if (type == "Cuboid") {
        double length, width, height;
        
        // 支持两种格式：length/width/height 或 width/height/depth
        if (data.isMember("length")) {
            // 使用 length/width/height 格式
            length = data["length"].asDouble();
            width = data["width"].asDouble();
            height = data["height"].asDouble();
        } else {
            // 使用 width/height/depth 格式，映射到 length/width/height
            length = data["depth"].asDouble();  // depth -> length
            width = data["width"].asDouble();   // width -> width
            height = data["height"].asDouble(); // height -> height
        }
        
        auto cuboid = std::make_shared<Cuboid>(name, position, color, length, width, height);
        if (id != -1) cuboid->setId(id);
        return cuboid;
    } else if (type == "Cylinder") {
        double radius = data["radius"].asDouble();
        double height = data["height"].asDouble();
        auto cylinder = std::make_shared<Cylinder>(name, position, color, radius, height);
        if (id != -1) cylinder->setId(id);
        return cylinder;
    }
    
    return nullptr;
}

// 构造函数 - 不使用数据库模式
GeometryService::GeometryService() : dao(nullptr), container(std::make_unique<GeometryContainer>()), useDatabaseMode(false) {
    // 仅使用容器模式，不加载数据库数据
}

// 带数据库配置的构造函数 - 使用数据库模式
GeometryService::GeometryService(const DatabaseConfig& dbConfig) : dao(std::make_unique<GeometryDAO>(dbConfig)), container(std::make_unique<GeometryContainer>()), useDatabaseMode(true) {
    loadAllFromDatabase();
}

// 析构函数
GeometryService::~GeometryService() = default;

// 从数据库加载所有图元到容器
void GeometryService::loadAllFromDatabase() {
    if (!useDatabaseMode || !dao) {
        return;  // 非数据库模式或dao未初始化，直接返回
    }
    
    try {
        auto primitives = dao->findAll();
        container->clear();
        for (const auto& primitive : primitives) {
            container->addPrimitive(primitive);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading primitives from database: " << e.what() << std::endl;
    }
}

// 创建图元
std::shared_ptr<GeometryPrimitive> GeometryService::createPrimitive(const Json::Value& data) {
    auto primitive = createPrimitiveFromJson(data);
    if (!primitive) {
        //std::cerr << "Error creating primitive from JSON data." << std::endl;
        return nullptr;
    }
    
    if (useDatabaseMode && dao) {
        // 数据库模式：保存到数据库
        if (dao->save(primitive)) {
            return primitive;
        }
        return nullptr;
    } else {
        //std::cout << "Using container mode." << std::endl;
        // 容器模式：直接添加到容器
        container->addPrimitive(primitive);
        return primitive;
    }
}

// 根据ID获取图元
std::shared_ptr<GeometryPrimitive> GeometryService::getPrimitiveById(int id) {
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库查询
        return dao->findById(id);
    } else {
        // 容器模式：从容器查询
        return container->findById(id);
    }
}

// 更新图元
bool GeometryService::updatePrimitive(int id, const Json::Value& data) {
    auto newPrimitive = createPrimitiveFromJson(data);
    if (!newPrimitive) {
        return false;
    }
    
    // 设置正确的ID
    newPrimitive->setId(id);
    
    if (useDatabaseMode && dao) {
        // 数据库模式：更新数据库
        return dao->save(newPrimitive);
    } else {
        // 容器模式：更新容器
        return container->updatePrimitive(id, newPrimitive);
    }
}

// 删除图元
bool GeometryService::deletePrimitive(int id) {
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库删除
        return dao->deleteById(id);
    } else {
        // 容器模式：从容器删除
        return container->deletePrimitive(id);
    }
}

// 删除所有图元
bool GeometryService::deleteAllPrimitives() {
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库删除所有
        return dao->deleteAll();
    } else {
        // 容器模式：清空容器
        container->clear();
        return true;
    }
}

// 根据类型获取图元
std::vector<std::shared_ptr<GeometryPrimitive>> GeometryService::getPrimitivesByType(const std::string& type) {
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库查询
        return dao->findByType(type);
    } else {
        // 容器模式：从容器查询
        return container->findByType(type);
    }
}

// 根据名称获取图元
std::vector<std::shared_ptr<GeometryPrimitive>> GeometryService::getPrimitivesByName(const std::string& name) {
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库查询
        return dao->findByName(name);
    } else {
        // 容器模式：从容器查询
        return container->findByName(name);
    }
}

// 根据颜色获取图元
std::vector<std::shared_ptr<GeometryPrimitive>> GeometryService::getPrimitivesByColor(const std::string& color) {
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库查询
        return dao->findByColor(color);
    } else {
        // 容器模式：从容器查询
        return container->findByColor(color);
    }
}

// 根据面积范围获取图元
std::vector<std::shared_ptr<GeometryPrimitive>> GeometryService::getPrimitivesByAreaRange(double minArea, double maxArea) {
    if (useDatabaseMode && dao) {
        // 数据库模式：获取所有图元然后过滤
        auto allPrimitives = dao->findAll();
        std::vector<std::shared_ptr<GeometryPrimitive>> result;
        for (auto primitive : allPrimitives) {
            double area = primitive->area();
            if (area >= minArea && area <= maxArea) {
                result.push_back(primitive);
            }
        }
        return result;
    } else {
        return container->findByAreaRange(minArea, maxArea);
    }
}

// 获取所有图元
std::vector<std::shared_ptr<GeometryPrimitive>> GeometryService::getAllPrimitives() {
    std::cout << "=== GeometryService::getAllPrimitives 调用开始 ===" << std::endl;
    std::vector<std::shared_ptr<GeometryPrimitive>> result;
    
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库查询
        std::cout << "使用数据库模式获取图元" << std::endl;
        result = dao->findAll();
    } else {
        // 容器模式：从容器查询
        std::cout << "使用容器模式获取图元" << std::endl;
        result = container->getAllPrimitives();
    }
    
    std::cout << "获取到 " << result.size() << " 个图元" << std::endl;
    for (size_t i = 0; i < result.size(); i++) {
        std::cout << "[" << i << "] ID: " << result[i]->getId() 
                  << ", Type: " << result[i]->getType()
                  << ", Name: " << result[i]->getName() << std::endl;
    }
    std::cout << "=== GeometryService::getAllPrimitives 调用结束 ===" << std::endl;
    
    return result;
}

// 获取所有图元的JSON表示
Json::Value GeometryService::getAllPrimitivesAsJson() {
    Json::Value result(Json::arrayValue);
    auto allPrimitives = getAllPrimitives();
    
    for (const auto& primitive : allPrimitives) {
        Json::Value primitiveJson;
        primitiveJson["id"] = primitive->getId();
        primitiveJson["type"] = primitive->getType();
        primitiveJson["name"] = primitive->getName();
        primitiveJson["color"] = primitive->getColor();
        
        // 添加位置信息
        auto position = primitive->getPosition();
        primitiveJson["x"] = position.x;
        primitiveJson["y"] = position.y;
        primitiveJson["z"] = position.z;
        
        // 根据类型添加特定属性
        std::string type = primitive->getType();
        if (type == "Circle") {
            auto circle = std::dynamic_pointer_cast<Circle>(primitive);
            if (circle) {
                primitiveJson["radius"] = circle->getRadius();
            }
        } else if (type == "Rectangle") {
            auto rectangle = std::dynamic_pointer_cast<Rectangle>(primitive);
            if (rectangle) {
                primitiveJson["width"] = rectangle->getWidth();
                primitiveJson["height"] = rectangle->getHeight();
            }
        } else if (type == "Cuboid") {
            auto cuboid = std::dynamic_pointer_cast<Cuboid>(primitive);
            if (cuboid) {
                primitiveJson["length"] = cuboid->getLength();
                primitiveJson["width"] = cuboid->getWidth();
                primitiveJson["height"] = cuboid->getHeight();
            }
        } else if (type == "Cylinder") {
            auto cylinder = std::dynamic_pointer_cast<Cylinder>(primitive);
            if (cylinder) {
                primitiveJson["radius"] = cylinder->getRadius();
                primitiveJson["height"] = cylinder->getHeight();
            }
        }
        
        result.append(primitiveJson);
    }
    
    return result;
}

// 获取统计信息
Json::Value GeometryService::getStatistics() {
    Json::Value stats;
    
    auto allPrimitives = getAllPrimitives();  // 使用统一的获取方法
    stats["total"] = static_cast<int>(allPrimitives.size());
    
    // 按类型统计
    std::map<std::string, int> typeCount;
    double totalArea = 0;
    
    for (const auto& primitive : allPrimitives) {
        typeCount[primitive->getType()]++;
        totalArea += primitive->area();
    }
    
    Json::Value typeStats;
    for (const auto& pair : typeCount) {
        typeStats[pair.first] = pair.second;
    }
    stats["byType"] = typeStats;
    
    stats["totalArea"] = totalArea;
    stats["averageArea"] = allPrimitives.empty() ? 0.0 : totalArea / allPrimitives.size();
    
    return stats;
}

// 按ID排序
void GeometryService::sortPrimitivesById() {
    if (!useDatabaseMode) {
        // 仅在容器模式下进行排序
        container->sortById();
    }
    // 数据库模式下排序由SQL查询处理
}

// 按面积排序
void GeometryService::sortPrimitivesByArea() {
    if (!useDatabaseMode) {
        // 仅在容器模式下进行排序
        container->sortByArea();
    }
    // 数据库模式下排序由SQL查询处理
}

// 按名称排序
void GeometryService::sortPrimitivesByName() {
    if (!useDatabaseMode) {
        // 仅在容器模式下进行排序
        container->sortByName();
    }
    // 数据库模式下排序由SQL查询处理
}

// 获取图元数量
size_t GeometryService::getPrimitiveCount() {
    if (useDatabaseMode && dao) {
        // 数据库模式：从数据库获取数量
        return dao->count();
    } else {
        // 容器模式：从容器获取数量
        return container->size();
    }
}

// 检查服务是否可用
bool GeometryService::isServiceAvailable() {
    if (useDatabaseMode) {
        return dao != nullptr;
    } else {
        return container != nullptr;
    }
}

// 检查数据库是否可用
bool GeometryService::isDatabaseAvailable() {
    if (!useDatabaseMode || !dao) {
        return false;  // 非数据库模式或dao未初始化
    }
    
    try {
        dao->count();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// 同步到数据库
bool GeometryService::syncToDatabase() {
    if (!useDatabaseMode || !dao) {
        return false;  // 非数据库模式或dao未初始化，无法同步
    }
    
    try {
        auto allPrimitives = container->getAllPrimitives();
        for (const auto& primitive : allPrimitives) {
            dao->save(primitive);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error syncing to database: " << e.what() << std::endl;
        return false;
    }
}

// 打印统计信息
void GeometryService::printStatistics() {
    if (useDatabaseMode && dao) {
        // 数据库模式：获取所有数据后打印统计
        auto allPrimitives = getAllPrimitives();
        std::cout << "=== 几何图元统计信息 ===" << std::endl;
        std::cout << "总数量: " << allPrimitives.size() << std::endl;
        
        std::map<std::string, int> typeCount;
        for (const auto& primitive : allPrimitives) {
            typeCount[primitive->getType()]++;
        }
        
        std::cout << "按类型统计:" << std::endl;
        for (const auto& pair : typeCount) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    } else {
        // 容器模式：直接使用容器的打印方法
        container->printStatistics();
    }
}