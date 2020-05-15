# RTGC
一个用于C++的智能指针，解决了环引用问题，可自动管理内存，无需人工释放

[最新文档](http://github.com/RTGC/)

### 简单示例
```c++
// requires: C++17
#include<iostream>
#include<memory>
#include<string>
#include<RTGC/RTGC.hpp>

class T1 {//use RTGC
    RTGC_AutoChainLink(T1, 2)
private:
    std::string str;
public:
    RTGC::SnapPtr<T1> next;
    T1(std::string &&str):str(str){}
    ~T1() {
        std::cout << str << " destructor\n";
    }
};

class T2 {//use shared_ptr
private:
    std::string str;
public:
    std::shared_ptr<T2> next;
    T2(std::string &&str):str(str){}
    ~T2() {
        std::cout << str << " destructor\n";
    }
};

int main() {
    std::cout << "RTGC:" << std::endl;
    {
        RTGC::RootPtr<T1> t1(new RTGC::ChainCore<T1>("t1"));
        RTGC::RootPtr<T1> t2(new RTGC::ChainCore<T1>("t2"));
        t1->next = t2;
        t2->next = t1;
    }
    std::cout << "shared_ptr:" << std::endl;
    {
        std::shared_ptr<T2> t1(new T2("t1"));
        std::shared_ptr<T2> t2(new T2("t2"));
        t1->next = t2;
        t2->next = t1;
    }
    return 0;
}
```
输出:
```
RTGC:
t1 destructor
t2 destructor
shared_ptr:
```

### 前提及限制

* 支持C++17的编译器（支持：GCC, Clang）
* 所有使用的有引用关系的类均需按规范改写（暂未支持大部分STL容器及算法；仅支持std::string等无引用关系的类）

### 许可证

使用MS-RL半开源发布
