#include "GeometryContainer.h"
#include <map>

void GeometryContainer::addPrimitive(std::shared_ptr<GeometryPrimitive> primitive) {
    if (!primitive) {
        std::cerr << "添加的图元指针为空" << std::endl;
        return;
    }
    
    // 查找当前容器中的最大ID
    int maxId = 0;
    Node* current = head;
    while (current) {
        if (current->data->getId() > maxId) {
            maxId = current->data->getId();
        }
        current = current->next;
    }
    
    // 设置新图元的ID为最大ID+1   模仿数据库的AUTO INCREMENT
    primitive->setId(maxId + 1);
    
    Node* newNode = new Node(primitive);
    
    if (!head) {
        // 链表为空
        head = newNode;
        tail = newNode;
    } else {
        // 添加到链表尾部
        newNode->prev = tail;
        tail->next = newNode;
        tail = newNode;
    }
    count++;
}

bool GeometryContainer::deletePrimitive(int id) {
    Node* current = head;
    
    while (current) {
        if (current->data->getId() == id) {
            // 找到要删除的节点
            
            // 调整前后节点的指针
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                // 删除的是头节点
                head = current->next;
            }
            
            if (current->next) {
                current->next->prev = current->prev;
            } else {
                // 删除的是尾节点
                tail = current->prev;
            }
            
            // 删除节点
            delete current;
            count--;
            
            return true;
        }
        
        current = current->next;
    }
    
    return false;
}

bool GeometryContainer::deletePrimitiveByIndex(size_t index) {
    if (index >= count) {
        return false;
    }
    
    Node* current = head;
    
    // 找到指定索引的节点
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    
    // 调整前后节点的指针
    if (current->prev) {
        current->prev->next = current->next;
    } else {
        // 删除的是头节点
        head = current->next;
    }
    
    if (current->next) {
        current->next->prev = current->prev;
    } else {
        // 删除的是尾节点
        tail = current->prev;
    }
    
    // 删除节点
    delete current;
    count--;
    
    return true;
}

bool GeometryContainer::updatePrimitive(int id, std::shared_ptr<GeometryPrimitive> newPrimitive) {
    if (!newPrimitive) {
        return false;
    }
    
    Node* current = head;
    
    while (current) {
        if (current->data->getId() == id) {
            // 找到要更新的节点，更新数据
            current->data = newPrimitive;
            return true;
        }
        
        current = current->next;
    }
    
    return false;
}

std::shared_ptr<GeometryPrimitive> GeometryContainer::findById(int id) const {
    Node* current = head;
    while (current) {
        if (current->data->getId() == id) {
            return current->data;
        }
        current = current->next;
    }
    return nullptr;
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::findByName(const std::string& name) const {
    return findNodes([&name](const std::shared_ptr<GeometryPrimitive>& p) {
        return p->getName() == name;
    });
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::findByType(const std::string& type) const {
    return findNodes([&type](const std::shared_ptr<GeometryPrimitive>& p) {
        return p->getType() == type;
    });
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::findByColor(const std::string& color) const {
    return findNodes([&color](const std::shared_ptr<GeometryPrimitive>& p) {
        return p->getColor() == color;
    });
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::findByAreaRange(double minArea, double maxArea) const {
    return findNodes([minArea, maxArea](const std::shared_ptr<GeometryPrimitive>& p) {
        double area = p->area();
        return area >= minArea && area <= maxArea;
    });
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::findByCondition(
    std::function<bool(const std::shared_ptr<GeometryPrimitive>&)> condition) const {
    return findNodes(condition);
}

std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::getAllPrimitives() const {
    return toVector();
}

size_t GeometryContainer::size() const {
    return count;
}

bool GeometryContainer::empty() const {
    return count == 0;
}

void GeometryContainer::clear() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
    count = 0;
}

std::shared_ptr<GeometryPrimitive> GeometryContainer::at(size_t index) const {
    if (index >= count) {
        throw std::out_of_range("Index out of range");
    }
    
    Node* current = head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

void GeometryContainer::printAll() const {
    std::cout << "=== Geometry Container Contents ===" << std::endl;
    std::cout << "Total primitives: " << count << std::endl;
    
    Node* current = head;
    while (current) {
        current->data->print();
        current = current->next;
    }
    std::cout << "=== End of Container ===" << std::endl;
}

void GeometryContainer::printStatistics() const {
    std::cout << "=== Container Statistics ===" << std::endl;
    std::cout << "Total primitives: " << count << std::endl;
    
    // 按类型统计
    std::map<std::string, int> typeCount;
    double totalArea = 0;
    
    Node* current = head;
    while (current) {
        typeCount[current->data->getType()]++;
        totalArea += current->data->area();
        current = current->next;
    }
    
    std::cout << "By type:" << std::endl;
    for (const auto& pair : typeCount) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    std::cout << "Total area: " << totalArea << std::endl;
    std::cout << "Average area: " << (count == 0 ? 0 : totalArea / count) << std::endl;
    std::cout << "=== End of Statistics ===" << std::endl;
}

void GeometryContainer::sortById() {
    // 将链表转换为向量，排序后重建链表
    std::vector<std::shared_ptr<GeometryPrimitive>> vec = toVector();
    std::sort(vec.begin(), vec.end(),
        [](const std::shared_ptr<GeometryPrimitive>& a, const std::shared_ptr<GeometryPrimitive>& b) {
            return a->getId() < b->getId();
        });
    rebuildFromVector(vec);
}

void GeometryContainer::sortByArea() {
    std::vector<std::shared_ptr<GeometryPrimitive>> vec = toVector();
    std::sort(vec.begin(), vec.end(),
        [](const std::shared_ptr<GeometryPrimitive>& a, const std::shared_ptr<GeometryPrimitive>& b) {
            return a->area() < b->area();
        });
    rebuildFromVector(vec);
}

void GeometryContainer::sortByName() {
    std::vector<std::shared_ptr<GeometryPrimitive>> vec = toVector();
    std::sort(vec.begin(), vec.end(),
        [](const std::shared_ptr<GeometryPrimitive>& a, const std::shared_ptr<GeometryPrimitive>& b) {
            return a->getName() < b->getName();
        });
    rebuildFromVector(vec);
}

// 辅助方法：从向量重建链表
void GeometryContainer::rebuildFromVector(const std::vector<std::shared_ptr<GeometryPrimitive>>& vec) {
    // 先保存向量的副本，避免在clear过程中出现问题
    std::vector<std::shared_ptr<GeometryPrimitive>> tempVec = vec;
    
    clear(); // 清空当前链表
    
    // 重新添加所有元素
    for (const auto& primitive : tempVec) {
        if (primitive) { // 确保指针有效
            addPrimitive(primitive);
        }
    }
}

// 辅助方法：将链表转换为向量
std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::toVector() const {
    std::vector<std::shared_ptr<GeometryPrimitive>> result;
    Node* current = head;
    while (current) {
        result.push_back(current->data);
        current = current->next;
    }
    return result;
}

// 辅助方法：根据条件查找节点并返回向量
std::vector<std::shared_ptr<GeometryPrimitive>> GeometryContainer::findNodes(
    std::function<bool(const std::shared_ptr<GeometryPrimitive>&)> condition) const {
    std::vector<std::shared_ptr<GeometryPrimitive>> result;
    Node* current = head;
    while (current) {
        if (condition(current->data)) {
            result.push_back(current->data);
        }
        current = current->next;
    }
    return result;
}