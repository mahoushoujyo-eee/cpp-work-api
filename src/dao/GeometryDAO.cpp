#include "GeometryDAO.h"
#include "../models/Shape2D.h"
#include "../models/Shape3D.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

std::string GeometryDAO::propertiesToJson(std::shared_ptr<GeometryPrimitive> primitive) {
    Json::Value properties;
    std::string type = primitive->getType();
    
    if (type == "Rectangle") {
        auto rect = std::dynamic_pointer_cast<Rectangle>(primitive);
        properties["width"] = rect->getWidth();
        properties["height"] = rect->getHeight();
    } else if (type == "Circle") {
        auto circle = std::dynamic_pointer_cast<Circle>(primitive);
        properties["radius"] = circle->getRadius();
    } else if (type == "Cuboid") {
        auto cuboid = std::dynamic_pointer_cast<Cuboid>(primitive);
        properties["width"] = cuboid->getWidth();
        properties["height"] = cuboid->getHeight();
        properties["depth"] = cuboid->getLength();
    } else if (type == "Cylinder") {
        auto cylinder = std::dynamic_pointer_cast<Cylinder>(primitive);
        properties["radius"] = cylinder->getRadius();
        properties["height"] = cylinder->getHeight();
    }
    
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, properties);
}

void GeometryDAO::parsePropertiesFromJson(const std::string& jsonStr, std::shared_ptr<GeometryPrimitive> primitive) {
    Json::CharReaderBuilder builder;
    Json::Value properties;
    std::string errors;
    
    std::istringstream stream(jsonStr);
    if (!Json::parseFromStream(builder, stream, &properties, &errors)) {
        std::cerr << "Failed to parse JSON properties: " << errors << std::endl;
        return;
    }
    
    std::string type = primitive->getType();
    
    if (type == "Rectangle") {
        auto rect = std::dynamic_pointer_cast<Rectangle>(primitive);
        if (properties.isMember("width") && properties.isMember("height")) {
        }
    } else if (type == "Circle") {
        auto circle = std::dynamic_pointer_cast<Circle>(primitive);
        if (properties.isMember("radius")) {
        }
    } else if (type == "Cuboid") {
        auto cuboid = std::dynamic_pointer_cast<Cuboid>(primitive);
        if (properties.isMember("width") && properties.isMember("height") && properties.isMember("depth")) {
        }
    } else if (type == "Cylinder") {
        auto cylinder = std::dynamic_pointer_cast<Cylinder>(primitive);
        if (properties.isMember("radius") && properties.isMember("height")) {
        }
    }
}

std::shared_ptr<GeometryPrimitive> GeometryDAO::createGeometryFromResult(MYSQL_RES* result) {
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        return nullptr;
    }
    
    int id = std::atoi(row[0]);
    std::string name = row[1] ? row[1] : "";
    std::string type = row[2] ? row[2] : "";
    std::string color = row[3] ? row[3] : "";
    double x = row[4] ? std::atof(row[4]) : 0.0;
    double y = row[5] ? std::atof(row[5]) : 0.0;
    double z = row[6] ? std::atof(row[6]) : 0.0;
    std::string propertiesJson = row[7] ? row[7] : "{}";
    
    Json::CharReaderBuilder builder;
    Json::Value properties;
    std::string errors;
    std::istringstream stream(propertiesJson);
    
    if (!Json::parseFromStream(builder, stream, &properties, &errors)) {
        std::cerr << "Failed to parse JSON properties: " << errors << std::endl;
        return nullptr;
    }
    
    std::shared_ptr<GeometryPrimitive> primitive = nullptr;
    
    if (type == "Rectangle") {
        double width = properties["width"].asDouble();
        double height = properties["height"].asDouble();
        primitive = std::make_shared<Rectangle>(name, Position(x, y, z), color, width, height);
        primitive->setId(id);
    } else if (type == "Circle") {
        double radius = properties["radius"].asDouble();
        primitive = std::make_shared<Circle>(name, Position(x, y, z), color, radius);
        primitive->setId(id);
    } else if (type == "Cuboid") {
        double width = properties["width"].asDouble();
        double height = properties["height"].asDouble();
        double depth = properties["depth"].asDouble();
        primitive = std::make_shared<Cuboid>(name, Position(x, y, z), color, width, height, depth);
        primitive->setId(id);
    } else if (type == "Cylinder") {
        double radius = properties["radius"].asDouble();
        double height = properties["height"].asDouble();
        primitive = std::make_shared<Cylinder>(name, Position(x, y, z), color, radius, height);
        primitive->setId(id);
    }
    
    
    return primitive;
}

