#include<iostream>
#include<string>
#include<memory>
#include<RTGC/RTGC.hpp>
#include<boost/timer.hpp>

using namespace std;

class T1A;
class T1B;
class T2A;
class T2B;

class T1A {
    CLASSLINK(T1A, 2)
    string str;
public:
    RTGC::SnapPtr<T1B> next;
    T1A(string str):str(str){}
    T1A(const T1A &t):str(t.str){}
    ~T1A() {
        cout << str << " destructor\n";
    }
    void print(void *ance);
    friend ostream& operator<<(ostream &ostr, T1A &t) {
        // else {
        //     auto &s = *t.next;
        //     ostr << " -> " << s;
        // }
        return ostr;
    }
};

class T1B {
    CLASSLINK(T1B, 2)
    string str;
public:
    RTGC::SnapPtr<T1A> next;
    T1B(string str):str(str){}
    T1B(const T1B &t):str(t.str){}
    ~T1B() {
        cout << str << " destructor\n";
    }
    void print(void *ance);
    friend ostream& operator<<(ostream &ostr, T1B &t) {
        ostr << t.str;
        return ostr;
    }
};
void T1A::print(void *ance = nullptr) {
    if(ance != this) {
        cout << str << " -> ";
        if(next == nullptr)
            cout << "nullptr";
        else if(ance == nullptr)
            next->print(this);
        else
            next->print(ance);
    } else {
        cout << str << endl;
    }
}
void T1B::print(void *ance = nullptr) {
    if(ance != this) {
        cout << str << " -> ";
        if(next == nullptr)
            cout << "nullptr";
        else if(ance == nullptr)
            next->print(this);
        else
            next->print(ance);
    } else {
        cout << str << endl;
    }
}



class T2B {
    CLASSLINK(T2B, 2)
    string str;
public:
    RTGC::SnapPtr<T2A> next;
    T2B(string str):str(str){}
    T2B(const T2B &t):str(t.str){}
    ~T2B() {
        cout << str << " destructor\n";
    }
    void print(void *ance);
    friend ostream& operator<<(ostream &ostr, T2B &t) {
        ostr << t.str;
        return ostr;
    }
};

class T2A {
    CLASSLINK(T2A, 2)
    string str;
public:
    T2B next;
    T2A(string str1, string str2):str(str1), next(str2){}
    ~T2A() {
        cout << str << " destructor\n";
    }
    void print(void *ance);
    friend ostream& operator<<(ostream &ostr, T2A &t) {
        // else {
        //     auto &s = *t.next;
        //     ostr << " -> " << s;
        // }
        return ostr;
    }
};

void T2A::print(void *ance = nullptr) {
    if(ance != this) {
        cout << str << " -> ";
        if(ance == nullptr)
            next.print(this);
        else
            next.print(ance);
    } else {
        cout << str << endl;
    }
}
void T2B::print(void *ance = nullptr) {
    if(ance != this) {
        cout << str << " -> ";
        if(next == nullptr)
            cout << "nullptr";
        else if(ance == nullptr)
            next->print(this);
        else
            next->print(ance);
    } else {
        cout << str << endl;
    }
}



int main() {
    cout << "Test1" << endl;
    {
        RTGC::RootPtr<T1A> a1(new RTGC::CorePtr<T1A>("a1"));
        RTGC::RootPtr<T1A> a2(new RTGC::CorePtr<T1A>("a2"));
        a1->next = new RTGC::CorePtr<T1B>("b1");
        a2->next = new RTGC::CorePtr<T1B>("b2");
        a1->next->next = a2;
        a2->next->next = a1;
        a1->print();
        a2->print();
    }
    cout << "Test2" << endl;
    {
        RTGC::RootPtr<T2A> a1(new RTGC::CorePtr<T2A>("a1", "b1"));
        RTGC::RootPtr<T2A> a2(new RTGC::CorePtr<T2A>("a2", "b2"));
        a1->next.next = a2;
        a2->next.next = a1;
        a1->print();
        a2->print();
    }
}

