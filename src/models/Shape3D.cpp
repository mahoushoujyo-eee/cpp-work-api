#include "Shape3D.h"

double Cuboid::volume() const {
    return length * width * height;
}

double Cuboid::surfaceArea() const {
    return 2 * (length * width + length * height + width * height);
}

void Cuboid::print() const {
    std::cout << "Cuboid [ID: " << id << ", Name: " << name 
              << ", Position: (" << position.x << ", " << position.y << ", " << position.z << ")"
              << ", Color: " << color << ", Length: " << length 
              << ", Width: " << width << ", Height: " << height
              << ", Volume: " << volume() << ", Surface Area: " << surfaceArea() << "]" << std::endl;
}

Json::Value Cuboid::toJson() const {
    Json::Value json = Shape3D::toJson();
    json["length"] = length;
    json["width"] = width;
    json["height"] = height;
    return json;
}

void Cuboid::fromJson(const Json::Value& json) {
    Shape3D::fromJson(json);
    length = json["length"].asDouble();
    width = json["width"].asDouble();
    height = json["height"].asDouble();
}

double Cylinder::volume() const {
    return M_PI * radius * radius * height;
}

double Cylinder::surfaceArea() const {
    return 2 * M_PI * radius * (radius + height);
}

void Cylinder::print() const {
    std::cout << "Cylinder [ID: " << id << ", Name: " << name 
              << ", Position: (" << position.x << ", " << position.y << ", " << position.z << ")"
              << ", Color: " << color << ", Radius: " << radius << ", Height: " << height
              << ", Volume: " << volume() << ", Surface Area: " << surfaceArea() << "]" << std::endl;
}

Json::Value Cylinder::toJson() const {
    Json::Value json = Shape3D::toJson();
    json["radius"] = radius;
    json["height"] = height;
    return json;
}

void Cylinder::fromJson(const Json::Value& json) {
    Shape3D::fromJson(json);
    radius = json["radius"].asDouble();
    height = json["height"].asDouble();
}