GeometryDAO::GeometryDAO() : connection(nullptr) {
    try {
        initConnection();
    } catch (const std::exception& e) {
        std::cerr << "GeometryDAO initialization failed: " << e.what() << std::endl;
    }
}

GeometryDAO::GeometryDAO(const DatabaseConfig& dbConfig) : config(dbConfig), connection(nullptr) {
    try {
        initConnection();
    } catch (const std::exception& e) {
        std::cerr << "GeometryDAO initialization failed: " << e.what() << std::endl;
    }
}

GeometryDAO::~GeometryDAO() {
    if (connection) {
        mysql_close(connection);
    }
}

bool GeometryDAO::save(std::shared_ptr<GeometryPrimitive> primitive) {
    if (!connection || !primitive) {
        return false;
    }
        
    std::string properties = propertiesToJson(primitive);
    
    std::string sql = "INSERT INTO geometry (name, type, color, x, y, z, properties) VALUES (?, ?, ?, ?, ?, ?, ?)";
    
    MYSQL_STMT* stmt = mysql_stmt_init(connection);
    if (!stmt) {
        return false;
    }
    
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        mysql_stmt_close(stmt);
        return false;
    }
    
    MYSQL_BIND bind[7];
    memset(bind, 0, sizeof(bind));
    
    std::string name = primitive->getName();
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = const_cast<char*>(name.c_str());
    bind[0].buffer_length = name.length();
    
    std::string type = primitive->getType();
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = const_cast<char*>(type.c_str());
    bind[1].buffer_length = type.length();
    
    std::string color = primitive->getColor();
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = const_cast<char*>(color.c_str());
    bind[2].buffer_length = color.length();
    
    double x = primitive->getPosition().x;
    double y = primitive->getPosition().y;
    double z = primitive->getPosition().z;
    bind[3].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[3].buffer = &x;
    bind[4].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[4].buffer = &y;
    bind[5].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[5].buffer = &z;
    
    bind[6].buffer_type = MYSQL_TYPE_STRING;
    bind[6].buffer = const_cast<char*>(properties.c_str());
    bind[6].buffer_length = properties.length();
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        mysql_stmt_close(stmt);
        return false;
    }
    
    bool success = (mysql_stmt_execute(stmt) == 0);
    
    if (success) {
        my_ulonglong insertId = mysql_stmt_insert_id(stmt);
        primitive->setId(static_cast<int>(insertId));
    }
    
    mysql_stmt_close(stmt);
    return success;
}

