# 几何图元管理系统 (Geometry Primitives Management System)

基于C++的几何图元管理系统，采用三层架构设计，支持MySQL数据持久化存储。

## 系统架构

### 三层架构设计

```
┌─────────────────────────────────────────────────────────────┐
│                    表示层 (Controller Layer)                 │
│  - GeometryController: HTTP请求处理                         │
│  - JSON序列化/反序列化                                       │
│  - CORS支持                                                 │
└─────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────┐
│                    业务层 (Service Layer)                    │
│  - GeometryService: 业务逻辑处理                            │
│  - 数据验证                                                 │
│  - 缓存管理                                                 │
│  - 事务处理                                                 │
└─────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────┐
│                   数据访问层 (DAO Layer)                     │
│  - GeometryDAO: 数据库操作                                  │
│  - MySQL连接管理                                            │
│  - SQL查询执行                                              │
│  - 数据转换                                                 │
└─────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────┐
│                      数据层 (Data Layer)                     │
│  - MySQL数据库                                              │
│  - 几何图元数据表                                            │
└─────────────────────────────────────────────────────────────┘
```

## 功能特性

### 支持的几何图元

#### 2D图元
- **Rectangle (矩形)**: 包含宽度、高度属性
- **Circle (圆形)**: 包含半径属性

#### 3D图元
- **Cuboid (长方体)**: 包含长、宽、高属性
- **Cylinder (圆柱体)**: 包含半径、高度属性

### 图元属性
- **ID**: 唯一标识符（自动生成）
- **名称**: 图元名称
- **位置**: 3D坐标 (x, y, z)
- **颜色**: 图元颜色
- **类型**: 图元类型
- **几何属性**: 面积、周长、体积、表面积等

### 容器管理功能
- 添加图元
- 删除图元
- 修改图元
- 按ID查询
- 按名称查询
- 按类型查询
- 按颜色查询
- 按面积范围查询
- 自定义条件查询
- 排序功能（按ID、面积、名称）
- 统计信息

### 数据持久化
- MySQL数据库存储
- 自动表结构创建
- 事务支持
- 连接池管理
- 数据同步

## 项目结构

```
cpp_backend/
├── src/
│   ├── models/                    # 模型层
│   │   ├── GeometryPrimitive.h   # 基础图元类
│   │   ├── Shape2D.h             # 2D图元实现
│   │   ├── Shape3D.h             # 3D图元实现
│   │   └── GeometryContainer.h   # 容器类
│   ├── dao/                      # 数据访问层
│   │   └── GeometryDAO.h         # 数据库操作
│   ├── service/                  # 业务逻辑层
│   │   └── GeometryService.h     # 业务服务
│   ├── controller/               # 控制器层
│   │   └── GeometryController.h  # HTTP控制器
│   ├── test/                     # 测试文件
│   │   └── test_geometry.cpp     # 单元测试
│   └── main.cpp                  # 主程序入口
├── Makefile                      # 构建配置
└── README.md                     # 项目文档
```

## 环境要求

### 系统要求
- Linux (Ubuntu 18.04+)
- GCC 7.0+ (支持C++17)
- MySQL 5.7+

### 依赖库
- **jsoncpp**: JSON处理
- **mysql-client**: MySQL客户端库
- **httplib**: HTTP服务器（头文件库）
- **pthread**: 多线程支持

## 安装和配置

### 1. 安装依赖

```bash
# 安装系统依赖
make install-deps

# 或手动安装
sudo apt-get update
sudo apt-get install -y libmysqlclient-dev libmysql++-dev libjsoncpp-dev build-essential
```

### 2. 配置MySQL数据库

```bash
# 查看数据库配置说明
make setup-db

# 登录MySQL并执行以下命令
mysql -u root -p
```

```sql
CREATE DATABASE geometry_db;
CREATE USER 'geometry_user'@'localhost' IDENTIFIED BY 'geometry_pass';
GRANT ALL PRIVILEGES ON geometry_db.* TO 'geometry_user'@'localhost';
FLUSH PRIVILEGES;
```

### 3. 编译项目

```bash
# 编译所有目标
make all

# 或分别编译
make server    # 编译服务器
make test      # 编译测试程序
```

## 使用方法

### 启动服务器

