#ifndef SHAPE3D_H
#define SHAPE3D_H

#include "GeometryPrimitive.h"
#include <cmath>

// 长方体类
class Cuboid : public Shape3D {
private:
    double length;
    double width;
    double height;
    
public:
    // 构造函数
    Cuboid(const std::string& name, const Position& pos, const std::string& color, 
           double length, double width, double height)
        : Shape3D(name, pos, color, "Cuboid"), length(length), width(width), height(height) {}
    
    // 析构函数
    ~Cuboid() override = default;
    
    // 计算体积
    double volume() const override;
    
    // 计算表面积
    double surfaceArea() const override;
    
    // 打印特征数据
    void print() const override;
    
    // JSON序列化
    Json::Value toJson() const override;
    
    // JSON反序列化
    void fromJson(const Json::Value& json) override;
    
    // Getter和Setter方法
    double getLength() const { return length; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    void setLength(double newLength) { length = newLength; }
    void setWidth(double newWidth) { width = newWidth; }
    void setHeight(double newHeight) { height = newHeight; }
};

// 圆柱体类
class Cylinder : public Shape3D {
private:
    double radius;
    double height;
    
public:
    // 构造函数
    Cylinder(const std::string& name, const Position& pos, const std::string& color, 
             double radius, double height)
        : Shape3D(name, pos, color, "Cylinder"), radius(radius), height(height) {}
    
    // 析构函数
    ~Cylinder() override = default;
    
    // 计算体积
    double volume() const override;
    
    // 计算表面积
    double surfaceArea() const override;
    
    // 打印特征数据
    void print() const override;
    
    // JSON序列化
    Json::Value toJson() const override;
    
    // JSON反序列化
    void fromJson(const Json::Value& json) override;
    
    // Getter和Setter方法
    double getRadius() const { return radius; }
    double getHeight() const { return height; }
    void setRadius(double newRadius) { radius = newRadius; }
    void setHeight(double newHeight) { height = newHeight; }
};

#endif // SHAPE3D_H