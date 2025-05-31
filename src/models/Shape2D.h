#ifndef SHAPE2D_H
#define SHAPE2D_H

#include "GeometryPrimitive.h"
#include <cmath>

// 矩形类
class Rectangle : public Shape2D {
private:
    double width;
    double height;
    
public:
    // 构造函数
    Rectangle(const std::string& name, const Position& pos, const std::string& color, double width, double height)
        : Shape2D(name, pos, color, "Rectangle"), width(width), height(height) {}
    
    // 析构函数
    ~Rectangle() override = default;
    
    // 计算面积
    double area() const override;
    
    // 计算周长
    double perimeter() const override;
    
    // 打印特征数据
    void print() const override;
    
    // JSON序列化
    Json::Value toJson() const override;
    
    // JSON反序列化
    void fromJson(const Json::Value& json) override;
    
    // Getter和Setter方法
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    void setWidth(double newWidth) { width = newWidth; }
    void setHeight(double newHeight) { height = newHeight; }
};

// 圆形类
class Circle : public Shape2D {
private:
    double radius;
    
public:
    // 构造函数
    Circle(const std::string& name, const Position& pos, const std::string& color, double radius)
        : Shape2D(name, pos, color, "Circle"), radius(radius) {}
    
    // 析构函数
    ~Circle() override = default;
    
    // 计算面积
    double area() const override;
    
    // 计算周长
    double perimeter() const override;
    
    // 打印特征数据
    void print() const override;
    
    // JSON序列化
    Json::Value toJson() const override;
    
    // JSON反序列化
    void fromJson(const Json::Value& json) override;
    
    // Getter和Setter方法
    double getRadius() const { return radius; }
    void setRadius(double newRadius) { radius = newRadius; }
};

#endif // SHAPE2D_H