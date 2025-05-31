# 几何图元管理器 - 运行模式说明

## 概述

几何图元管理器现在支持两种运行模式：

1. **数据库模式**（默认）：数据持久化到MySQL数据库
2. **容器模式**：数据仅存储在内存中，程序退出后数据丢失

## 命令行参数

### 基本参数

- `--port PORT`: 设置服务器端口（默认：58010）
- `--host HOST`: 设置服务器主机地址（默认：0.0.0.0）
- `--help`: 显示帮助信息

### 模式控制参数

- `--no-database`: 启用容器模式，不使用数据库

### 数据库配置参数（仅在数据库模式下有效）

- `--db-host HOST`: 数据库主机地址（默认：122.51.67.113）
- `--db-user USER`: 数据库用户名（默认：cpp_work）
- `--db-password PASS`: 数据库密码（默认：yuzaoqian123）
- `--db-name NAME`: 数据库名称（默认：cpp_work）
- `--db-port PORT`: 数据库端口（默认：3306）

## 使用示例

### 1. 容器模式（无数据库）

```bash
# 使用默认端口启动容器模式
./geometry_server --no-database

# 使用自定义端口启动容器模式
./geometry_server --no-database --port 8081
```

**特点：**
- 数据仅存储在内存中
- 程序重启后数据丢失
- 不需要数据库连接
- 启动速度快
- 适合测试和开发

### 2. 数据库模式（默认）

```bash
# 使用默认数据库配置
./geometry_server

# 使用自定义数据库配置
./geometry_server --db-host localhost --db-user myuser --db-password mypass --db-name mydb

# 使用自定义端口和数据库配置
./geometry_server --port 8082 --db-host localhost --db-user root --db-password password
```

**特点：**
- 数据持久化到MySQL数据库
- 程序重启后数据保持
- 需要有效的数据库连接
- 支持数据备份和恢复
- 适合生产环境

## API 接口

无论使用哪种模式，API接口都保持一致：

- `GET /health` - 健康检查
- `GET /primitives` - 获取所有图元
- `GET /primitives/{id}` - 获取单个图元
- `POST /primitives` - 创建新图元
- `PUT /primitives/{id}` - 更新图元
- `DELETE /primitives/{id}` - 删除图元
- `DELETE /primitives` - 清空所有图元
- `GET /statistics` - 获取统计信息
- `GET /primitives/type/{type}` - 按类型查询图元

## 测试示例

### 创建图元

```bash
curl -X POST http://localhost:58010/primitives \
  -H "Content-Type: application/json" \
  -d '{
    "type": "Circle",
    "name": "测试圆形",
    "x": 100,
    "y": 100,
    "z": 0,
    "color": "red",
    "radius": 50
  }'
```

### 获取所有图元

```bash
curl http://localhost:58010/primitives
```

### 获取统计信息

```bash
curl http://localhost:58010/statistics
```

## 注意事项

1. **容器模式**：数据不会持久化，程序重启后所有数据丢失
2. **数据库模式**：需要确保数据库服务正常运行且连接配置正确
3. **端口冲突**：确保指定的端口没有被其他程序占用
4. **权限问题**：确保程序有权限访问指定的数据库

## 故障排除

### 数据库连接失败

如果数据库模式启动失败，请检查：
- 数据库服务是否正常运行
- 连接参数是否正确
- 网络连接是否正常
- 用户权限是否足够

### 端口占用

如果提示端口被占用，请：
- 使用 `--port` 参数指定其他端口
- 或者停止占用该端口的其他程序

### 权限问题

确保程序有足够的权限：
- 访问数据库
- 绑定指定端口
- 读写必要的文件