std::shared_ptr<GeometryPrimitive> GeometryDAO::findById(int id) {
    if (!connection) {
        return nullptr;
    }
    
    std::string query = "SELECT id, name, type, color, x, y, z, properties FROM geometry WHERE id = " + std::to_string(id);
    if (mysql_query(connection, query.c_str())) {
        return nullptr;
    }
    
    MYSQL_RES* res = mysql_store_result(connection);
    if (!res) {
        return nullptr;
    }
    
    std::shared_ptr<GeometryPrimitive> primitive = createGeometryFromResult(res);
    mysql_free_result(res);
    
    return primitive;
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryDAO::findAll() {
    std::vector<std::shared_ptr<GeometryPrimitive>> result;
    
    if (!connection) {
        return result;
    }
    
    std::string sql = "SELECT id, name, type, color, x, y, z, properties FROM geometry ORDER BY id";
    
    if (mysql_query(connection, sql.c_str())) {
        return result;
    }
    
    MYSQL_RES* res = mysql_store_result(connection);
    if (!res) {
        return result;
    }
    
    std::shared_ptr<GeometryPrimitive> primitive;
    while ((primitive = createGeometryFromResult(res))) {
        result.push_back(primitive);
    }
    
    mysql_free_result(res);
    return result;
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryDAO::findByType(const std::string& type) {
    std::vector<std::shared_ptr<GeometryPrimitive>> result;
    
    if (!connection) {
        return result;
    }
    
    std::string query = "SELECT id, name, type, color, x, y, z, properties FROM geometry WHERE type = '" + type + "' ORDER BY id";
    if (mysql_query(connection, query.c_str())) {
        return result;
    }
    
    MYSQL_RES* res = mysql_store_result(connection);
    if (!res) {
        return result;
    }
    
    std::shared_ptr<GeometryPrimitive> primitive;
    while ((primitive = createGeometryFromResult(res))) {
        result.push_back(primitive);
    }
    
    mysql_free_result(res);
    return result;
}


bool GeometryDAO::update(std::shared_ptr<GeometryPrimitive> primitive) {
    if (!connection || !primitive) {
        return false;
    }
    
    std::string properties = propertiesToJson(primitive);
    
    std::string sql = "UPDATE geometry SET name = ?, type = ?, color = ?, x = ?, y = ?, z = ?, properties = ? WHERE id = ?";
    
    MYSQL_STMT* stmt = mysql_stmt_init(connection);
    if (!stmt) {
        return false;
    }
    
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        mysql_stmt_close(stmt);
        return false;
    }
    
    MYSQL_BIND bind[8];
    memset(bind, 0, sizeof(bind));
    
    std::string name = primitive->getName();
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = const_cast<char*>(name.c_str());
    bind[0].buffer_length = name.length();
    
    std::string type = primitive->getType();
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = const_cast<char*>(type.c_str());
    bind[1].buffer_length = type.length();
    
    std::string color = primitive->getColor();
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = const_cast<char*>(color.c_str());
    bind[2].buffer_length = color.length();
    
    double x = primitive->getPosition().x;
    double y = primitive->getPosition().y;
    double z = primitive->getPosition().z;
    bind[3].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[3].buffer = &x;
    bind[4].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[4].buffer = &y;
    bind[5].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[5].buffer = &z;
    
    bind[6].buffer_type = MYSQL_TYPE_STRING;
    bind[6].buffer = const_cast<char*>(properties.c_str());
    bind[6].buffer_length = properties.length();
    
    int id = primitive->getId();
    bind[7].buffer_type = MYSQL_TYPE_LONG;
    bind[7].buffer = &id;
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        mysql_stmt_close(stmt);
        return false;
    }
    
    bool success = false;
    if (mysql_stmt_execute(stmt) == 0) {
        my_ulonglong affected = mysql_stmt_affected_rows(stmt);
        success = (affected > 0);
    }
    
    mysql_stmt_close(stmt);
    return success;
}

bool GeometryDAO::deleteById(int id) {
    if (!connection) {
        return false;
    }
    
    std::string sql = "DELETE FROM geometry WHERE id = ?";
    
    MYSQL_STMT* stmt = mysql_stmt_init(connection);
    if (!stmt) {
        return false;
    }
    
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        mysql_stmt_close(stmt);
        return false;
    }
    
    MYSQL_BIND bind;
    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = MYSQL_TYPE_LONG;
    bind.buffer = &id;
    
    if (mysql_stmt_bind_param(stmt, &bind)) {
        mysql_stmt_close(stmt);
        return false;
    }
    
    bool success = false;
    if (mysql_stmt_execute(stmt) == 0) {
        my_ulonglong affected = mysql_stmt_affected_rows(stmt);
        success = (affected > 0);
    }
    
    mysql_stmt_close(stmt);
    return success;
}

bool GeometryDAO::deleteAll() {
    if (!connection) {
        return false;
    }
    
    std::string sql = "DELETE FROM geometry";
    
    if (mysql_query(connection, sql.c_str())) {
        return false;
    }
        
    return true;
}

int GeometryDAO::count() {
    if (!connection) {
        return 0;
    }
    
    std::string sql = "SELECT COUNT(*) FROM geometry";
    
    if (mysql_query(connection, sql.c_str())) {
        return 0;
    }
    
    MYSQL_RES* res = mysql_store_result(connection);
    if (!res) {
        return 0;
    }
    
    int total = 0;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row && row[0]) {
        total = std::atoi(row[0]);
    }
    
    mysql_free_result(res);
    return total;
}

void GeometryDAO::initConnection() {
    connection = mysql_init(nullptr);
    if (!connection) {
        throw std::runtime_error("Failed to initialize MySQL connection");
    }
    
    if (!mysql_real_connect(connection, 
                           config.host.c_str(), 
                           config.username.c_str(), 
                           config.password.c_str(), 
                           config.database.c_str(), 
                           config.port, 
                           nullptr, 0)) {
        mysql_close(connection);
        connection = nullptr;
        std::string error_msg = "Failed to connect to MySQL database: ";
        error_msg += mysql_error(connection ? connection : mysql_init(nullptr));
        throw std::runtime_error(error_msg);
    }
}