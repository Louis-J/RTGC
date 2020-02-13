#include<iostream>
#include<string>
#include<memory>
#include<RTGC/RTGC.hpp>
#include"memUse.hpp"
#include<boost/timer.hpp>

using namespace std;


class T1 {
    CLASSLINK(T1, 2)
    string str;
public:
    RTGC::ShellPtr<T1> next;
    T1(string str):str(str){}
    T1(const T1 &t):str(t.str){}
    ~T1() {
        // cout << str << " destructor\n";
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

#define LOOPSIZE 10000000
int main() {
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test1" << endl;
    {
        const size_t memUse = getCurrentRSS();
        const boost::timer timeBegin;
        for (int i = 0; i < LOOPSIZE; i++) {
            RTGC::ShellPtr<T1> t1a(new RTGC::CorePtr<T1>("t1"));
            RTGC::ShellPtr<T1> t1b(t1a);
            RTGC::ShellPtr<T1> t2 = new RTGC::CorePtr<T1>("t2");
            t1a->next = t2;
            t2->next = t1a;
            // cout << "t1a : " <<  *t1a << endl;
            // cout << "t1b : " <<  *t1b << endl;
            // cout << "t2 : " <<  *t2 << endl;
        }
        const uint32_t timeBy = timeBegin.elapsed() * 1000;
        cout << "time1:" << timeBy << "ms" << endl;
        cout << "memory use1:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test2" << endl;
    {
        const size_t memUse = getCurrentRSS();
        const boost::timer timeBegin;
        for (int i = 0; i < LOOPSIZE; i++) {
            shared_ptr<T2> t1a(new T2("t1"));
            shared_ptr<T2> t1b(t1a);
            shared_ptr<T2> t2; t2.reset(new T2("t2"));
            t1a->next = t2;
            t2->next = t1a;
            // cout << "t1a : " <<  *t1a << endl;
            // cout << "t1b : " <<  *t1b << endl;
            // cout << "t2 : " <<  *t2 << endl;
        }
        const uint32_t timeBy = timeBegin.elapsed() * 1000;
        cout << "time2:" << timeBy << "ms" << endl;
        cout << "memory use2:" << getCurrentRSS() - memUse << endl;
    }
}

