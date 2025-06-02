#ifndef GEOMETRY_DAO_H
#define GEOMETRY_DAO_H

#include "../models/GeometryPrimitive.h"
#include <mysql/mysql.h>
#include <vector>
#include <memory>
#include <string>
#include <json/json.h>

// 数据库配置结构体
struct DatabaseConfig {
    std::string host = "localhost";
    std::string username = "root";
    std::string password = "";
    std::string database = "geometry_db";
    unsigned int port = 3306;
};

// 数据访问对象类 - 使用统一表结构
class GeometryDAO {
private:
    DatabaseConfig config;
    MYSQL* connection;
    // 不再需要静态ID生成器，使用数据库自增ID
    
    // 初始化数据库连接
    void initConnection();
    

    
    // 辅助函数：从结果集创建几何图元对象
    std::shared_ptr<GeometryPrimitive> createGeometryFromResult(MYSQL_RES* result);
    
    // 辅助函数：将属性转换为JSON字符串
    std::string propertiesToJson(std::shared_ptr<GeometryPrimitive> primitive);
    
    // 辅助函数：从JSON字符串解析属性
    void parsePropertiesFromJson(const std::string& jsonStr, std::shared_ptr<GeometryPrimitive> primitive);
    
    // 不再需要生成唯一ID方法，使用数据库自增ID
    
public:
    // 构造函数
    GeometryDAO();
    GeometryDAO(const DatabaseConfig& dbConfig);
    
    // 析构函数
    ~GeometryDAO();
    
    // 保存图元到数据库（统一表）
    bool save(std::shared_ptr<GeometryPrimitive> primitive);
    
    // 根据ID查找图元
    std::shared_ptr<GeometryPrimitive> findById(int id);
    
    // 查找所有图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findAll();
    
    // 根据类型查找图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findByType(const std::string& type);
    
    // 根据名称查找图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findByName(const std::string& name);
    
    // 根据颜色查找图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findByColor(const std::string& color);
    
    // 根据ID更新图元
    bool update(std::shared_ptr<GeometryPrimitive> primitive);
    
    // 根据ID删除图元
    bool deleteById(int id);
    
    // 删除所有图元
    bool deleteAll();
    
    // 获取图元总数
    int count();
    
    // 不再需要获取下一个可用ID方法，使用数据库自增ID
};

#endif // GEOMETRY_DAO_H