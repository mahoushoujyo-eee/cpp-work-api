#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "../models/GeometryPrimitive.h"
#include "../models/Shape2D.h"
#include "../models/Shape3D.h"
#include "../models/GeometryContainer.h"

using namespace std;

class InteractiveConsole {
private:
    GeometryContainer container;
    
public:
    // 构造函数
    InteractiveConsole() : container() {
        loadFromJson();
    }
    
    // 析构函数
    ~InteractiveConsole() {
        // 数据保存已在退出时手动处理
    }
    
    // 从JSON文件加载数据
    void loadFromJson() {
        ifstream file("geometry_data.json");
        if (!file.is_open()) {
            cout << "未找到geometry_data.json文件，将从空容器开始。" << endl;
            return;
        }
        
        Json::Value root;
        Json::Reader reader;
        
        if (!reader.parse(file, root)) {
            cout << "JSON文件解析失败: " << reader.getFormattedErrorMessages() << endl;
            file.close();
            return;
        }
        
        file.close();
        
        if (!root.isArray()) {
            cout << "JSON文件格式错误，应为数组格式。" << endl;
            return;
        }
        
        int loadedCount = 0;
        for (const auto& item : root) {
            try {
                shared_ptr<GeometryPrimitive> geometry = createGeometryFromJson(item);
                if (geometry) {
                    container.addPrimitive(geometry);
                    loadedCount++;
                }
            } catch (const exception& e) {
                cout << "加载图元时出错: " << e.what() << endl;
            }
        }
        
        cout << "成功从JSON文件加载了 " << loadedCount << " 个图元。" << endl;
    }
    
    // 从JSON对象创建几何图元
    shared_ptr<GeometryPrimitive> createGeometryFromJson(const Json::Value& json) {
        if (!json.isMember("type") || !json.isMember("name") || 
            !json.isMember("color") || !json.isMember("x") || 
            !json.isMember("y") || !json.isMember("z")) {
            return nullptr;
        }
        
        string type = json["type"].asString();
        string name = json["name"].asString();
        string color = json["color"].asString();
        double x = json["x"].asDouble();
        double y = json["y"].asDouble();
        double z = json["z"].asDouble();
        
        Position pos(x, y, z);
        
        if (type == "Rectangle") {
            if (!json.isMember("width") || !json.isMember("height")) {
                return nullptr;
            }
            double width = json["width"].asDouble();
            double height = json["height"].asDouble();
            auto rect = make_shared<Rectangle>(name, pos, color, width, height);
            if (json.isMember("id")) {
                rect->setId(json["id"].asInt());
            }
            return rect;
        }
        else if (type == "Circle") {
            if (!json.isMember("radius")) {
                return nullptr;
            }
            double radius = json["radius"].asDouble();
            auto circle = make_shared<Circle>(name, pos, color, radius);
            if (json.isMember("id")) {
                circle->setId(json["id"].asInt());
            }
            return circle;
        }
        else if (type == "Cuboid") {
            if (!json.isMember("length") || !json.isMember("width") || !json.isMember("height")) {
                return nullptr;
            }
            double length = json["length"].asDouble();
            double width = json["width"].asDouble();
            double height = json["height"].asDouble();
            auto cuboid = make_shared<Cuboid>(name, pos, color, length, width, height);
            if (json.isMember("id")) {
                cuboid->setId(json["id"].asInt());
            }
            return cuboid;
        }
        else if (type == "Cylinder") {
            if (!json.isMember("radius") || !json.isMember("height")) {
                return nullptr;
            }
            double radius = json["radius"].asDouble();
            double height = json["height"].asDouble();
            auto cylinder = make_shared<Cylinder>(name, pos, color, radius, height);
            if (json.isMember("id")) {
                cylinder->setId(json["id"].asInt());
            }
            return cylinder;
        }
        
        return nullptr;
    }
    
