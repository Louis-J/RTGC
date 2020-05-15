//LeetCode 0025
#include<iostream>
#include<vector>
#include<tuple>
#include<memory>
#include<RTGC/RTGC.hpp>
#include"memUse.hpp"
#include<boost/timer/timer.hpp>
#include"gc_cpp.h"
#include"gc_allocator.h"

#ifdef USE_TCMALLOC
#include"tcmalloc.h"
void * makeTCMallocUsed = tc_malloc(4);
#endif

using namespace std;
using namespace RTGC;

#define HAVE_CNS 1
#define HAVE_DES 1

struct ListNodeA {
    using TPtr = ListNodeA*;
    static auto Alloc(int val) {
        return new ListNodeA(val);
    }

    static size_t cnsNum;
    static size_t cnsNumMax;
    int val;
    ListNodeA *next;
    ListNodeA(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        if(cnsNum >= cnsNumMax)
            cnsNumMax = cnsNum;
        #endif
    }
    static ListNodeA *Create(initializer_list<int>& list) {
        ListNodeA *head(new ListNodeA(0));
        ListNodeA *next(head);
        for(auto& i : list){
            next->next = new ListNodeA(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, ListNodeA *& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, ListNodeA *&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    ~ListNodeA() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct ListNodeB {
    using TPtr = shared_ptr<ListNodeB>;
    static auto Alloc(int val) {
        return make_shared<ListNodeB>(val);
    }

    static size_t cnsNum;
    static size_t cnsNumMax;
    int val;
    shared_ptr<ListNodeB> next;
    ListNodeB(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        if(cnsNum >= cnsNumMax)
            cnsNumMax = cnsNum;
        #endif
    }
    static shared_ptr<ListNodeB> Create(initializer_list<int>& list) {
        shared_ptr<ListNodeB> head = make_shared<ListNodeB>(0);
        shared_ptr<ListNodeB> next(head);
        for(auto& i : list){
            next->next = make_shared<ListNodeB>(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, shared_ptr<ListNodeB>& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, shared_ptr<ListNodeB>&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    ~ListNodeB() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct ListNodeC {
    using TPtr = ChainPtr<ListNodeC>;
    static auto Alloc(int val) {
        return MakeChain<ListNodeC>(val);
    }

    static size_t cnsNum;
    static size_t cnsNumMax;
    RTGC_AutoChainLink(ListNodeC, 2)
    int val;
    ChainPtr<ListNodeC> next;
    ListNodeC(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        if(cnsNum >= cnsNumMax)
            cnsNumMax = cnsNum;
        #endif
    }
    static ChainPtr<ListNodeC> Create(initializer_list<int>& list) {
        ChainPtr<ListNodeC> head(MakeChain<ListNodeC>(0));
        ChainPtr<ListNodeC> next(head);
        for(auto& i : list){
            next->next = MakeChain<ListNodeC>(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, ChainPtr<ListNodeC>& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, ChainPtr<ListNodeC>&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    ~ListNodeC() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

struct ListNodeD : public GC_NS_QUALIFY(gc_cleanup) {
    using TPtr = ListNodeD*;
    static auto Alloc(int val) {
        return new ListNodeD(val);
    }
    static size_t cnsNum;
    static size_t cnsNumMax;
    int val;
    ListNodeD *next;
    
    // static void GC_CALLBACK finalize(void * obj) {
    //     // FINALIZER_LOCK();
    //     // FINALIZER_UNLOCK();
    // }

    // static void GC_CALLBACK finalizer(void * obj, void * client_data) {
    //     // FINALIZER_LOCK();
    //     // FINALIZER_UNLOCK();
    // }

    void *operator new(size_t size) {
        auto p = GC_malloc(size);
        return p;
    }
    ListNodeD(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        if(cnsNum >= cnsNumMax)
            cnsNumMax = cnsNum;
        #endif
    }
    static ListNodeD *Create(initializer_list<int>& list) {
        ListNodeD *head(new ListNodeD(0));
        ListNodeD *next(head);
        for(auto& i : list){
            next->next = new ListNodeD(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, ListNodeD *& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, ListNodeD *&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    ~ListNodeD() {
        #if HAVE_DES
        cnsNum --;
        #endif
    }
};

template<typename T>
class Solution {
    using TPtr = typename T::TPtr;
public:
    TPtr reverseKGroup(TPtr head, int k) {
        if(k <= 1){
            return head;
        }
        //带头指针
        TPtr oldhead(T::Alloc(0));
        oldhead->next = head;
        //算出长度
        int n = 0;
        for(TPtr i = oldhead; i->next != nullptr; i = i->next) {
            n++;
        }
        //新头指针
        TPtr newhead(T::Alloc(0));
        TPtr newtail = newhead;
        //转换
        for(int i = 0; i < n/k; i++) {
            TPtr temphead = newtail;
            newtail = oldhead->next;
            for(int j = 0; j < k && oldhead->next != nullptr; j++) {
                //head取第一个插入temphead的开头
                TPtr temp = oldhead->next;
                oldhead->next = temp->next;
                temp->next = temphead->next;
                temphead->next = temp;
            }
        }
        newtail->next = oldhead->next;
        return newhead->next;
    }
};

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

size_t ListNodeA::cnsNum = 0;
size_t ListNodeB::cnsNum = 0;
size_t ListNodeC::cnsNum = 0;
size_t ListNodeD::cnsNum = 0;

size_t ListNodeA::cnsNumMax = 0;
size_t ListNodeB::cnsNumMax = 0;
size_t ListNodeC::cnsNumMax = 0;
size_t ListNodeD::cnsNumMax = 0;


#define LOOPSIZE 20000
int main() {
    vector<tuple<initializer_list<int>, int>> exams = {
        {{1,2,3,4,5}, 2},
        {{1,2,3,4,5,6,7}, 3},
        {{1,2,3,4,5,6,7,8,9,10}, 4},
        {{1,2,3,4,5,6,7,8,9,10,11,12,13,14}, 5},
        {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19}, 6},
        {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25}, 7},
        {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}, 8},
    };
    cout << endl << "Press any key:";
    cin.get();
    cout << "Result" << endl;
    {
        for (auto &[list, k] : exams) {
            cout << "list: " << vector<int>(list) << endl << "k: "<< k << endl;
            cout << "1: " << Solution<ListNodeA>().reverseKGroup(ListNodeA::Create(list), k) << endl;
            cout << "2: " << Solution<ListNodeB>().reverseKGroup(ListNodeB::Create(list), k) << endl;
            cout << "3: " << Solution<ListNodeC>().reverseKGroup(ListNodeC::Create(list), k) << endl;
            cout << "4: " << Solution<ListNodeD>().reverseKGroup(ListNodeD::Create(list), k) << endl;
            cout << endl;
        }
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test1" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // ListNodeA::Create(list);
                Solution<ListNodeA>().reverseKGroup(ListNodeA::Create(list), k);
            }
        }
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << "ms" << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << "ms" << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << "ms" << endl; //输出：start()至调用此函数的系统时间
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test2" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // ListNodeB::Create(list);
                Solution<ListNodeB>().reverseKGroup(ListNodeB::Create(list), k);
            }
        }
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << "ms" << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << "ms" << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << "ms" << endl; //输出：start()至调用此函数的系统时间
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test3" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // ListNodeC::Create(list);
                Solution<ListNodeC>().reverseKGroup(ListNodeC::Create(list), k);
            }
        }
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << "ms" << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << "ms" << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << "ms" << endl; //输出：start()至调用此函数的系统时间
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test4" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        GC_INIT();
        // GC_set_await_finalize_proc(ListNodeD::finalize);
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // ListNodeD::Create(list);
                Solution<ListNodeD>().reverseKGroup(ListNodeD::Create(list), k);
            }
        }
        GC_gcollect();
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << "ms" << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << "ms" << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << "ms" << endl; //输出：start()至调用此函数的系统时间
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Construct Number:" << endl;
    {
        cout << "1: " << endl;
        cout << ListNodeA::cnsNum << endl;
        cout << ListNodeA::cnsNumMax << endl;
        cout << "2: " << endl;
        cout << ListNodeB::cnsNum << endl;
        cout << ListNodeB::cnsNumMax << endl;
        cout << "3: " << endl;
        cout << ListNodeC::cnsNum << endl;
        cout << ListNodeC::cnsNumMax << endl;
        cout << "4: " << endl;
        cout << ListNodeD::cnsNum << endl;
        cout << ListNodeD::cnsNumMax << endl;
        cout << endl;
    }
}

