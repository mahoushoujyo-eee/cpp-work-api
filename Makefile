# Makefile for Geometry Management System with Three-Layer Architecture

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -I/usr/include/jsoncpp -I/usr/include/mysql -I/usr/include/mysql++
LIBS = -ljsoncpp -lmysqlclient -lpthread

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = src/test

# Source files
MAIN_SRC = $(SRC_DIR)/main.cpp
TEST_SRC = $(TEST_DIR)/test_geometry.cpp
LIB_SRCS = $(SRC_DIR)/models/GeometryPrimitive.cpp \
           $(SRC_DIR)/models/Shape2D.cpp \
           $(SRC_DIR)/models/Shape3D.cpp \
           $(SRC_DIR)/models/GeometryContainer.cpp \
           $(SRC_DIR)/dao/GeometryDAO.cpp \
           $(SRC_DIR)/service/GeometryService.cpp \
           $(SRC_DIR)/controller/GeometryController.cpp

# Object files
MAIN_OBJ = $(BUILD_DIR)/main.o
TEST_OBJ = $(BUILD_DIR)/test_geometry.o
LIB_OBJS = $(BUILD_DIR)/GeometryPrimitive.o \
           $(BUILD_DIR)/Shape2D.o \
           $(BUILD_DIR)/Shape3D.o \
           $(BUILD_DIR)/GeometryContainer.o \
           $(BUILD_DIR)/GeometryDAO.o \
           $(BUILD_DIR)/GeometryService.o \
           $(BUILD_DIR)/GeometryController.o

# Executables
SERVER_TARGET = geometry_server
TEST_TARGET = test_geometry
STATIC_SERVER_TARGET = geometry_server_static
STATIC_TEST_TARGET = test_geometry_static

# Default target
all: $(SERVER_TARGET) $(TEST_TARGET)

# Static compilation targets (semi-static: statically link C++ runtime)
static: $(STATIC_SERVER_TARGET) $(STATIC_TEST_TARGET)

# Full static compilation targets (may fail without static libraries)
static-full: $(STATIC_SERVER_TARGET)-full $(STATIC_TEST_TARGET)-full

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build main server
$(SERVER_TARGET): $(BUILD_DIR) $(MAIN_OBJ) $(LIB_OBJS)
	$(CXX) $(MAIN_OBJ) $(LIB_OBJS) -o $(SERVER_TARGET) $(LIBS)
	@echo "Server built successfully: $(SERVER_TARGET)"

# Build test program
$(TEST_TARGET): $(BUILD_DIR) $(TEST_OBJ) $(LIB_OBJS)
	$(CXX) $(TEST_OBJ) $(LIB_OBJS) -o $(TEST_TARGET) $(LIBS)
	@echo "Test program built successfully: $(TEST_TARGET)"

# Build static server (standalone executable with static linking where possible)
$(STATIC_SERVER_TARGET): $(BUILD_DIR) $(MAIN_OBJ) $(LIB_OBJS)
	$(CXX) -static-libgcc -static-libstdc++ $(MAIN_OBJ) $(LIB_OBJS) -o $(STATIC_SERVER_TARGET) $(LIBS) -Wl,-rpath,'$$ORIGIN'
	@echo "Static server built successfully: $(STATIC_SERVER_TARGET)"
	@echo "Note: This is a semi-static build. Some system libraries may still be dynamically linked."

# Build static test program (standalone executable with static linking where possible)
$(STATIC_TEST_TARGET): $(BUILD_DIR) $(TEST_OBJ) $(LIB_OBJS)
	$(CXX) -static-libgcc -static-libstdc++ $(TEST_OBJ) $(LIB_OBJS) -o $(STATIC_TEST_TARGET) $(LIBS) -Wl,-rpath,'$$ORIGIN'
	@echo "Static test program built successfully: $(STATIC_TEST_TARGET)"
	@echo "Note: This is a semi-static build. Some system libraries may still be dynamically linked."

# Build fully static server (requires manual library compilation)
$(STATIC_SERVER_TARGET)-full: $(BUILD_DIR) $(MAIN_OBJ) $(LIB_OBJS)
	@echo "Attempting full static compilation..."
	@echo "Warning: This may fail if static libraries are not available"
	$(CXX) -static $(MAIN_OBJ) $(LIB_OBJS) -o $(STATIC_SERVER_TARGET)-full -L/usr/lib/x86_64-linux-gnu -ljsoncpp -lmysqlclient -lpthread -ldl -lssl -lcrypto -lz -lm -lrt || echo "Full static compilation failed. Use 'make static' for semi-static build."

# Build fully static test program (requires manual library compilation)
$(STATIC_TEST_TARGET)-full: $(BUILD_DIR) $(TEST_OBJ) $(LIB_OBJS)
	@echo "Attempting full static compilation..."
	@echo "Warning: This may fail if static libraries are not available"
	$(CXX) -static $(TEST_OBJ) $(LIB_OBJS) -o $(STATIC_TEST_TARGET)-full -L/usr/lib/x86_64-linux-gnu -ljsoncpp -lmysqlclient -lpthread -ldl -lssl -lcrypto -lz -lm -lrt || echo "Full static compilation failed. Use 'make static' for semi-static build."

# Compile main.cpp
$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(MAIN_SRC) -o $(MAIN_OBJ)

# Compile test_geometry.cpp
$(TEST_OBJ): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(TEST_SRC) -o $(TEST_OBJ)

