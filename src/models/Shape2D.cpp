#include "Shape2D.h"

// Rectangle类方法实现
double Rectangle::area() const {
    return width * height;
}

double Rectangle::perimeter() const {
    return 2 * (width + height);
}

void Rectangle::print() const {
    std::cout << "Rectangle [ID: " << id << ", Name: " << name 
              << ", Position: (" << position.x << ", " << position.y << ", " << position.z << ")"
              << ", Color: " << color << ", Width: " << width << ", Height: " << height
              << ", Area: " << area() << ", Perimeter: " << perimeter() << "]" << std::endl;
}

Json::Value Rectangle::toJson() const {
    Json::Value json = Shape2D::toJson();
    json["width"] = width;
    json["height"] = height;
    json["perimeter"] = perimeter();
    return json;
}

void Rectangle::fromJson(const Json::Value& json) {
    Shape2D::fromJson(json);
    width = json["width"].asDouble();
    height = json["height"].asDouble();
}

// Circle类方法实现
double Circle::area() const {
    return M_PI * radius * radius;
}

double Circle::perimeter() const {
    return 2 * M_PI * radius;
}

void Circle::print() const {
    std::cout << "Circle [ID: " << id << ", Name: " << name 
              << ", Position: (" << position.x << ", " << position.y << ", " << position.z << ")"
              << ", Color: " << color << ", Radius: " << radius
              << ", Area: " << area() << ", Perimeter: " << perimeter() << "]" << std::endl;
}

Json::Value Circle::toJson() const {
    Json::Value json = Shape2D::toJson();
    json["radius"] = radius;
    json["perimeter"] = perimeter();
    return json;
}

void Circle::fromJson(const Json::Value& json) {
    Shape2D::fromJson(json);
    radius = json["radius"].asDouble();
}