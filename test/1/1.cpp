#include<iostream>
#include<string>
#include<memory>
#include<RTGC/RTGC.hpp>

using namespace std;


class T1 {
    CLASSLINK(T1, 2)
    // this equals:
    // public:
    // inline void LinkAnce(void *n, void *o) {
    //     next.LinkAnce(n, o);
    //     // std::cout << "l\n";
    // }
    // inline void LinkInit(void *n) {
    //     next.LinkInit(n);
    //     // std::cout << "2\n";
    // }
private:
    string str;
public:
    RTGC::ShellPtr<T1> next;

public:
    T1(string str):str(str){}
    T1(T1 &t):str(t.str){}
    ~T1() {
        cout << str << " destructor\n";
    }
    friend ostream& operator<<(ostream &ostr, T1 &t) {
        ostr << t.str;
        if(t.next == nullptr)
            ostr << " -> nullptr";
        else
            ostr << " -> " << t.next->str;
        return ostr;
    }
};

class T2 {
    string str;
public:
    shared_ptr<T2> next;

public:
    T2(string str):str(str){}
    T2(T2 &t):str(t.str){}
    ~T2() {
        cout << str << " destructor\n";
    }
    friend ostream& operator<<(ostream &ostr, T2 &t) {
        ostr << t.str;
        if(t.next == nullptr)
            ostr << " -> nullptr";
        else
            ostr << " -> " << t.next->str;
        return ostr;
    }
};

int main() {
    cout << "Test1" << endl;
    {
        RTGC::ShellPtr<T1> t1a(new RTGC::CorePtr<T1>("t1"));
        RTGC::ShellPtr<T1> t1b(t1a);
        RTGC::ShellPtr<T1> t2 = new RTGC::CorePtr<T1>("t2");
        t1a->next = t2;
        t2->next = t1a;
        cout << "t1a : " <<  *t1a << endl;
        cout << "t1b : " <<  *t1b << endl;
        cout << "t2 : " <<  *t2 << endl;
    }
    cout << "Test2" << endl;
    {
        shared_ptr<T2> t1a(new T2("t1"));
        shared_ptr<T2> t1b(t1a);
        shared_ptr<T2> t2; t2.reset(new T2("t2"));
        t1a->next = t2;
        t2->next = t1a;
        cout << "t1a : " <<  *t1a << endl;
        cout << "t1b : " <<  *t1b << endl;
        cout << "t2 : " <<  *t2 << endl;
    }
    return 0;
}