# Compile library source files
$(BUILD_DIR)/GeometryPrimitive.o: $(SRC_DIR)/models/GeometryPrimitive.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/models/GeometryPrimitive.cpp -o $(BUILD_DIR)/GeometryPrimitive.o

$(BUILD_DIR)/Shape2D.o: $(SRC_DIR)/models/Shape2D.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/models/Shape2D.cpp -o $(BUILD_DIR)/Shape2D.o

$(BUILD_DIR)/Shape3D.o: $(SRC_DIR)/models/Shape3D.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/models/Shape3D.cpp -o $(BUILD_DIR)/Shape3D.o

$(BUILD_DIR)/GeometryContainer.o: $(SRC_DIR)/models/GeometryContainer.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/models/GeometryContainer.cpp -o $(BUILD_DIR)/GeometryContainer.o

$(BUILD_DIR)/GeometryDAO.o: $(SRC_DIR)/dao/GeometryDAO.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/dao/GeometryDAO.cpp -o $(BUILD_DIR)/GeometryDAO.o

$(BUILD_DIR)/GeometryService.o: $(SRC_DIR)/service/GeometryService.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/service/GeometryService.cpp -o $(BUILD_DIR)/GeometryService.o

$(BUILD_DIR)/GeometryController.o: $(SRC_DIR)/controller/GeometryController.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/controller/GeometryController.cpp -o $(BUILD_DIR)/GeometryController.o

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y libmysqlclient-dev libmysql++-dev libjsoncpp-dev build-essential
	@echo "Dependencies installed successfully"

# Install static dependencies for static compilation
install-static-deps:
	@echo "Installing static dependencies..."
	sudo apt-get update
	sudo apt-get install -y libmysqlclient-dev libmysql++-dev libjsoncpp-dev build-essential
	sudo apt-get install -y libssl-dev libz-dev
	@echo "Static dependencies installed successfully"
	@echo "Note: Static compilation may require additional library paths and dependencies"

# Setup MySQL database
setup-db:
	@echo "Setting up MySQL database..."
	@echo "Please run the following commands in MySQL:"
	@echo "CREATE DATABASE geometry_db;"
	@echo "CREATE USER 'geometry_user'@'localhost' IDENTIFIED BY 'geometry_pass';"
	@echo "GRANT ALL PRIVILEGES ON geometry_db.* TO 'geometry_user'@'localhost';"
	@echo "FLUSH PRIVILEGES;"

# Run server
run-server: $(SERVER_TARGET)
	@echo "Starting geometry server..."
	./$(SERVER_TARGET) --host=localhost --port=8080 --db-host=localhost --db-port=3306 --db-name=geometry_db --db-user=geometry_user --db-pass=geometry_pass

# Run tests
run-tests: $(TEST_TARGET)
	@echo "Running geometry tests..."
	./$(TEST_TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	rm -f $(SERVER_TARGET) $(TEST_TARGET) $(STATIC_SERVER_TARGET) $(STATIC_TEST_TARGET) $(STATIC_SERVER_TARGET)-full $(STATIC_TEST_TARGET)-full
	@echo "Clean completed"

# Clean and rebuild
rebuild: clean all

# Development mode (with auto-restart)
dev: $(SERVER_TARGET)
	@echo "Starting development server with auto-restart..."
	@while true; do \
		./$(SERVER_TARGET) --host=localhost --port=8080 --db-host=localhost --db-port=3306 --db-name=geometry_db --db-user=geometry_user --db-pass=geometry_pass; \
		echo "Server stopped. Restarting in 2 seconds..."; \
		sleep 2; \
	done

# Check code style (if clang-format is available)
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "Formatting code..."; \
		find $(SRC_DIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i; \
		echo "Code formatting completed"; \
	else \
		echo "clang-format not found. Install it with: sudo apt-get install clang-format"; \
	fi

# Static analysis (if cppcheck is available)
analyze:
	@if command -v cppcheck >/dev/null 2>&1; then \
		echo "Running static analysis..."; \
		cppcheck --enable=all --std=c++17 $(SRC_DIR); \
	else \
		echo "cppcheck not found. Install it with: sudo apt-get install cppcheck"; \
	fi

# Memory check (if valgrind is available)
memcheck: $(TEST_TARGET)
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "Running memory check..."; \
		valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TARGET); \
	else \
		echo "valgrind not found. Install it with: sudo apt-get install valgrind"; \
	fi

# Help
help:
	@echo "Available targets:"
	@echo "  all               - Build both server and test programs"
	@echo "  static            - Build semi-static versions (C++ runtime statically linked)"
	@echo "  static-full       - Build fully static versions (may fail without static libs)"
	@echo "  server            - Build only the server"
	@echo "  test              - Build only the test program"
	@echo "  install-deps      - Install required dependencies"
	@echo "  install-static-deps - Install dependencies for static compilation"
	@echo "  setup-db          - Show MySQL database setup instructions"
	@echo "  run-server        - Build and run the server"
	@echo "  run-tests         - Build and run tests"
	@echo "  clean             - Remove build files"
	@echo "  rebuild           - Clean and rebuild everything"
	@echo "  dev               - Run server in development mode with auto-restart"
	@echo "  format            - Format code using clang-format"
	@echo "  analyze           - Run static code analysis"
	@echo "  memcheck          - Run memory leak detection on tests"
	@echo "  help              - Show this help message"

# Phony targets
.PHONY: all static static-full clean rebuild install-deps install-static-deps setup-db run-server run-tests dev format analyze memcheck help

# Server alias
server: $(SERVER_TARGET)

# Test alias
test: $(TEST_TARGET)