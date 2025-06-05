# 几何图元管理系统 - 后端

## 项目概述

几何图元管理系统是一个基于C++开发的后端应用程序，用于管理和可视化二维和三维几何图元。该系统采用三层架构设计，风格贴近Java企业级应用，具有低耦合度和高可扩展性的特点。系统支持通过Web界面和控制台两种方式进行交互，可以对几何图元进行创建、查询、修改、删除等操作。

## 系统架构

系统采用经典的三层架构设计：

1. **控制器层**
   - **GeometryController**：处理HTTP请求，调用服务层接口
   - 使用第三方库httplib实现HTTP服务器功能

2. **服务层**
   - **GeometryService**：实现业务逻辑，连接控制器和数据访问层
   - 提供图元的创建、查询、修改、删除等功能

3. **数据访问层（DAO层）**
   - **GeometryDAO**：负责数据持久化
   - 支持MySQL数据库存储和JSON文件存储两种方式

4. **对象信息层（图元对象）**
   - **二维对象**：矩形、圆形等
   - **三维对象**：立方体、圆柱体等

5. **持久层**
   - **ObjectContainer**：基于双向链表实现的对象容器
   - **MySQL数据库**：关系型数据库存储
   - **文件存储**：JSON格式文件存储

## 功能特点

### 几何图元管理

- 支持多种二维图元（矩形、圆形）和三维图元（立方体、圆柱体）
- 提供图元的创建、查询、修改、删除等基本操作
- 支持按名称、类型、颜色、面积范围等条件查询图元
- 支持图元按ID、名称、面积等属性排序

### 数据持久化

- 支持MySQL数据库存储
- 支持JSON文件存储
- 可在两种存储方式之间灵活切换

### 用户界面

- Web界面：通过HTTP API提供服务，支持前端调用
- 控制台界面：命令行交互式操作

## 技术栈

- C++17
- MySQL（数据库）
- httplib（HTTP服务器库）
- jsoncpp（JSON解析库）

## 安装与运行

### 依赖项

- g++ 编译器（支持C++17）
- MySQL开发库
- jsoncpp库

### 编译

```bash
make
```

### 运行服务器

```bash
./geometry_server
```

可选参数：
- `--port <端口号>`：指定HTTP服务器端口（默认：58010）
- `--host <主机地址>`：指定HTTP服务器主机（默认：0.0.0.0）
- `--no-database`：使用文件存储模式（不连接数据库）
- `--db-host <数据库主机>`：指定MySQL主机
- `--db-user <用户名>`：指定MySQL用户名
- `--db-password <密码>`：指定MySQL密码
- `--db-name <数据库名>`：指定MySQL数据库名

### 运行控制台程序

```bash
cd src/console
make
./console_demo
```

## 项目结构

```
./
├── Makefile                # 主Makefile
├── geometry_data.json      # 数据文件（文件存储模式）
├── geometry_server         # 编译后的服务器可执行文件
└── src/                    # 源代码目录
    ├── console/            # 控制台程序
    │   ├── Makefile
    │   ├── console_demo.cpp
    │   └── geometry_data.json
    ├── controller/         # 控制器层
    │   ├── GeometryController.cpp
    │   └── GeometryController.h
    ├── dao/                # 数据访问层
    │   ├── GeometryDAO.cpp
    │   └── GeometryDAO.h
    ├── main.cpp            # 主程序入口
    ├── models/             # 模型层
    │   ├── GeometryContainer.cpp
    │   ├── GeometryContainer.h
    │   ├── GeometryPrimitive.cpp
    │   ├── GeometryPrimitive.h
    │   ├── Shape2D.cpp
    │   ├── Shape2D.h
    │   ├── Shape3D.cpp
    │   └── Shape3D.h
    └── service/            # 服务层
        ├── GeometryService.cpp
        └── GeometryService.h
```

## 设计特点

1. **低耦合度**：采用三层架构设计，各层之间通过接口交互，降低了系统各部分之间的依赖
2. **可扩展性**：可以轻松添加新的几何图元类型和功能
3. **多种存储方式**：支持数据库存储和文件存储，可根据需求灵活切换
4. **双重交互方式**：提供Web界面和控制台两种交互方式，满足不同使用场景
5. **面向对象设计**：采用继承和多态等面向对象设计原则，代码结构清晰

## 注意事项

- 默认HTTP服务器端口为58010，可通过命令行参数修改
- 默认使用MySQL数据库存储，可通过`--no-db`参数切换到文件存储模式
- 前端默认连接到`http://localhost:58010`，如需修改，请更新前端代码中的API基础URL