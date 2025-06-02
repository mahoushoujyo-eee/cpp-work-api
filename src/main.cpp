#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <signal.h>
#include "httplib.h"
#include "models/GeometryPrimitive.h"
#include "models/Shape2D.h"
#include "models/Shape3D.h"
#include "models/GeometryContainer.h"
#include "dao/GeometryDAO.h"
#include "service/GeometryService.h"
#include "controller/GeometryController.h"

// 配置信息结构体
struct AppConfig {
    std::string host;
    int port;
    DatabaseConfig dbConfig;
    bool useDatabase;
    
    AppConfig()
        : host("0.0.0.0"),
          port(58010),
          useDatabase(true) {
        dbConfig.host = "122.51.67.113";
        dbConfig.username = "cpp_work";
        dbConfig.password = "yuzaoqian123";
        dbConfig.database = "cpp_work";
        dbConfig.port = 3306;
    }
};

// 解析命令行参数
AppConfig parseCommandLineArgs(int argc, char** argv) {
    AppConfig config;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--port" && i + 1 < argc) {
            config.port = std::stoi(argv[++i]);
        } else if (arg == "--host" && i + 1 < argc) {
            config.host = argv[++i];
        } else if (arg == "--db-host" && i + 1 < argc) {
            config.dbConfig.host = argv[++i];
        } else if (arg == "--db-user" && i + 1 < argc) {
            config.dbConfig.username = argv[++i];
        } else if (arg == "--db-password" && i + 1 < argc) {
            config.dbConfig.password = argv[++i];
        } else if (arg == "--db-name" && i + 1 < argc) {
            config.dbConfig.database = argv[++i];
        } else if (arg == "--db-port" && i + 1 < argc) {
            config.dbConfig.port = std::stoi(argv[++i]);
        } else if (arg == "--no-database") {
            config.useDatabase = false;
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --port PORT          Server port (default: 58010)" << std::endl;
            std::cout << "  --host HOST          Server host (default: 0.0.0.0)" << std::endl;
            std::cout << "  --db-host HOST       Database host (default: localhost)" << std::endl;
            std::cout << "  --db-user USER       Database user (default: root)" << std::endl;
            std::cout << "  --db-password PASS   Database password (default: password)" << std::endl;
            std::cout << "  --db-name NAME       Database name (default: geometry_db)" << std::endl;
            std::cout << "  --db-port PORT       Database port (default: 3306)" << std::endl;
            std::cout << "  --no-database        Use container mode only (no database)" << std::endl;
            std::cout << "  --help               Show this help message" << std::endl;
            exit(0);
        }
    }
    
    return config;
}

// 前向声明
void createDefaultTestData(GeometryService& service);

// 全局变量用于信号处理
GeometryService* g_service = nullptr;
std::string g_dataFile = "geometry_data.json";
bool g_useDatabase = false;

// 从文件加载数据
void loadDataFromFile(GeometryService& service, const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "数据文件 " << filename << " 不存在，将创建默认测试数据" << std::endl;
            createDefaultTestData(service);
            return;
        }
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errors;
        
        if (!Json::parseFromStream(builder, file, &root, &errors)) {
            std::cerr << "解析JSON文件失败: " << errors << std::endl;
            createDefaultTestData(service);
            return;
        }
        
        if (root.isArray()) {
            for (const auto& primitive : root) {
                try {
                    service.createPrimitive(primitive);
                } catch (const std::exception& e) {
                    std::cerr << "加载图元失败: " << e.what() << std::endl;
                }
            }
            std::cout << "从文件 " << filename << " 成功加载了 " << root.size() << " 个图元" << std::endl;
        } else {
            std::cerr << "JSON文件格式错误，应该是数组格式" << std::endl;
            createDefaultTestData(service);
        }
        
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "加载数据文件失败: " << e.what() << std::endl;
        createDefaultTestData(service);
    }
}

// 保存数据到文件
void saveDataToFile(GeometryService& service, const std::string& filename) {
    try {
        // 获取所有图元数据
        Json::Value allPrimitives = service.getAllPrimitivesAsJson();
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法创建数据文件: " << filename << std::endl;
            return;
        }
        
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "  "; // 美化输出
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        writer->write(allPrimitives, &file);
        
        file.close();
        std::cout << "数据已保存到文件: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "保存数据文件失败: " << e.what() << std::endl;
    }
}

