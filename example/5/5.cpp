#include<iostream>
#include<array>
#include<vector>
#include<tuple>
#include<memory>
#include<RTGC/RTGC.hpp>
#include"memUse.hpp"
#include<boost/timer/timer.hpp>

#ifdef USE_TCMALLOC
#include"tcmalloc.h"
void * makeTCMallocUsed = tc_malloc(4);
#endif

#include <random>
#include <thread>
		
// #include <algorithm>
// #include <iterator>
// #include <iostream>

using namespace std;
using namespace RTGC;

// 稍复杂的示意, 以一个循环链表操作展示了性能及内存泄漏情况

#define HAVE_CNS 1
#define HAVE_DES 0

template<typename T>
class AtomicPtr{
    atomic_flag flag = ATOMIC_FLAG_INIT;
    T real;
public:
    AtomicPtr() {}
    AtomicPtr(T &t) {
        while(flag.test_and_set());
        real = t;
        flag.clear();
    }
    AtomicPtr(T &&t) {
        while(flag.test_and_set());
        real = t;
        flag.clear();
    }
    AtomicPtr(AtomicPtr &a) {
        auto t = a.load();
        while(flag.test_and_set());
        real = t;
        flag.clear();
    }
    AtomicPtr(AtomicPtr &&a) {
        auto t = a.load();
        while(flag.test_and_set());
        real = t;
        flag.clear();
    }
    T &operator->() {
        while(flag.test_and_set());
        auto ret = real;
        flag.clear();
        return ret.operator->();
    }
    T load() {
        while(flag.test_and_set());
        auto ret = real;
        flag.clear();
        return ret;
    }
    AtomicPtr &operator=(AtomicPtr &a) {
        auto t = a.load();
        while(flag.test_and_set());
        real = t;
        flag.clear();
        return *this;
    }
    AtomicPtr &operator=(AtomicPtr &&a) {
        // auto t = a.load();
        // while(flag.test_and_set());
        // real = t;
        // flag.clear();
        // return *this;
        return operator=(a);
    }
    AtomicPtr &operator=(T t) {
        while(flag.test_and_set());
        real = t;
        flag.clear();
        return *this;
    }
};

struct CirNodeA {
    using TPtr = CirNodeA*;
    using ATPtr = AtomicPtr<CirNodeA*>;
    static auto Alloc() {
        return new CirNodeA();
    }

    static string TPtrName() {
        return "CirNodeA*";
    }

