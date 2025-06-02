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
        if (!json.isMember("type")) {
            return nullptr;
        }
        
        string type = json["type"].asString();
        shared_ptr<GeometryPrimitive> geometry = nullptr;
        
        // 创建对应类型的几何图元实例
        if (type == "Rectangle") {
            geometry = make_shared<Rectangle>("", Position(0, 0, 0), "", 0, 0);
        }
        else if (type == "Circle") {
            geometry = make_shared<Circle>("", Position(0, 0, 0), "", 0);
        }
        else if (type == "Cuboid") {
            geometry = make_shared<Cuboid>("", Position(0, 0, 0), "", 0, 0, 0);
        }
        else if (type == "Cylinder") {
            geometry = make_shared<Cylinder>("", Position(0, 0, 0), "", 0, 0);
        }
        
        if (geometry) {
            // 转换扁平化的位置信息为嵌套格式，以兼容fromJson方法
            Json::Value jsonCopy = json;
            if (json.isMember("x") && json.isMember("y") && json.isMember("z")) {
                Json::Value position;
                position["x"] = json["x"];
                position["y"] = json["y"];
                position["z"] = json["z"];
                jsonCopy["position"] = position;
            }
            
            // 使用已有的fromJson方法进行反序列化
            geometry->fromJson(jsonCopy);
            if (json.isMember("id")) {
                geometry->setId(json["id"].asInt());
            }
        }
        
        return geometry;
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
            // 使用已有的toJson方法进行序列化
            Json::Value geometryJson = geometry->toJson();
            
            // 添加ID和类型信息（确保包含）
            geometryJson["id"] = geometry->getId();
            geometryJson["type"] = geometry->getType();
            
            // 确保位置信息以扁平化格式存储（兼容旧格式）
            if (geometryJson.isMember("position")) {
                Json::Value pos = geometryJson["position"];
                geometryJson["x"] = pos["x"];
                geometryJson["y"] = pos["y"];
                geometryJson["z"] = pos["z"];
                geometryJson.removeMember("position");
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
    // 辅助函数：检查容器是否为空
    bool checkContainerEmpty(const string& operation = "操作") {
        if (container.size() == 0) {
            cout << "容器中没有图元，无法进行" << operation << "!" << endl;
            return true;
        }
        return false;
    }
    
    // 辅助函数：显示搜索结果
    template<typename T>
    void displaySearchResults(const vector<T>& results, const string& searchType) {
        if (results.empty()) {
            cout << "未找到符合条件的图元!" << endl;
        } else {
            cout << "找到 " << results.size() << " 个符合条件的图元:" << endl;
            for (const auto& item : results) {
                item->print();
            }
        }
        (void)searchType; // 避免未使用参数警告
    }
    
    // 辅助函数：查找并验证几何体
    shared_ptr<GeometryPrimitive> findAndValidateGeometry(int id, const string& operation = "操作") {
        auto geometry = container.findById(id);
        if (!geometry) {
            cout << "未找到ID为 " << id << " 的图元，无法进行" << operation << "!" << endl;
        }
        return geometry;
    }
    
    // 辅助函数：获取基本几何体信息
    struct BasicGeometryInfo {
        string name;
        string color;
        Position position;
    };
    
    BasicGeometryInfo getBasicGeometryInfo() {
        BasicGeometryInfo info;
        cout << "请输入图元名称: ";
        cin >> info.name;
        cout << "请输入图元颜色: ";
        cin >> info.color;
        info.position = getPositionInput();
        return info;
    }
    
    // 辅助函数：获取位置输入
    Position getPositionInput() {
        double x, y, z;
        cout << "请输入位置坐标:" << endl;
        cout << "X坐标: ";
        cin >> x;
        cout << "Y坐标: ";
        cin >> y;
        cout << "Z坐标: ";
        cin >> z;
        return Position(x, y, z);
    }
    
    // 辅助函数：确认操作
    bool confirmAction(const string& action) {
        char confirm;
        cout << "确认" << action << "? (y/n): ";
        cin >> confirm;
        return (confirm == 'y' || confirm == 'Y');
    }
    
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
        cout << "请选择操作: ";
    }
    
    int getChoice() {
        int choice;
        while (!(cin >> choice)) {
            cout << "输入无效，请输入数字: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        return choice;
    }
    
    void createGeometry() {
        cout << "\n=== 创建几何图元 ===" << endl;
        cout << "请选择要创建的图元类型:" << endl;
        cout << "1. 矩形 (Rectangle)" << endl;
        cout << "2. 圆形 (Circle)" << endl;
        cout << "3. 长方体 (Cuboid)" << endl;
        cout << "4. 圆柱体 (Cylinder)" << endl;
        cout << "请选择: ";
        
        int choice = getChoice();
        
        auto info = getBasicGeometryInfo();
        shared_ptr<GeometryPrimitive> geometry = nullptr;
        
        switch (choice) {
            case 1: { // 矩形
                double width, height;
                cout << "请输入矩形宽度: ";
                cin >> width;
                cout << "请输入矩形高度: ";
                cin >> height;
                geometry = make_shared<Rectangle>(info.name, info.position, info.color, width, height);
                break;
            }
            case 2: { // 圆形
                double radius;
                cout << "请输入圆形半径: ";
                cin >> radius;
                geometry = make_shared<Circle>(info.name, info.position, info.color, radius);
                break;
            }
            case 3: { // 长方体
                double width, height, depth;
                cout << "请输入长方体宽度: ";
                cin >> width;
                cout << "请输入长方体高度: ";
                cin >> height;
                cout << "请输入长方体深度: ";
                cin >> depth;
                geometry = make_shared<Cuboid>(info.name, info.position, info.color, width, height, depth);
                break;
            }
            case 4: { // 圆柱体
                double radius, height;
                cout << "请输入圆柱体半径: ";
                cin >> radius;
                cout << "请输入圆柱体高度: ";
                cin >> height;
                geometry = make_shared<Cylinder>(info.name, info.position, info.color, radius, height);
                break;
            }
            default:
                cout << "无效选择!" << endl;
                return;
        }
        
        if (geometry) {
            container.addPrimitive(geometry);
            cout << "图元创建成功! ID: " << geometry->getId() << endl;
            geometry->print();
        }
    }
    
    void viewGeometries() {
        cout << "\n=== 查看所有图元 ===" << endl;
        if (checkContainerEmpty("查看")) return;
        
        cout << "当前容器中共有 " << container.size() << " 个图元:" << endl;
        container.printAll();
    }
    
    void searchGeometries() {
        cout << "\n=== 搜索图元 ===" << endl;
        if (checkContainerEmpty("搜索")) return;
        
        cout << "请选择搜索方式:" << endl;
        cout << "1. 按ID搜索" << endl;
        cout << "2. 按名称搜索" << endl;
        cout << "3. 按类型搜索" << endl;
        cout << "4. 按颜色搜索" << endl;
        cout << "5. 按面积范围搜索" << endl;
        cout << "6. 按索引搜索" << endl;
        cout << "请选择: ";
        
        int choice = getChoice();
        
        switch (choice) {
            case 1: { // 按ID搜索
                int id;
                cout << "请输入要搜索的ID: ";
                cin >> id;
                auto geometry = container.findById(id);
                if (geometry) {
                    cout << "找到图元:" << endl;
                    geometry->print();
                } else {
                    cout << "未找到ID为 " << id << " 的图元!" << endl;
                }
                break;
            }
            case 2: { // 按名称搜索
                string name;
                cout << "请输入要搜索的名称: ";
                cin >> name;
                auto results = container.findByName(name);
                displaySearchResults(results, "名称");
                break;
            }
            case 3: { // 按类型搜索
                string type;
                cout << "请输入要搜索的类型 (Rectangle/Circle/Cuboid/Cylinder): ";
                cin >> type;
                auto results = container.findByType(type);
                displaySearchResults(results, "类型");
                break;
            }
            case 4: { // 按颜色搜索
                string color;
                cout << "请输入要搜索的颜色: ";
                cin >> color;
                auto results = container.findByColor(color);
                displaySearchResults(results, "颜色");
                break;
            }
            case 5: { // 按面积范围搜索
                double minArea, maxArea;
                cout << "请输入最小面积: ";
                cin >> minArea;
                cout << "请输入最大面积: ";
                cin >> maxArea;
                auto results = container.findByAreaRange(minArea, maxArea);
                displaySearchResults(results, "面积范围");
                break;
            }
             case 6: { // 按索引搜索
                 int index;
                 cout << "请输入要搜索的索引 (0-" << container.size()-1 << "): ";
                 cin >> index;
                 auto geometry = container.at(index);
                 if (geometry) {
                     cout << "找到图元:" << endl;
                     geometry->print();
                 } else {
                     cout << "索引 " << index << " 超出范围!" << endl;
                 }
                 break;
             }
            default:
                cout << "无效选择!" << endl;
        }
    }
    
    void modifyGeometry() {
        cout << "\n=== 修改图元 ===" << endl;
        if (checkContainerEmpty("修改")) return;
        
        int id;
        cout << "请输入要修改的图元ID: ";
        cin >> id;
        
        auto geometry = findAndValidateGeometry(id, "修改");
        if (!geometry) return;
        
        cout << "当前图元信息: ";
        geometry->print();
        
        cout << "\n请选择要修改的属性:" << endl;
        cout << "1. 修改名称" << endl;
        cout << "2. 修改颜色" << endl;
        cout << "3. 修改位置" << endl;
        cout << "请选择: ";
        
        int choice = getChoice();
        
        switch (choice) {
            case 1: { // 修改名称
                string newName;
                cout << "请输入新名称: ";
                cin >> newName;
                geometry->setName(newName);
                cout << "名称修改成功!" << endl;
                break;
            }
            case 2: { // 修改颜色
                string newColor;
                cout << "请输入新颜色: ";
                cin >> newColor;
                geometry->setColor(newColor);
                cout << "颜色修改成功!" << endl;
                break;
            }
            case 3: { // 修改位置
                Position newPosition = getPositionInput();
                geometry->setPosition(newPosition);
                cout << "位置修改成功!" << endl;
                break;
            }
            default:
                cout << "无效选择!" << endl;
                return;
        }
        
        cout << "\n修改后的图元信息: ";
        geometry->print();
    }
    
    void deleteGeometry() {
        cout << "\n=== 删除图元 ===" << endl;
        if (checkContainerEmpty("删除")) return;
        
        int id;
        cout << "请输入要删除的图元ID: ";
        cin >> id;
        
        auto geometry = findAndValidateGeometry(id, "删除");
        if (!geometry) return;
        
        cout << "要删除的图元信息: ";
        geometry->print();
        
        if (confirmAction("删除")) {
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
        if (checkContainerEmpty("排序")) return;
        
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
        if (checkContainerEmpty("导出")) return;
        
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
                auto geometry = findAndValidateGeometry(id, "导出");
                if (geometry) {
                    cout << "图元JSON格式:" << endl;
                    cout << geometry->toJson() << endl;
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
        if (checkContainerEmpty("清空")) return;
        
        if (confirmAction("清空容器中的所有 " + to_string(container.size()) + " 个图元")) {
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