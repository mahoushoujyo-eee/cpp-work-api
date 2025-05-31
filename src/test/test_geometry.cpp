#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <chrono>
#include "../models/GeometryPrimitive.h"
#include "../models/Shape2D.h"
#include "../models/Shape3D.h"
#include "../service/GeometryService.h"

// 测试2D图元
void test2DShapes() {
    std::cout << "=== 测试2D图元 ===" << std::endl;
    
    // 测试矩形
    Position rectPos(10, 20, 0);
    auto rectangle = std::make_shared<Rectangle>("测试矩形", rectPos, "红色", 100, 50);
    
    std::cout << "矩形测试:" << std::endl;
    rectangle->print();
    assert(rectangle->area() == 5000); // 100 * 50
    assert(rectangle->perimeter() == 300); // 2 * (100 + 50)
    std::cout << "矩形测试通过!" << std::endl << std::endl;
    
    // 测试圆形
    Position circlePos(30, 40, 0);
    auto circle = std::make_shared<Circle>("测试圆形", circlePos, "蓝色", 25);
    
    std::cout << "圆形测试:" << std::endl;
    circle->print();
    double expectedArea = M_PI * 25 * 25;
    double expectedPerimeter = 2 * M_PI * 25;
    assert(abs(circle->area() - expectedArea) < 0.001);
    assert(abs(circle->perimeter() - expectedPerimeter) < 0.001);
    std::cout << "圆形测试通过!" << std::endl << std::endl;
}

// 测试3D图元
void test3DShapes() {
    std::cout << "=== 测试3D图元 ===" << std::endl;
    
    // 测试长方体
    Position cuboidPos(50, 60, 10);
    auto cuboid = std::make_shared<Cuboid>("测试长方体", cuboidPos, "绿色", 10, 8, 6);
    
    std::cout << "长方体测试:" << std::endl;
    cuboid->print();
    assert(cuboid->volume() == 480); // 10 * 8 * 6
    assert(cuboid->surfaceArea() == 376); // 2 * (10*8 + 10*6 + 8*6)
    std::cout << "长方体测试通过!" << std::endl << std::endl;
    
    // 测试圆柱体
    Position cylinderPos(70, 80, 20);
    auto cylinder = std::make_shared<Cylinder>("测试圆柱体", cylinderPos, "黄色", 5, 12);
    
    std::cout << "圆柱体测试:" << std::endl;
    cylinder->print();
    double expectedVolume = M_PI * 5 * 5 * 12;
    double expectedSurfaceArea = 2 * M_PI * 5 * (5 + 12);
    assert(abs(cylinder->volume() - expectedVolume) < 0.001);
    assert(abs(cylinder->surfaceArea() - expectedSurfaceArea) < 0.001);
    std::cout << "圆柱体测试通过!" << std::endl << std::endl;
}

// 测试容器类
void testContainer() {
    std::cout << "=== 测试容器类 ===" << std::endl;
    
    GeometryContainer container;
    
    // 创建测试图元
    auto rect1 = std::make_shared<Rectangle>("矩形1", Position(0, 0, 0), "红色", 10, 20);
    auto rect2 = std::make_shared<Rectangle>("矩形2", Position(10, 10, 0), "蓝色", 15, 25);
    auto circle1 = std::make_shared<Circle>("圆形1", Position(20, 20, 0), "绿色", 10);
    auto circle2 = std::make_shared<Circle>("圆形2", Position(30, 30, 0), "红色", 15);
    auto cuboid1 = std::make_shared<Cuboid>("长方体1", Position(40, 40, 0), "黄色", 5, 6, 7);
    
    // 测试添加功能
    container.addPrimitive(rect1);
    container.addPrimitive(rect2);
    container.addPrimitive(circle1);
    container.addPrimitive(circle2);
    container.addPrimitive(cuboid1);
    
    assert(container.size() == 5);
    std::cout << "添加图元测试通过!" << std::endl;
    
    // 测试查找功能
    auto foundRect = container.findById(rect1->getId());
    assert(foundRect != nullptr);
    assert(foundRect->getName() == "矩形1");
    std::cout << "按ID查找测试通过!" << std::endl;
    
    // 测试按类型查找
    auto rectangles = container.findByType("Rectangle");
    assert(rectangles.size() == 2);
    std::cout << "按类型查找测试通过!" << std::endl;
    
    // 测试按颜色查找
    auto redShapes = container.findByColor("红色");
    assert(redShapes.size() == 2);
    std::cout << "按颜色查找测试通过!" << std::endl;
    
    // 测试按面积范围查找
    auto mediumShapes = container.findByAreaRange(200, 400);
    assert(mediumShapes.size() >= 1);
    std::cout << "按面积范围查找测试通过!" << std::endl;
    
    // 测试删除功能
    bool deleted = container.deletePrimitive(rect1->getId());
    assert(deleted);
    assert(container.size() == 4);
    std::cout << "删除图元测试通过!" << std::endl;
    
    // 测试排序功能
    container.sortByArea();
    auto allPrimitives = container.getAllPrimitives();
    for (size_t i = 1; i < allPrimitives.size(); i++) {
        assert(allPrimitives[i-1]->area() <= allPrimitives[i]->area());
    }
    std::cout << "按面积排序测试通过!" << std::endl;
    
    // 打印容器统计信息
    container.printStatistics();
    
    std::cout << "容器类测试全部通过!" << std::endl << std::endl;
}

