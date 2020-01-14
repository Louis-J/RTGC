#include<iostream>
#include<string>
#include<memory>
#include<RTGC.hpp>

using namespace std;

class Test1 {
private:
    string str;
public:
    Test1(string str):str(str){}
    Test1(Test1 &t):str(t.str){}
    ~Test1() {
        cout << str << " destructor\n";
    }
    void print() {
        cout << str << '\n';
    }
};


class Test2 {
public:
    string str;
    agent_ptr<Test2> next;
public:
    void LinkAnce(void *n, void *o = nullptr) {
        next.LinkAnce(n, o);
    }
    Test2(string str):str(str){}
    Test2(Test2 &t):str(t.str){}
    ~Test2() {
        cout << str << " destructor\n";
    }
    friend ostream& operator<<(ostream &ostr, Test2 &t) {
        ostr << t.str;
        if(t.next.isNull())
            ostr << " -> nullptr";
        else
            ostr << " -> " << t.next->str;
        return ostr;
    }
};

class Test3 {
public:
    string str;
    shared_ptr<Test3> next;
public:
    Test3(string str):str(str){}
    Test3(Test3 &t):str(t.str){}
    ~Test3() {
        cout << str << " destructor\n";
    }
    friend ostream& operator<<(ostream &ostr, Test3 &t) {
        ostr << t.str;
        if(t.next == nullptr)
            ostr << " -> nullptr";
        else
            ostr << " -> " << t.next->str;
        return ostr;
    }
};

int main() {
    // cout << "Test1" << endl;
    // {
    //     outer_ptr<Test1> t1a(new inner_ptr<Test1>("t1"));
    //     outer_ptr<Test1> t1b(move(t1a));
    //     outer_ptr<Test1> t2 = new inner_ptr<Test1>("t2");
    // }
    
    // cout << "Test2" << endl;
    // {
    //     outer_ptr<Test2> t1a(new inner_ptr<Test2>("t1"));
    //     outer_ptr<Test2> t1b(move(t1a));
    //     outer_ptr<Test2> t2 = new inner_ptr<Test2>("t2");
    //     cout << "t1a : " <<  *t1a << endl;
    //     cout << "t1b : " <<  *t1b << endl;
    //     cout << "t2 : " <<  *t2 << endl;
    // }
    // cout << "Test3" << endl;
    // {
    //     outer_ptr<Test2> t1a(new inner_ptr<Test2>("t1"));
    //     outer_ptr<Test2> t1b(move(t1a));
    //     outer_ptr<Test2> t2 = new inner_ptr<Test2>("t2");
    //     t1a->next = t2;
    //     cout << "t1a : " <<  *t1a << endl;
    //     cout << "t1b : " <<  *t1b << endl;
    //     cout << "t2 : " <<  *t2 << endl;
    //     cout << "b0" << endl;
    //     t1a = new inner_ptr<Test2>("333");
    //     cout << "b1" << endl;
    //     t1b = t1a;
    //     cout << "b2" << endl;
    //     t2 = t1a;
    //     cout << "b3" << endl;
    //     cout << *t2 << endl;
    // }
    cout << "Test4" << endl;
    {
        root_ptr<Test2> t1a(new inner_ptr<Test2>("t1"));
        root_ptr<Test2> t1b(t1a);
        root_ptr<Test2> t2 = new inner_ptr<Test2>("t2");
        t1a->next = t2;
        t2->next = t1a;
        cout << "t1a : " <<  *t1a << endl;
        cout << "t1b : " <<  *t1b << endl;
        cout << "t2 : " <<  *t2 << endl;
    }
    cout << "Test5" << endl;
    {
        shared_ptr<Test3> t1a(new Test3("t1"));
        shared_ptr<Test3> t1b(t1a);
        shared_ptr<Test3> t2; t2.reset(new Test3("t2"));
        t1a->next = t2;
        t2->next = t1a;
        cout << "t1a : " <<  *t1a << endl;
        cout << "t1b : " <<  *t1b << endl;
        cout << "t2 : " <<  *t2 << endl;
    }
    return 0;
}