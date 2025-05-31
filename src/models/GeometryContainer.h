#ifndef GEOMETRY_CONTAINER_H
#define GEOMETRY_CONTAINER_H

#include "GeometryPrimitive.h"
#include "Shape2D.h"
#include "Shape3D.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>

// 线性表容器类
class GeometryContainer {
private:
    // 双向链表节点定义
    struct Node {
        std::shared_ptr<GeometryPrimitive> data;
        Node* prev;
        Node* next;
        
        Node(std::shared_ptr<GeometryPrimitive> data) : data(data), prev(nullptr), next(nullptr) {}
        
        // 显式析构函数
        ~Node() {
            data.reset();
        }
    };
    
    Node* head;
    Node* tail;
    size_t count;
    
    // 辅助方法：将链表转换为向量
    std::vector<std::shared_ptr<GeometryPrimitive>> toVector() const;
    
    // 辅助方法：根据条件查找节点并返回向量
    std::vector<std::shared_ptr<GeometryPrimitive>> findNodes(
        std::function<bool(const std::shared_ptr<GeometryPrimitive>&)> condition) const;
    
    // 辅助方法：从向量重建链表
    void rebuildFromVector(const std::vector<std::shared_ptr<GeometryPrimitive>>& vec);

public:
    // 构造函数
    GeometryContainer() : head(nullptr), tail(nullptr), count(0) {}
    
    // 析构函数
    ~GeometryContainer() {
        clear();
    }
    
    // 添加图元
    void addPrimitive(std::shared_ptr<GeometryPrimitive> primitive);
    
    // 根据ID删除图元
    bool deletePrimitive(int id);
    
    // 根据索引删除图元
    bool deletePrimitiveByIndex(size_t index);
    
    // 修改图元（通过ID查找并替换）
    bool updatePrimitive(int id, std::shared_ptr<GeometryPrimitive> newPrimitive);
    
    // 根据ID查询图元
    std::shared_ptr<GeometryPrimitive> findById(int id) const;
    
    // 根据名称查询图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findByName(const std::string& name) const;
    
    // 根据类型查询图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findByType(const std::string& type) const;
    
    // 根据颜色查询图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findByColor(const std::string& color) const;
    
    // 根据面积范围查询图元
    std::vector<std::shared_ptr<GeometryPrimitive>> findByAreaRange(double minArea, double maxArea) const;
    
    // 自定义查询条件
    std::vector<std::shared_ptr<GeometryPrimitive>> findByCondition(
        std::function<bool(const std::shared_ptr<GeometryPrimitive>&)> condition) const;
    
    // 获取所有图元
    std::vector<std::shared_ptr<GeometryPrimitive>> getAllPrimitives() const;
    
    // 获取图元数量
    size_t size() const;
    
    // 检查容器是否为空
    bool empty() const;
    
    // 清空容器
    void clear();
    
    // 根据索引获取图元
    std::shared_ptr<GeometryPrimitive> at(size_t index) const;
    
    // 打印所有图元信息
    void printAll() const;
    
    // 获取统计信息
    void printStatistics() const;
    
    // 排序功能
    void sortById();
    void sortByArea();
    void sortByName();
};

#endif // GEOMETRY_CONTAINER_H