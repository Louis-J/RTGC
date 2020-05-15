#include<iostream>
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

using namespace std;
using namespace RTGC;

#define HAVE_CNS 1
#define HAVE_DES 0

struct CirNodeA {
    using TPtr = CirNodeA*;
    static auto Alloc(int val) {
        return new CirNodeA(val);
    }

    static size_t cnsNum;
    int val;
    TPtr next;
    CirNodeA(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    int GetLast() {
        auto n = next;
        for(; n->next != this; n = n->next) {}
        return n->val;
    }
    static TPtr Create(int num) {
        TPtr head(new CirNodeA(0));
        TPtr next(head);
        for(auto i = 0; i < num; i++) {
            next->next = new CirNodeA(i);
            next = next->next;
        }
        next->next = head->next;
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, TPtr c) {
        if(c == nullptr)
            return ostr;
        ostr << c->val;
        for(auto n = c->next; n != c; n = n->next)
            ostr << "->" << n->val;
        return ostr;
    }
    ~CirNodeA() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct CirNodeB {
    class TPtr : public atomic<CirNodeB*> {
        typedef atomic<CirNodeB*> Base;
    public:
        TPtr(CirNodeB *c) : Base(c) {}
        TPtr(TPtr &t) : Base(t.load()) {}
        CirNodeB *operator->() { return load(); }
        CirNodeB *operator=(TPtr &t) { return Base::operator=(t.load()); }
        CirNodeB *operator=(TPtr &&t) { return Base::operator=(t.load()); }
    };
    // using TPtr = atomic<CirNodeB*>;
    static TPtr Alloc(int val) {
        return new CirNodeB(val);
    }

    static size_t cnsNum;
    int val;
    TPtr next;
    CirNodeB(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    int GetLast() {
        TPtr n = next;
        for(; n->next != this; n = n->next) {}
        return n->val;
    }
    static TPtr Create(int num) {
        TPtr head(new CirNodeB(0));
        TPtr next(head);
        for(auto i = 0; i < num; i++) {
            next->next = new CirNodeB(i);
            next = next->next;
        }
        next->next = head->next;
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, TPtr &c) {
        if(c == nullptr)
            return ostr;
        ostr << c->val;
        for(TPtr n = c->next; n != c; n = n->next)
            ostr << "->" << n->val;
        return ostr;
    }
    ~CirNodeB() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct CirNodeC {
    using TPtr = shared_ptr<CirNodeC>;
    static TPtr Alloc(int val) {
        return make_shared<CirNodeC>(val);
    }

    static size_t cnsNum;
    int val;
    TPtr next;
    CirNodeC(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    int GetLast() {
        auto n = next;
        for(; n->next.get() != this; n = n->next) {}
        return n->val;
    }
    static TPtr Create(int num) {
        TPtr head = make_shared<CirNodeC>(0);
        TPtr next(head);
        for(auto i = 0; i < num; i++) {
            next->next = make_shared<CirNodeC>(i);
            next = next->next;
        }
        next->next = head->next;
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, TPtr &c) {
        if(c == nullptr)
            return ostr;
        ostr << c->val;
        for(auto n = c->next; n != c; n = n->next)
            ostr << "->" << n->val;
        return ostr;
    }
    ~CirNodeC() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct CirNodeD {
    using TPtr = ChainPtr<CirNodeD>;
    static auto Alloc(int val) {
        return MakeChain<CirNodeD>(val);
    }

    static size_t cnsNum;
    int val;
    TPtr next;
    CirNodeD(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }

    int GetLast() {
        auto n = next;
        for(; n->next != this; n = n->next) {}
        return n->val;
    }
    static TPtr Create(int num) {
        TPtr head(MakeChain<CirNodeD>(0));
        TPtr next(head);
        for(auto i = 0; i < num; i++) {
            next->next = MakeChain<CirNodeD>(i);
            next = next->next;
        }
        next->next = head->next;
        #if 0
        return head->next;
        #else
        auto ret = head->next;
        MakeOptimisedDes(head, next);
        return ret;
        #endif
    }
    friend ostream& operator<<(ostream& ostr, TPtr &c) {
        if(c == nullptr)
            return ostr;
        ostr << c->val;
        for(auto n = c->next; n != c; n = n->next)
            ostr << "->" << n->val;
        return ostr;
    }
    ~CirNodeD() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
    RTGC_AutoChainLink(CirNodeD, 2);
};
namespace RTGC { namespace detail {
template<class T, class U, std::enable_if_t<RTGC_IsComplex<T>::value && std::is_same<T, CirNodeD>::value, int> = 0>
constexpr bool CanReferTo() {
    using V = decltype(std::declval<CirNodeD>().ToTuple());
    return MakeCanReferTo<V, U>();
}
}}
static constexpr bool Dcir = RTGC::detail::CanReferTo<CirNodeD, CirNodeD>();


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
size_t CirNodeB::cnsNum = 0;
size_t CirNodeC::cnsNum = 0;
size_t CirNodeD::cnsNum = 0;


constexpr size_t LOOPSIZE = 20000;
using CirType = CirNodeD;

int main() {
    cout << Dcir << endl;
    return 0;

    cout << "Result" << endl;
    for(auto i = 20; i < 50; i++) {
        auto l = CirType::Create(i);
        cout << "list: " << l << endl;
        cout << "last: " << l->GetLast() << endl;
        cout << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "TimeTest" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        t.start();
        for (size_t i = 0; i < LOOPSIZE; i++) {
            for(auto j = 20; j < 50; j++)
                CirType::Create(j)->GetLast();
        }
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << "ms" << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << "ms" << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << "ms" << endl; //输出：start()至调用此函数的系统时间
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << "Construct Number:" << endl;
    cout << CirType::cnsNum << endl;
}