// 测试JSON序列化和反序列化
void testJsonSerialization() {
    std::cout << "=== 测试JSON序列化 ===" << std::endl;
    
    // 测试矩形JSON序列化
    auto rectangle = std::make_shared<Rectangle>("JSON矩形", Position(100, 200, 0), "紫色", 50, 30);
    Json::Value rectJson = rectangle->toJson();
    
    assert(rectJson["type"].asString() == "Rectangle");
    assert(rectJson["name"].asString() == "JSON矩形");
    assert(rectJson["width"].asDouble() == 50);
    assert(rectJson["height"].asDouble() == 30);
    assert(rectJson["area"].asDouble() == 1500);
    
    std::cout << "矩形JSON序列化测试通过!" << std::endl;
    
    // 测试圆形JSON序列化
    auto circle = std::make_shared<Circle>("JSON圆形", Position(150, 250, 0), "橙色", 20);
    Json::Value circleJson = circle->toJson();
    
    assert(circleJson["type"].asString() == "Circle");
    assert(circleJson["name"].asString() == "JSON圆形");
    assert(circleJson["radius"].asDouble() == 20);
    
    std::cout << "圆形JSON序列化测试通过!" << std::endl;
    
    // 测试3D图元JSON序列化
    auto cuboid = std::make_shared<Cuboid>("JSON长方体", Position(200, 300, 50), "粉色", 8, 6, 4);
    Json::Value cuboidJson = cuboid->toJson();
    
    assert(cuboidJson["type"].asString() == "Cuboid");
    assert(cuboidJson["volume"].asDouble() == 192);
    assert(cuboidJson["surfaceArea"].asDouble() == 208);
    
    std::cout << "长方体JSON序列化测试通过!" << std::endl;
    
    std::cout << "JSON序列化测试全部通过!" << std::endl << std::endl;
}

// 性能测试
void performanceTest() {
    std::cout << "=== 性能测试 ===" << std::endl;
    
    GeometryContainer container;
    const int numShapes = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 创建大量图元
    for (int i = 0; i < numShapes; i++) {
        if (i % 4 == 0) {
            auto rect = std::make_shared<Rectangle>("矩形" + std::to_string(i), 
                                                   Position(i, i, 0), "颜色" + std::to_string(i % 10), 
                                                   10 + i % 100, 20 + i % 50);
            container.addPrimitive(rect);
        } else if (i % 4 == 1) {
            auto circle = std::make_shared<Circle>("圆形" + std::to_string(i), 
                                                  Position(i, i, 0), "颜色" + std::to_string(i % 10), 
                                                  5 + i % 50);
            container.addPrimitive(circle);
        } else if (i % 4 == 2) {
            auto cuboid = std::make_shared<Cuboid>("长方体" + std::to_string(i), 
                                                  Position(i, i, i), "颜色" + std::to_string(i % 10), 
                                                  5 + i % 20, 6 + i % 15, 7 + i % 10);
            container.addPrimitive(cuboid);
        } else {
            auto cylinder = std::make_shared<Cylinder>("圆柱体" + std::to_string(i), 
                                                      Position(i, i, i), "颜色" + std::to_string(i % 10), 
                                                      3 + i % 15, 10 + i % 20);
            container.addPrimitive(cylinder);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "创建 " << numShapes << " 个图元耗时: " << duration.count() << " 毫秒" << std::endl;
    
    // 测试查找性能
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; i++) {
        auto rectangles = container.findByType("Rectangle");
        auto circles = container.findByType("Circle");
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "执行 1000 次类型查找耗时: " << duration.count() << " 毫秒" << std::endl;
    
    // 测试排序性能
    start = std::chrono::high_resolution_clock::now();
    container.sortByArea();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "对 " << numShapes << " 个图元按面积排序耗时: " << duration.count() << " 毫秒" << std::endl;
    
    std::cout << "性能测试完成!" << std::endl << std::endl;
}

int main() {
    std::cout << "开始几何图元系统测试..." << std::endl << std::endl;
    
    try {
        // 运行各项测试
        test2DShapes();
        test3DShapes();
        testContainer();
        testJsonSerialization();
        performanceTest();
        
        std::cout << "所有测试通过! 几何图元系统运行正常。" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}