    // 保存数据到JSON文件
    void saveToJson() {
        ofstream file("geometry_data.json");
        if (!file.is_open()) {
            cout << "无法打开geometry_data.json文件进行保存。" << endl;
            return;
        }
        
        Json::Value root(Json::arrayValue);
        auto allGeometries = container.getAllPrimitives();
        
        for (const auto& geometry : allGeometries) {
            Json::Value geometryJson;
            geometryJson["id"] = geometry->getId();
            geometryJson["type"] = geometry->getType();
            geometryJson["name"] = geometry->getName();
            geometryJson["color"] = geometry->getColor();
            
            Position pos = geometry->getPosition();
            geometryJson["x"] = pos.x;
            geometryJson["y"] = pos.y;
            geometryJson["z"] = pos.z;
            
            // 根据类型添加特定属性
            if (geometry->getType() == "Rectangle") {
                auto rect = dynamic_pointer_cast<Rectangle>(geometry);
                if (rect) {
                    geometryJson["width"] = rect->getWidth();
                    geometryJson["height"] = rect->getHeight();
                }
            }
            else if (geometry->getType() == "Circle") {
                auto circle = dynamic_pointer_cast<Circle>(geometry);
                if (circle) {
                    geometryJson["radius"] = circle->getRadius();
                }
            }
            else if (geometry->getType() == "Cuboid") {
                auto cuboid = dynamic_pointer_cast<Cuboid>(geometry);
                if (cuboid) {
                    geometryJson["length"] = cuboid->getLength();
                    geometryJson["width"] = cuboid->getWidth();
                    geometryJson["height"] = cuboid->getHeight();
                }
            }
            else if (geometry->getType() == "Cylinder") {
                auto cylinder = dynamic_pointer_cast<Cylinder>(geometry);
                if (cylinder) {
                    geometryJson["radius"] = cylinder->getRadius();
                    geometryJson["height"] = cylinder->getHeight();
                }
            }
            
            root.append(geometryJson);
        }
        
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "  ";
        unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        writer->write(root, &file);
        file.close();
        
        cout << "数据已保存到geometry_data.json文件，共保存 " << allGeometries.size() << " 个图元。" << endl;
    }
    
    void run() {
        cout << "=== 几何图元交互式控制台程序 ===" << endl;
        cout << "欢迎使用几何图元管理系统!" << endl << endl;
        
        while (true) {
            showMainMenu();
            int choice = getChoice();
            
            switch (choice) {
                case 1: createGeometry(); break;
                case 2: viewGeometries(); break;
                case 3: searchGeometries(); break;
                case 4: modifyGeometry(); break;
                case 5: deleteGeometry(); break;
                case 6: sortGeometries(); break;
                case 7: showStatistics(); break;
                case 8: exportToJson(); break;
                case 9: clearContainer(); break;
                case 0: 
                    cout << "正在保存数据..." << endl;
                    saveToJson();
                    cout << "感谢使用几何图元管理系统，再见!" << endl;
                    return;
                default:
                    cout << "无效选择，请重新输入!" << endl;
            }
            
            cout << "\n按回车键继续...";
            cin.ignore();
            cin.get();
        }
    }
    
private:
    void showMainMenu() {
        cout << "\n==================== 主菜单 ====================" << endl;
        cout << "1. 创建几何图元" << endl;
        cout << "2. 查看所有图元" << endl;
        cout << "3. 搜索图元" << endl;
        cout << "4. 修改图元" << endl;
        cout << "5. 删除图元" << endl;
        cout << "6. 排序图元" << endl;
        cout << "7. 显示统计信息" << endl;
        cout << "8. 导出JSON" << endl;
        cout << "9. 清空容器" << endl;
        cout << "0. 退出程序" << endl;
        cout << "===============================================" << endl;
        cout << "当前图元数量: " << container.size() << endl;
        cout << "请选择操作: ";
    }
    
