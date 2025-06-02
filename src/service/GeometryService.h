#ifndef GEOMETRY_SERVICE_H
#define GEOMETRY_SERVICE_H

#include "../dao/GeometryDAO.h"
#include "../models/GeometryContainer.h"
#include <jsoncpp/json/json.h>
#include <memory>
#include <vector>
#include <map>

// 几何图元服务类
class GeometryService {
private:
    std::unique_ptr<GeometryDAO> dao;
    std::unique_ptr<GeometryContainer> container;
    bool useDatabaseMode;  // 标志是否使用数据库模式
    
    // 验证图元数据
    bool validatePrimitiveData(const Json::Value& data);
    
    // 从JSON创建图元对象
    std::shared_ptr<GeometryPrimitive> createPrimitiveFromJson(const Json::Value& data);
    
public:
    // 构造函数
    GeometryService();
    GeometryService(const DatabaseConfig& dbConfig);
    
    // 析构函数
    ~GeometryService();
    
    // 从数据库加载所有图元到容器
    void loadAllFromDatabase();
    
    // 创建图元
    std::shared_ptr<GeometryPrimitive> createPrimitive(const Json::Value& data);

    // 获取所有图元
    std::vector<std::shared_ptr<GeometryPrimitive>> getAllPrimitives();
    
    // 根据ID获取图元
    std::shared_ptr<GeometryPrimitive> getPrimitiveById(int id);
    
    // 更新图元
    bool updatePrimitive(int id, const Json::Value& data);
    
    // 删除图元
    bool deletePrimitive(int id);
    
    // 删除所有图元
    bool deleteAllPrimitives();
    
    // 根据类型获取图元
    std::vector<std::shared_ptr<GeometryPrimitive>> getPrimitivesByType(const std::string& type);
    
    // 获取所有图元的JSON表示
    Json::Value getAllPrimitivesAsJson();
    
    // 获取统计信息
    Json::Value getStatistics();
    
    // 获取图元数量
    size_t getPrimitiveCount();
    
    // 检查服务是否可用
    bool isServiceAvailable();
    
    // 检查数据库是否可用
    bool isDatabaseAvailable();
    
    // 打印统计信息
    void printStatistics();
};

#endif // GEOMETRY_SERVICE_H