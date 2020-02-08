#include<iostream>
#include<string>
#include<memory>
#include<RTGC.hpp>
#include <boost/timer.hpp>

using namespace std;


class Test1 {
    CLASSLINK(Test1, 2)
    string str;
public:
    RTGC::agent_ptr<Test1> next;
    Test1(string str):str(str){}
    Test1(const Test1 &t):str(t.str){}
    ~Test1() {
        // cout << str << " destructor\n";
    }
    friend ostream& operator<<(ostream &ostr, Test1 &t) {
        ostr << t.str;
        if(t.next.isNull())
            ostr << " -> nullptr";
        else
            ostr << " -> " << t.next->str;
        return ostr;
    }
};

class Test2 {
    CLASSLINK(Test2, 1)
    string str;
public:
    Test2(string str):str(str){}
    Test2(const Test2 &t):str(t.str){}
    ~Test2() {
        cout << str << " destructor\n";
    }
    friend ostream& operator<<(ostream &ostr, Test2 &t) {
        ostr << t.str;
        return ostr;
    }
};

class Test3 {
    string str;
public:
    shared_ptr<Test3> next;
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
    
    cout << "Test1" << endl;
    {
        const boost::timer timeBegin;
        for (int i = 0; i < 100000; i++) {
            RTGC::root_ptr<Test1> t1a(new RTGC::inner_ptr<Test1>("t1"));
            RTGC::root_ptr<Test1> t1b(t1a);
            RTGC::root_ptr<Test1> t2 = new RTGC::inner_ptr<Test1>("t2");
            t1a->next = t2;
            t2->next = t1a;
            // cout << "t1a : " <<  *t1a << endl;
            // cout << "t1b : " <<  *t1b << endl;
            // cout << "t2 : " <<  *t2 << endl;
            
            // RTGC::root_ptr<Test2> t3(new RTGC::inner_ptr<Test2>("haha"));
            // cout << *t3 << endl;
        }
        const uint32_t timeBy = timeBegin.elapsed() * 1000;
        cout << "time1:" << timeBy << "ms" << endl;
    }
    cout << "Test3" << endl;
    {
        const boost::timer timeBegin;
        for (int i = 0; i < 100000; i++) {
            shared_ptr<Test3> t1a(new Test3("t1"));
            shared_ptr<Test3> t1b(t1a);
            shared_ptr<Test3> t2; t2.reset(new Test3("t2"));
            t1a->next = t2;
            t2->next = t1a;
            // cout << "t1a : " <<  *t1a << endl;
            // cout << "t1b : " <<  *t1b << endl;
            // cout << "t2 : " <<  *t2 << endl;
        }
        const uint32_t timeBy = timeBegin.elapsed() * 1000;
        cout << "time3:" << timeBy << "ms" << endl;
    }
}