    static size_t cnsNum;
    TPtr l, r;
    CirNodeA() : l(nullptr), r(nullptr) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    ~CirNodeA() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct CirNodeC {
    using TPtr = shared_ptr<CirNodeC>;
    // using ATPtr = atomic<TPtr>;
    // using ATPtr = std::experimental::atomic_shared_ptr<CirNodeC>;
    using ATPtr = AtomicPtr<TPtr>;
    static TPtr Alloc() {
        return make_shared<CirNodeC>();
    }

    static string TPtrName() {
        return "shared_ptr<CirNodeC>";
    }

    static size_t cnsNum;
    TPtr l, r;
    CirNodeC() : l(nullptr), r(nullptr) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    ~CirNodeC() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct CirNodeD {
    constexpr static bool RTGC_MayCirRef = true;
    RTGC_AutoChainLink(CirNodeD, 2);
    RTGC_AutoCRDetectIn(CirNodeD, 2);

    using TPtr = SmarterPtr<CirNodeD>;
    // using ATPtr = atomic<TPtr>;
    using ATPtr = AtomicPtr<TPtr>;
    static auto Alloc() {
        return MakeSmarter<CirNodeD>();
    }

    static string TPtrName() {
        return "SmarterPtr<CirNodeD>";
    }

    static size_t cnsNum;
    TPtr l, r;
    CirNodeD() : l(nullptr), r(nullptr) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    ~CirNodeD() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};
RTGC_AutoCRDetectOut(CirNodeD);

template<typename T>
ostream& operator<<(ostream& ostr, vector<T>& v) {
    ostr << "{";
    for(auto& t : v){
        ostr << t << ", ";
    }
    return ostr << "}";
}
template<typename T>
ostream& operator<<(ostream& ostr, vector<T>&& v) {
    ostr << "{";
    for(auto& t : v){
        ostr << t << ",";
    }
    return ostr << "}";
}

size_t CirNodeA::cnsNum = 0;
size_t CirNodeC::cnsNum = 0;
size_t CirNodeD::cnsNum = 0;


constexpr size_t LOOPSIZE = 20000;
using CirType = CirNodeD;
constexpr size_t thrNum = 8;
// auto thrNum = std::thread::hardware_concurrency();

template<typename T>
class ThreadProc : public thread {
    using TPtr = typename T::TPtr;
    using ATPtr = typename T::ATPtr;
public:
    static atomic<int> thrId;
    static ATPtr cursors[thrNum];
    static void Test() {
        // atomic<int> thrId = 0;
        // ATPtr cursors[thrNum];

        boost::timer::cpu_timer t;
        t.start();

        ATPtr tp;
        std::cout << "TPtr: " << T::TPtrName() << '\n';
        
        for(auto &i : vector<ThreadProc>(thrNum))
            i.join();
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << endl; //输出：start()至调用此函数的系统时间
        cout << "cnsNum = " << T::cnsNum << endl;
        // cout << "desNum = " << T::desNum << endl;
    };
    // ThreadProc(atomic<int> &thrId, ATPtr cursors[]) :thread([&]() {
    ThreadProc() :thread([&]() {
        int myId = thrId.fetch_add(1);
        cursors[myId] = T::Alloc();
        std::mt19937 g(myId);
        std::uniform_int_distribution<> rThr(1, thrNum);
        std::uniform_int_distribution<> rOp(1, 3);
        for(size_t i = 0; i < LOOPSIZE; i++) {
            switch(rOp(g)) {
            case 0: { //新建
                cursors[myId] = T::Alloc();
            } break;
            case 1: { //左连接
                // cursors[myId].load()->l = TPtr(cursors[rThr(g)]);
                cursors[myId].load()->l = cursors[rThr(g)];
            } break;
            case 2: { //右连接
                cursors[myId].load()->r = TPtr(cursors[rThr(g)]);
            } break;
            case 3: { //删除左
                cursors[myId].load()->l = nullptr;
            } break;
            case 4: { //删除右
                cursors[myId].load()->r = nullptr;
            } break;
            case 5: { //新建左左
                TPtr n = T::Alloc();
                n->l = cursors[myId].load()->l;
                cursors[myId].load()->l = n;
            } break;
            case 6: { //新建左右
                TPtr n = T::Alloc();
                n->r = cursors[myId].load()->l;
                cursors[myId].load()->l = n;
            } break;
            case 7: { //新建右左
                TPtr n = T::Alloc();
                n->l = cursors[myId].load()->r;
                cursors[myId].load()->r = nullptr;
            } break;
            case 8: { //新建右右
                TPtr n = T::Alloc();
                n->r = cursors[myId].load()->r;
                cursors[myId].load()->r = nullptr;
            } break;
            case 9: { //左移
                cursors[myId] = cursors[myId].load()->l;
                if(cursors[myId].load() == nullptr)
                    cursors[myId] = T::Alloc();
            } break;
            case 10: { //左移
                cursors[myId] = cursors[myId].load()->r;
                if(cursors[myId].load() == nullptr)
                    cursors[myId] = T::Alloc();
            } break;
            }
        }
    }) {}
};

int main() {
    cout << endl << "Press any key to run Test1:";
    cin.get();
    ThreadProc<CirNodeA>::Test();

    cout << endl << "Press any key to run Test3:";
    cin.get();
    ThreadProc<CirNodeC>::Test();

    // cout << endl << "Press any key to run Test4:";
    // cin.get();
    // ThreadProc<CirNodeD>::Test();
    cout << "Construct Number:" << endl;
    {
        cout << "1: " << endl;
        cout << CirNodeA::cnsNum << endl;
        cout << "3: " << endl;
        cout << CirNodeC::cnsNum << endl;
        cout << "4: " << endl;
        cout << CirNodeD::cnsNum << endl;
        cout << endl;
    }
}