    int getChoice() {
        int choice;
        while (!(cin >> choice)) {
            cout << "请输入有效数字: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(); // 清除换行符
        return choice;
    }
    
    void createGeometry() {
        cout << "\n=== 创建几何图元 ===" << endl;
        cout << "1. 矩形 (Rectangle)" << endl;
        cout << "2. 圆形 (Circle)" << endl;
        cout << "3. 立方体 (Cuboid)" << endl;
        cout << "4. 圆柱体 (Cylinder)" << endl;
        cout << "请选择图元类型: ";
        
        int type = getChoice();
        
        string name, color;
        cout << "请输入图元名称: ";
        getline(cin, name);
        cout << "请输入图元颜色: ";
        getline(cin, color);
        
        double x, y, z = 0;
        cout << "请输入位置坐标 x: ";
        cin >> x;
        cout << "请输入位置坐标 y: ";
        cin >> y;
        
        Position pos(x, y);
        
        shared_ptr<GeometryPrimitive> geometry;
        
        switch (type) {
            case 1: { // 矩形
                double width, height;
                cout << "请输入矩形宽度: ";
                cin >> width;
                cout << "请输入矩形高度: ";
                cin >> height;
                geometry = make_shared<Rectangle>(name, pos, color, width, height);
                break;
            }
            case 2: { // 圆形
                double radius;
                cout << "请输入圆形半径: ";
                cin >> radius;
                geometry = make_shared<Circle>(name, pos, color, radius);
                break;
            }
            case 3: { // 立方体
                cout << "请输入位置坐标 z: ";
                cin >> z;
                pos = Position(x, y, z);
                double width, height, depth;
                cout << "请输入立方体宽度: ";
                cin >> width;
                cout << "请输入立方体高度: ";
                cin >> height;
                cout << "请输入立方体深度: ";
                cin >> depth;
                geometry = make_shared<Cuboid>(name, pos, color, width, height, depth);
                break;
            }
            case 4: { // 圆柱体
                cout << "请输入位置坐标 z: ";
                cin >> z;
                pos = Position(x, y, z);
                double radius, height;
                cout << "请输入圆柱体半径: ";
                cin >> radius;
                cout << "请输入圆柱体高度: ";
                cin >> height;
                geometry = make_shared<Cylinder>(name, pos, color, radius, height);
                break;
            }
            default:
                cout << "无效的图元类型!" << endl;
                return;
        }
        
        container.addPrimitive(geometry);
        cout << "\n图元创建成功!" << endl;
        cout << "图元信息: ";
        geometry->print();
    }
    
    void viewGeometries() {
        cout << "\n=== 查看所有图元 ===" << endl;
        if (container.size() == 0) {
            cout << "容器中没有图元!" << endl;
            return;
        }
        
        cout << "总共 " << container.size() << " 个图元:" << endl;
        cout << "-----------------------------------" << endl;
        container.printAll();
    }
    
    void searchGeometries() {
        cout << "\n=== 搜索图元 ===" << endl;
        cout << "1. 按ID搜索" << endl;
        cout << "2. 按名称搜索" << endl;
        cout << "3. 按类型搜索" << endl;
        cout << "4. 按颜色搜索" << endl;
        cout << "5. 按面积范围搜索" << endl;
        cout << "6. 按索引访问" << endl;
        cout << "请选择搜索方式: ";
        
        int choice = getChoice();
        
        switch (choice) {
            case 1: { // 按ID搜索
                int id;
                cout << "请输入图元ID: ";
                cin >> id;
                auto found = container.findById(id);
                if (found) {
                    cout << "找到图元: ";
                    found->print();
                    cout << "面积: " << found->area() << endl;
                } else {
                    cout << "未找到ID为 " << id << " 的图元!" << endl;
                }
                break;
            }
            case 2: { // 按名称搜索
                string name;
                cout << "请输入图元名称: ";
                getline(cin, name);
                auto found = container.findByName(name);
                if (!found.empty()) {
                    cout << "找到 " << found.size() << " 个匹配的图元:" << endl;
                    for (const auto& geometry : found) {
                        geometry->print();
                    }
                } else {
                    cout << "未找到名称为 \"" << name << "\" 的图元!" << endl;
                }
                break;
            }
            case 3: { // 按类型搜索
                string type;
                cout << "请输入图元类型 (Rectangle/Circle/Cuboid/Cylinder): ";
                getline(cin, type);
                auto found = container.findByType(type);
                if (!found.empty()) {
                    cout << "找到 " << found.size() << " 个 " << type << " 类型的图元:" << endl;
                    for (const auto& geometry : found) {
                        geometry->print();
                    }
                } else {
                    cout << "未找到类型为 \"" << type << "\" 的图元!" << endl;
                }
                break;
            }
            case 4: { // 按颜色搜索
                string color;
                cout << "请输入图元颜色: ";
                getline(cin, color);
                auto found = container.findByColor(color);
                if (!found.empty()) {
                    cout << "找到 " << found.size() << " 个 " << color << " 颜色的图元:" << endl;
                    for (const auto& geometry : found) {
                        geometry->print();
                    }
                } else {
                    cout << "未找到颜色为 \"" << color << "\" 的图元!" << endl;
                }
                break;
            }
            case 5: { // 按面积范围搜索
                double minArea, maxArea;
                cout << "请输入最小面积: ";
                cin >> minArea;
                cout << "请输入最大面积: ";
                cin >> maxArea;
                auto found = container.findByAreaRange(minArea, maxArea);
                if (!found.empty()) {
                    cout << "找到 " << found.size() << " 个面积在 [" << minArea << ", " << maxArea << "] 范围内的图元:" << endl;
                    for (const auto& geometry : found) {
                        geometry->print();
                        cout << "面积: " << geometry->area() << endl;
                    }
                } else {
                    cout << "未找到面积在指定范围内的图元!" << endl;
                }
                break;
            }
            case 6: { // 按索引访问
                int index;
                cout << "请输入索引 (0-" << (container.size()-1) << "): ";
                cin >> index;
                auto found = container.at(index);
                if (found) {
                    cout << "索引 " << index << " 处的图元: ";
                    found->print();
                } else {
                    cout << "无效的索引!" << endl;
                }
                break;
            }
            default:
                cout << "无效的搜索方式!" << endl;
        }
    }
    
    void modifyGeometry() {
        cout << "\n=== 修改图元 ===" << endl;
        if (container.size() == 0) {
            cout << "容器中没有图元!" << endl;
            return;
        }
        
        int id;
        cout << "请输入要修改的图元ID: ";
        cin >> id;
        
        auto geometry = container.findById(id);
        if (!geometry) {
            cout << "未找到ID为 " << id << " 的图元!" << endl;
            return;
        }
        
        cout << "当前图元信息: ";
        geometry->print();
        
        cout << "\n请选择要修改的属性:" << endl;
        cout << "1. 名称" << endl;
        cout << "2. 颜色" << endl;
        cout << "3. 位置" << endl;
        cout << "请选择: ";
        
        int choice = getChoice();
        
        switch (choice) {
            case 1: { // 修改名称
                string newName;
                cout << "请输入新名称: ";
                getline(cin, newName);
                geometry->setName(newName);
                cout << "名称修改成功!" << endl;
                break;
            }
            case 2: { // 修改颜色
                string newColor;
                cout << "请输入新颜色: ";
                getline(cin, newColor);
                geometry->setColor(newColor);
                cout << "颜色修改成功!" << endl;
                break;
            }
            case 3: { // 修改位置
                double x, y, z = 0;
                cout << "请输入新的x坐标: ";
                cin >> x;
                cout << "请输入新的y坐标: ";
                cin >> y;
                
                // 检查是否为3D图元
                if (geometry->getType() == "Cuboid" || geometry->getType() == "Cylinder") {
                    cout << "请输入新的z坐标: ";
                    cin >> z;
                    geometry->setPosition(Position(x, y, z));
                } else {
                    geometry->setPosition(Position(x, y));
                }
                cout << "位置修改成功!" << endl;
                break;
            }
            default:
                cout << "无效选择!" << endl;
                return;
        }
        
        cout << "修改后的图元信息: ";
        geometry->print();
    }
    
    void deleteGeometry() {
        cout << "\n=== 删除图元 ===" << endl;
        if (container.size() == 0) {
            cout << "容器中没有图元!" << endl;
            return;
        }
        
        int id;
        cout << "请输入要删除的图元ID: ";
        cin >> id;
        
        auto geometry = container.findById(id);
        if (!geometry) {
            cout << "未找到ID为 " << id << " 的图元!" << endl;
            return;
        }
        
        cout << "要删除的图元信息: ";
        geometry->print();
        
        char confirm;
        cout << "确认删除? (y/n): ";
        cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            bool success = container.deletePrimitive(id);
            if (success) {
                cout << "图元删除成功!" << endl;
            } else {
                cout << "删除失败!" << endl;
            }
        } else {
            cout << "取消删除操作。" << endl;
        }
    }
    
