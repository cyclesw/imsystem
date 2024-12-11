//
// Created by lang liu on 2024/12/6.
//
#include <iostream>
#include <utility> // For std::move
#include <string>

class Demo {
private:
    int* data;

public:
    // 构造函数
    Demo(int value) : data(new int(value)) {
        std::cout << "构造函数被调用，创建了对象，值 = " << *data << std::endl;
    }

    // 拷贝构造函数
    Demo(const Demo& other) : data(new int(*(other.data))) {
        std::cout << "拷贝构造函数被调用，复制了对象，值 = " << *data << std::endl;
    }

    // 移动构造函数
    Demo(Demo&& other) noexcept : data(other.data) {
        std::cout << "移动构造函数被调用，移动了对象，值 = " << *data << std::endl;
        other.data = nullptr; // 避免析构时删除原始数据
    }

    // 析构函数
    ~Demo() {
        std::cout << "析构函数被调用，销毁了对象，值 = " << *data << std::endl;
        delete data;
    }

    // 拷贝赋值运算符
    Demo& operator=(const Demo& other) {
        if (this != &other) {
            delete data; // 释放旧资源
            data = new int(*(other.data)); // 拷贝新资源
            std::cout << "拷贝赋值运算符被调用，赋值了对象，值 = " << *data << std::endl;
        }
        return *this;
    }

    // 移动赋值运算符
    Demo& operator=(Demo&& other) noexcept {
        if (this != &other) {
            delete data; // 释放旧资源
            data = other.data; // 移动资源
            other.data = nullptr; // 避免析构时删除原始数据
            std::cout << "移动赋值运算符被调用，移动了对象，值 = " << *data << std::endl;
        }
        return *this;
    }
};

int main() {
    Demo a(10); // 构造函数
    Demo b = a; // 拷贝构造函数
    Demo c = std::move(b); // 移动构造函数，b 的析构函数不会被调用，因为它的资源被移动了
    Demo d(20);
    d = a; // 拷贝赋值运算符
    Demo e(30);
    e = std::move(c); // 移动赋值运算符，c 的析构函数不会被调用，因为它的资源被移动了

    return 0; // 析构函数将被调用，销毁所有对象
}
