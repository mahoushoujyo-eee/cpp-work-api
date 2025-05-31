# 几何图元控制台演示程序

这是一个完整的控制台演示程序，展示了 `models` 文件夹中所有几何图元类和容器类的功能。

## 功能特性

### 1. 几何图元类
- **GeometryPrimitive**: 几何图元基类
- **Shape2D**: 2D图元基类（Rectangle, Circle）
- **Shape3D**: 3D图元基类（Cuboid, Cylinder）
- **Position**: 位置类

### 2. 容器类
- **GeometryContainer**: 基于双向链表的几何图元容器

### 3. 演示的方法

#### 图元基本方法
- 构造函数和析构函数
- `area()` - 计算面积/表面积
- `perimeter()` - 计算周长（2D图元）
- `volume()` - 计算体积（3D图元）
- `surfaceArea()` - 计算表面积（3D图元）
- `print()` - 打印图元信息
- `toJson()` / `fromJson()` - JSON序列化/反序列化

#### Getter方法
- `getId()`, `getName()`, `getPosition()`, `getColor()`, `getType()`
- `getWidth()`, `getHeight()` (Rectangle)
- `getRadius()` (Circle, Cylinder)
- `getLength()` (Cuboid)

#### Setter方法
- `setId()`, `setName()`, `setPosition()`, `setColor()`
- `setWidth()`, `setHeight()` (Rectangle)
- `setRadius()` (Circle, Cylinder)
- `setLength()` (Cuboid)

#### 容器操作方法
- `addPrimitive()` - 添加图元
- `deletePrimitive()` - 根据ID删除图元
- `deletePrimitiveByIndex()` - 根据索引删除图元
- `updatePrimitive()` - 更新图元
- `size()` - 获取容器大小
- `empty()` - 检查容器是否为空
- `clear()` - 清空容器
- `at()` - 根据索引访问图元

#### 查询方法
- `findById()` - 根据ID查找
- `findByName()` - 根据名称查找
- `findByType()` - 根据类型查找
- `findByColor()` - 根据颜色查找
- `findByAreaRange()` - 根据面积范围查找
- `findByCondition()` - 自定义条件查找
- `getAllPrimitives()` - 获取所有图元

#### 排序方法
- `sortById()` - 按ID排序
- `sortByArea()` - 按面积排序
- `sortByName()` - 按名称排序

#### 统计方法
- `printAll()` - 打印所有图元
- `printStatistics()` - 打印统计信息

## 编译和运行

### 前置条件
确保系统已安装：
- g++ (支持C++17)
- libjsoncpp-dev

在Ubuntu/Debian系统上安装依赖：
```bash
sudo apt update
sudo apt install g++ libjsoncpp-dev
```

### 编译
```bash
cd /workspace/cpp_backend/src/console
make
```

### 运行
```bash
make run
```

或者直接运行：
```bash
./console_demo
```

### 调试
```bash
make debug
```

### 清理
```bash
make clean
```

## 程序结构

程序分为7个主要演示部分：

1. **图元创建和基本方法** - 演示各种图元的创建、基本计算和属性访问
2. **容器基本操作** - 演示容器的添加、访问、状态查询等
3. **查询操作** - 演示各种查询方法的使用
4. **排序操作** - 演示不同排序方式
5. **修改和删除操作** - 演示图元的更新和删除
6. **JSON序列化操作** - 演示JSON序列化功能
7. **统计信息** - 演示统计功能和容器清空

## 示例输出

程序会输出详细的演示信息，包括：
- 图元的创建和属性信息
- 各种查询结果
- 排序前后的对比
- JSON序列化结果
- 统计分析数据

## 文件结构

```
src/console/
├── console_demo.cpp    # 主程序文件
├── Makefile           # 编译配置
└── README.md          # 说明文档

src/models/            # 依赖的模型类
├── GeometryPrimitive.h/.cpp
├── Shape2D.h/.cpp
├── Shape3D.h/.cpp
└── GeometryContainer.h/.cpp
```

## 注意事项

1. 程序使用了C++17标准，确保编译器支持
2. 需要链接jsoncpp库用于JSON操作
3. 程序包含详细的中文注释和输出，便于理解
4. 所有异常都有适当的处理
5. 内存管理使用智能指针，自动处理资源释放