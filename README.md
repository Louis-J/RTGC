# RTGC - Realtime Tracing Garrbage Collection
一个用于C++的无环形引用问题的智能指针，可以安全的、高效的保证资源的及时回收及确定性析构

# 简单示例
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

# 条件及依赖

* 支持structured bindings(C++17, 用于实现编译期反射)、SFINAE的编译器(用于自动生成保证无循环引用问题操作的代码)
* 已测试通过：GCC 9.2.1 libstdc++, Clang 10 libc++

# 特点及限制

### 特点
* ChainPtr智能指针通过引用链管理算法，保证无循环引用问题; CountPtr智能指针通过引用计数算法，不保证循环引用问题，但性能较高
* 编译期自动判断每个类型是否会出现循环引用，针对性的使用对应的智能指针，且最大程度保证性能
* 仅需对类使用较少的宏，即可自动生成保证无循环引用问题操作的代码
* 指针本身分为线程不安全与线程安全版本。线程不安全版本不保证多线程的安全；线程安全版本保证与shared_ptr相同的安全性(用户应保证:同一MTChainPtr/MTCountPtr不能被多个线程持有; 指针指向的对象也不能被多个线程同时操作)
### 限制
* 由于使用紧凑式的设计，无法支持多继承下的多态(C++多继承下，多态可能会改变指针地址)，因此需要用户保证多态的安全(支持单继承的多态、不改变指针地址的多继承多态，也可用户手动实现多态。跳转至WIKI)
* 所有可能出现在堆的对象，均需按规范改写（暂未适配STL，因此暂不支持大部分STL容器及算法；仅支持std::string等无引用关系的类）

# 原理介绍
见WIKI

# 许可证

使用MS-RL半开源发布