// 兜底处理， 如果读取json出现失败用这个内容模拟
void createDefaultTestData(GeometryService& service) {
    try {
        // 创建2D图元
        Json::Value circle;
        circle["type"] = "Circle";
        circle["name"] = "测试圆形";
        circle["x"] = 100;
        circle["y"] = 100;
        circle["z"] = 0;
        circle["color"] = "red";
        circle["radius"] = 50;
        service.createPrimitive(circle);
        
        Json::Value rectangle;
        rectangle["type"] = "Rectangle";
        rectangle["name"] = "测试矩形";
        rectangle["x"] = 200;
        rectangle["y"] = 200;
        rectangle["z"] = 0;
        rectangle["color"] = "blue";
        rectangle["width"] = 100;
        rectangle["height"] = 80;
        service.createPrimitive(rectangle);
        
        // 创建3D图元
        Json::Value cuboid;
        cuboid["type"] = "Cuboid";
        cuboid["name"] = "测试长方体";
        cuboid["x"] = 300;
        cuboid["y"] = 300;
        cuboid["z"] = 0;
        cuboid["color"] = "green";
        cuboid["length"] = 100;
        cuboid["width"] = 80;
        cuboid["height"] = 60;
        service.createPrimitive(cuboid);
        
        Json::Value cylinder;
        cylinder["type"] = "Cylinder";
        cylinder["name"] = "测试圆柱体";
        cylinder["x"] = 400;
        cylinder["y"] = 400;
        cylinder["z"] = 0;
        cylinder["color"] = "yellow";
        cylinder["radius"] = 40;
        cylinder["height"] = 120;
        service.createPrimitive(cylinder);
        
        std::cout << "默认测试数据创建成功！" << std::endl;
        
        // 保存默认数据到文件
        saveDataToFile(service, g_dataFile);
    } catch (const std::exception& e) {
        std::cerr << "创建默认测试数据失败: " << e.what() << std::endl;
    }
}

// 信号处理函数
void signalHandler(int signal) {
    std::cout << "\n接收到信号 " << signal << "，正在保存数据并退出..." << std::endl;
    if (g_service && !g_useDatabase) {
        // 只在非数据库模式下保存到JSON文件
        saveDataToFile(*g_service, g_dataFile);
    } else if (g_useDatabase) {
        std::cout << "数据库模式：数据已自动保存到数据库" << std::endl;
    }
    exit(0);
}

int main(int argc, char** argv) {
    try {
        // 解析命令行参数
        AppConfig config = parseCommandLineArgs(argc, argv);
        
        // 根据配置创建控制器和服务
        std::shared_ptr<GeometryService> geometryService;
        if (config.useDatabase) {
            geometryService = std::make_shared<GeometryService>(config.dbConfig);
        } else {
            geometryService = std::make_shared<GeometryService>();
        }
        
        if (!config.useDatabase) {
            // 从文件加载数据
            loadDataFromFile(*geometryService, g_dataFile);
        }
        
        GeometryController controller;
        controller.setService(geometryService);
        
        // 设置全局变量用于信号处理
        g_service = geometryService.get();
        g_useDatabase = config.useDatabase;
        
        // 注册信号处理函数
        signal(SIGINT, signalHandler);  // Ctrl+C
        signal(SIGTERM, signalHandler); // 终止信号
        
        // 输出服务信息
        std::cout << "几何图元管理器HTTP服务启动中..." << std::endl;
        std::cout << "服务地址: http://" << config.host << ":" << config.port << std::endl;
        std::cout << "API端点:" << std::endl;
        std::cout << "  GET    /health           - 健康检查" << std::endl;
        std::cout << "  GET    /primitives       - 获取所有图元" << std::endl;
        std::cout << "  GET    /primitives/{id}  - 获取单个图元" << std::endl;
        std::cout << "  POST   /primitives       - 创建新图元" << std::endl;
        std::cout << "  PUT    /primitives/{id}  - 更新图元" << std::endl;
        std::cout << "  DELETE /primitives/{id}  - 删除图元" << std::endl;
        std::cout << "  DELETE /primitives       - 清空所有图元" << std::endl;
        std::cout << "  GET    /statistics       - 获取统计信息" << std::endl;
        std::cout << "  GET    /primitives/type/:type - 按类型查询图元" << std::endl;
        
        // 启动服务器
        httplib::Server server;
        controller.registerRoutes(server);
        
        std::cout << "服务器启动成功，按 Ctrl+C 停止服务" << std::endl;
        server.listen(config.host.c_str(), config.port);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        return -1;
    }
}