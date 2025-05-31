#ifndef GEOMETRY_PRIMITIVE_H
#define GEOMETRY_PRIMITIVE_H

#include <string>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <cmath>

// 位置类
class Position {
public:
    double x, y, z;
    
    Position(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    
    Json::Value toJson() const;
    void fromJson(const Json::Value& json);
};

// 几何图元基类
class GeometryPrimitive {
protected:
    static int nextId;
    
public:
    int id;
    std::string name;
    Position position;
    std::string color;
    std::string type;
    
    // 构造函数
    GeometryPrimitive(const std::string& name, const Position& pos, const std::string& color, const std::string& type)
        : id(++nextId), name(name), position(pos), color(color), type(type) {}
    
    // 虚析构函数
    virtual ~GeometryPrimitive() = default;
    
    // 纯虚函数 - 计算面积
    virtual double area() const = 0;
    
    // 纯虚函数 - 打印特征数据
    virtual void print() const = 0;
    
    // 虚函数 - JSON序列化
    virtual Json::Value toJson() const;
    
    // 虚函数 - JSON反序列化
    virtual void fromJson(const Json::Value& json);
    
    // Getter方法
    int getId() const { return id; }
    std::string getName() const { return name; }
    Position getPosition() const { return position; }
    std::string getColor() const { return color; }
    std::string getType() const { return type; }
    
    // Setter方法
    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }
    void setPosition(const Position& newPos) { position = newPos; }
    void setColor(const std::string& newColor) { color = newColor; }
};

// 2D图元基类
class Shape2D : public GeometryPrimitive {
public:
    Shape2D(const std::string& name, const Position& pos, const std::string& color, const std::string& type)
        : GeometryPrimitive(name, pos, color, type) {}
    
    virtual ~Shape2D() = default;
    
    // 2D图元的周长计算（可选实现）
    virtual double perimeter() const = 0;
};

// 3D图元基类
class Shape3D : public GeometryPrimitive {
public:
    Shape3D(const std::string& name, const Position& pos, const std::string& color, const std::string& type)
        : GeometryPrimitive(name, pos, color, type) {}
    
    virtual ~Shape3D() = default;
    
    // 3D图元的体积计算
    virtual double volume() const = 0;
    
    // 3D图元的表面积计算
    virtual double surfaceArea() const = 0;
    
    // 重写area()方法，对于3D图元返回表面积
    double area() const override;
    
    Json::Value toJson() const override;
};

#endif // GEOMETRY_PRIMITIVE_H