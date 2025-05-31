#include "GeometryPrimitive.h"

// 初始化静态成员变量
int GeometryPrimitive::nextId = 0;

// GeometryPrimitive类方法实现
Json::Value GeometryPrimitive::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["name"] = name;
    json["position"] = position.toJson();
    json["color"] = color;
    json["type"] = type;
    json["area"] = area();
    return json;
}

void GeometryPrimitive::fromJson(const Json::Value& json) {
    name = json["name"].asString();
    position.fromJson(json["position"]);
    color = json["color"].asString();
}

// Position类方法实现
Json::Value Position::toJson() const {
    Json::Value json;
    json["x"] = x;
    json["y"] = y;
    json["z"] = z;
    return json;
}

void Position::fromJson(const Json::Value& json) {
    x = json["x"].asDouble();
    y = json["y"].asDouble();
    z = json["z"].asDouble();
}

// Shape3D类方法实现
double Shape3D::area() const {
    return surfaceArea();
}

Json::Value Shape3D::toJson() const {
    Json::Value json = GeometryPrimitive::toJson();
    json["volume"] = volume();
    json["surfaceArea"] = surfaceArea();
    return json;
}