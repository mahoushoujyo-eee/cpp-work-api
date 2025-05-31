#!/bin/bash

# 测试几何图元管理器的不同模式

echo "=== 几何图元管理器模式测试 ==="
echo

echo "1. 测试容器模式（无数据库）"
echo "启动命令: ./geometry_server --no-database --port 8081"
echo "说明: 使用内存容器存储数据，不连接数据库"
echo

echo "2. 测试数据库模式（默认）"
echo "启动命令: ./geometry_server --port 8082"
echo "说明: 使用数据库存储数据，需要配置数据库连接"
echo

echo "3. 自定义数据库配置"
echo "启动命令: ./geometry_server --db-host localhost --db-user myuser --db-password mypass --db-name mydb --port 8083"
echo "说明: 使用自定义数据库配置"
echo

echo "4. 查看帮助信息"
echo "启动命令: ./geometry_server --help"
echo

echo "=== API 测试示例 ==="
echo
echo "# 创建图元（POST请求）"
echo 'curl -X POST http://localhost:8081/primitives -H "Content-Type: application/json" -d '\''{
  "type": "Circle",
  "name": "测试圆形",
  "x": 100,
  "y": 100,
  "z": 0,
  "color": "red",
  "radius": 50
}'\'''
echo
echo "# 获取所有图元（GET请求）"
echo "curl http://localhost:8081/primitives"
echo
echo "# 获取统计信息（GET请求）"
echo "curl http://localhost:8081/statistics"
echo