    void sortGeometries() {
        cout << "\n=== 排序图元 ===" << endl;
        if (container.size() == 0) {
            cout << "容器中没有图元!" << endl;
            return;
        }
        
        cout << "请选择排序方式:" << endl;
        cout << "1. 按ID排序" << endl;
        cout << "2. 按面积排序" << endl;
        cout << "3. 按名称排序" << endl;
        cout << "请选择: ";
        
        int choice = getChoice();
        
        switch (choice) {
            case 1:
                container.sortById();
                cout << "按ID排序完成!" << endl;
                break;
            case 2:
                container.sortByArea();
                cout << "按面积排序完成!" << endl;
                break;
            case 3:
                container.sortByName();
                cout << "按名称排序完成!" << endl;
                break;
            default:
                cout << "无效选择!" << endl;
                return;
        }
        
        cout << "\n排序后的图元列表:" << endl;
        container.printAll();
    }
    
    void showStatistics() {
        cout << "\n=== 统计信息 ===" << endl;
        container.printStatistics();
    }
    
    void exportToJson() {
        cout << "\n=== 导出JSON ===" << endl;
        if (container.size() == 0) {
            cout << "容器中没有图元!" << endl;
            return;
        }
        
        cout << "请选择导出方式:" << endl;
        cout << "1. 导出单个图元" << endl;
        cout << "2. 导出所有图元" << endl;
        cout << "请选择: ";
        
        int choice = getChoice();
        
        switch (choice) {
            case 1: { // 导出单个图元
                int id;
                cout << "请输入要导出的图元ID: ";
                cin >> id;
                auto geometry = container.findById(id);
                if (geometry) {
                    cout << "图元JSON格式:" << endl;
                    cout << geometry->toJson() << endl;
                } else {
                    cout << "未找到指定图元!" << endl;
                }
                break;
            }
            case 2: { // 导出所有图元
                cout << "所有图元的JSON格式:" << endl;
                cout << "[" << endl;
                auto allGeometries = container.getAllPrimitives();
                for (size_t i = 0; i < allGeometries.size(); ++i) {
                    cout << "  " << allGeometries[i]->toJson();
                    if (i < allGeometries.size() - 1) {
                        cout << ",";
                    }
                    cout << endl;
                }
                cout << "]" << endl;
                break;
            }
            default:
                cout << "无效选择!" << endl;
        }
    }
    
    void clearContainer() {
        cout << "\n=== 清空容器 ===" << endl;
        if (container.size() == 0) {
            cout << "容器已经是空的!" << endl;
            return;
        }
        
        char confirm;
        cout << "确认清空容器中的所有 " << container.size() << " 个图元? (y/n): ";
        cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            container.clear();
            cout << "容器已清空!" << endl;
        } else {
            cout << "取消清空操作。" << endl;
        }
    }
};

int main() {
    InteractiveConsole console;
    console.run();
    return 0;
}