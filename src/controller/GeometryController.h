#ifndef GEOMETRY_CONTROLLER_H
#define GEOMETRY_CONTROLLER_H

#include "../service/GeometryService.h"
#include "../httplib.h"
#include <jsoncpp/json/json.h>
#include <memory>
#include <string>

// HTTP控制器类
class GeometryController {
private:
    std::shared_ptr<GeometryService> service;
    httplib::Server server;
    
    // 辅助函数
    void setCorsHeaders(httplib::Response& res);
    Json::Value parseJson(const std::string& body);
    std::string serializeJson(const Json::Value& json);
    void sendErrorResponse(httplib::Response& res, int statusCode, const std::string& message);
    void sendSuccessResponse(httplib::Response& res, const Json::Value& data);
    
public:
    // 构造函数
    GeometryController();
    GeometryController(const DatabaseConfig& dbConfig);
    
    // 设置服务实例
    void setService(std::shared_ptr<GeometryService> servicePtr);
    
    // 注册路由
    void registerRoutes(httplib::Server& server);
    
    // 启动服务器
    void start(const std::string& host = "0.0.0.0", int port = 58010);
    
    // 停止服务器
    void stop();
};

#endif // GEOMETRY_CONTROLLER_H