#ifndef GEOMETRY_CONTROLLER_H
#define GEOMETRY_CONTROLLER_H

#include "../service/GeometryService.h"
#include "../httplib.h"
#include <jsoncpp/json/json.h>
#include <memory>
#include <string>

class GeometryController {
private:
    std::shared_ptr<GeometryService> service;
    httplib::Server server;
    
    void setCorsHeaders(httplib::Response& res);
    Json::Value parseJson(const std::string& body);
    std::string serializeJson(const Json::Value& json);
    void sendErrorResponse(httplib::Response& res, int statusCode, const std::string& message);
    void sendSuccessResponse(httplib::Response& res, const Json::Value& data);
    
public:
    GeometryController();
    GeometryController(const DatabaseConfig& dbConfig);
    
    void setService(std::shared_ptr<GeometryService> servicePtr);
    
    void registerRoutes(httplib::Server& server);
    
    void start(const std::string& host = "0.0.0.0", int port = 58010);
    
    void stop();
};

#endif // GEOMETRY_CONTROLLER_H