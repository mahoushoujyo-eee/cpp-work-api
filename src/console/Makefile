# Makefile for Console Demo
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LIBS = -ljsoncpp

# 源文件目录
MODELS_DIR = ../models
CONSOLE_DIR = .

# 源文件
SOURCES = console_demo.cpp \
          $(MODELS_DIR)/GeometryPrimitive.cpp \
          $(MODELS_DIR)/Shape2D.cpp \
          $(MODELS_DIR)/Shape3D.cpp \
          $(MODELS_DIR)/GeometryContainer.cpp

# 目标文件
OBJECTS = $(SOURCES:.cpp=.o)

# 可执行文件名
TARGET = console_demo

# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

# 编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJECTS) $(TARGET)

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 调试运行
debug: $(TARGET)
	gdb ./$(TARGET)

# 显示帮助
help:
	@echo "可用的目标:"
	@echo "  all     - 编译程序 (默认)"
	@echo "  clean   - 清理编译文件"
	@echo "  run     - 编译并运行程序"
	@echo "  debug   - 使用gdb调试程序"
	@echo "  help    - 显示此帮助信息"

.PHONY: all clean run debug help