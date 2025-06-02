#ifndef GEOMETRY_DAO_H
#define GEOMETRY_DAO_H

#include "../models/GeometryPrimitive.h"
#include <mysql/mysql.h>
#include <vector>
#include <memory>
#include <string>
#include <json/json.h>

struct DatabaseConfig {
    std::string host = "localhost";
    std::string username = "root";
    std::string password = "";
    std::string database = "geometry_db";
    unsigned int port = 3306;
};

class GeometryDAO {
private:
    DatabaseConfig config;
    MYSQL* connection;
    
    void initConnection();
    

    
    std::shared_ptr<GeometryPrimitive> createGeometryFromResult(MYSQL_RES* result);
    
    std::string propertiesToJson(std::shared_ptr<GeometryPrimitive> primitive);
    
    void parsePropertiesFromJson(const std::string& jsonStr, std::shared_ptr<GeometryPrimitive> primitive);
    
    
public:
    GeometryDAO();
    GeometryDAO(const DatabaseConfig& dbConfig);
    
    ~GeometryDAO();
    
    bool save(std::shared_ptr<GeometryPrimitive> primitive);
    
    std::shared_ptr<GeometryPrimitive> findById(int id);
    
    std::vector<std::shared_ptr<GeometryPrimitive>> findAll();
    
    std::vector<std::shared_ptr<GeometryPrimitive>> findByType(const std::string& type);
    
    bool update(std::shared_ptr<GeometryPrimitive> primitive);
    bool deleteById(int id);
    bool deleteAll();
    int count();
};

#endif // GEOMETRY_DAO_H