```bash
# 使用默认配置启动
make run-server

# 或直接运行
./geometry_server --host=localhost --port=8080 \
                  --db-host=localhost --db-port=3306 \
                  --db-name=geometry_db --db-user=geometry_user \
                  --db-pass=geometry_pass
```

### 运行测试

```bash
# 运行单元测试
make run-tests

# 运行内存检查
make memcheck
```

### 开发模式

```bash
# 启动开发服务器（自动重启）
make dev
```

## API接口

### 基础URL
```
http://localhost:8080/api
```

### 端点列表

#### 健康检查
- **GET** `/health` - 服务健康状态

#### 图元管理
- **GET** `/primitives` - 获取所有图元
- **GET** `/primitives/{id}` - 根据ID获取图元
- **POST** `/primitives` - 创建新图元
- **PUT** `/primitives/{id}` - 更新图元
- **DELETE** `/primitives/{id}` - 删除图元

#### 查询功能
- **GET** `/primitives/type/{type}` - 按类型查询
- **GET** `/primitives/color/{color}` - 按颜色查询
- **GET** `/primitives/area?min={min}&max={max}` - 按面积范围查询

#### 统计信息
- **GET** `/statistics` - 获取统计信息

### 请求示例

#### 创建矩形
```bash
curl -X POST http://localhost:8080/api/primitives \
  -H "Content-Type: application/json" \
  -d '{
    "type": "Rectangle",
    "name": "测试矩形",
    "position": {"x": 10, "y": 20, "z": 0},
    "color": "红色",
    "width": 100,
    "height": 50
  }'
```

#### 创建圆形
```bash
curl -X POST http://localhost:8080/api/primitives \
  -H "Content-Type: application/json" \
  -d '{
    "type": "Circle",
    "name": "测试圆形",
    "position": {"x": 30, "y": 40, "z": 0},
    "color": "蓝色",
    "radius": 25
  }'
```

#### 创建长方体
```bash
curl -X POST http://localhost:8080/api/primitives \
  -H "Content-Type: application/json" \
  -d '{
    "type": "Cuboid",
    "name": "测试长方体",
    "position": {"x": 50, "y": 60, "z": 10},
    "color": "绿色",
    "length": 10,
    "width": 8,
    "height": 6
  }'
```

#### 创建圆柱体
```bash
curl -X POST http://localhost:8080/api/primitives \
  -H "Content-Type: application/json" \
  -d '{
    "type": "Cylinder",
    "name": "测试圆柱体",
    "position": {"x": 70, "y": 80, "z": 20},
    "color": "黄色",
    "radius": 5,
    "height": 12
  }'
```

## 开发工具

### 代码格式化
```bash
make format
```

### 静态分析
```bash
make analyze
```

### 内存检查
```bash
make memcheck
```

### 清理构建
```bash
make clean
make rebuild
```

## 性能特性

- **内存管理**: 使用智能指针自动管理内存
- **线程安全**: 支持多线程并发访问
- **连接池**: MySQL连接池提高数据库性能
- **缓存机制**: 内存缓存减少数据库访问
- **批量操作**: 支持批量数据操作

## 扩展性

### 添加新图元类型
1. 在相应的Shape2D.h或Shape3D.h中添加新类
2. 实现必要的虚函数
3. 更新GeometryService中的工厂方法
4. 添加相应的测试用例

### 添加新查询条件
1. 在GeometryContainer中添加新的查询方法
2. 在GeometryDAO中添加相应的数据库查询
3. 在GeometryService中添加业务逻辑
4. 在GeometryController中添加HTTP端点

## 故障排除

### 常见问题

1. **编译错误**: 确保安装了所有依赖库
2. **数据库连接失败**: 检查MySQL服务状态和连接参数
3. **端口占用**: 更改服务器端口或停止占用进程
4. **权限问题**: 确保数据库用户有足够权限

### 日志查看
服务器运行时会输出详细的日志信息，包括：
- 请求处理日志
- 数据库操作日志
- 错误信息
- 性能统计

## 许可证

本项目采用MIT许可证。详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进项目。

## 联系方式

如有问题或建议，请通过以下方式联系：
- 提交GitHub Issue
- 发送邮件至项目维护者

---

**注意**: 这是一个教学示例项目，用于演示C++三层架构设计和MySQL数据持久化。在生产环境中使用前，请进行充分的测试和安全评估。