//LeetCode 0025
#include<iostream>
#include<vector>
#include<tuple>
#include<memory>
#include<RTGC/RTGC.hpp>
#include"memUse.hpp"
#include<boost/timer.hpp>
#include"gc_cpp.h"
#include"gc_allocator.h"
// #include "gc.h"
// #include "gc_disclaim.h"

#ifdef USE_TCMALLOC
#include"tcmalloc.h"
void * makeTCMallocUsed = tc_malloc(4);
#endif

using namespace std;

#define HAVE_CNS 1
#define HAVE_DES 1

struct ListNodeA {
    static size_t memUse;
    static size_t memUseMax;
    int val;
    ListNodeA *next;
    ListNodeA(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        memUse += sizeof(ListNodeA);
        if(memUse >= memUseMax)
            memUseMax = memUse;
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
    static size_t desNum;
    ~ListNodeA() {
        #if HAVE_DES
        desNum++;
        memUse -= sizeof(ListNodeA);
        #endif
    }
};



struct ListNodeB : public GC_NS_QUALIFY(gc_cleanup) {
    static size_t memUse;
    static size_t memUseMax;
    int val;
    ListNodeB *next;
    
    static void GC_CALLBACK finalize(void * obj) {
        // FINALIZER_LOCK();
        desNum++;
        memUse -= sizeof(ListNodeB);
        // FINALIZER_UNLOCK();
    }

    static void GC_CALLBACK finalizer(void * obj, void * client_data) {
        // FINALIZER_LOCK();
        desNum++;
        memUse -= sizeof(ListNodeB);
        // FINALIZER_UNLOCK();
    }

    void *operator new(size_t size) {
        auto p = GC_malloc(size);
        // GC_register_finalizer(p, ListNodeB::finalizer, nullptr, (GC_finalization_proc *)0, (void * *)0);
        return p;
    }
    ListNodeB(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        memUse += sizeof(ListNodeB);
        if(memUse >= memUseMax)
            memUseMax = memUse;
        #endif
    }
    static ListNodeB *Create(initializer_list<int>& list) {
        ListNodeB *head(new ListNodeB(0));
        ListNodeB *next(head);
        for(auto& i : list){
            next->next = new ListNodeB(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, ListNodeB *& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, ListNodeB *&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    static size_t desNum;
    ~ListNodeB() {
        // GC_FREE(this);
        #if HAVE_DES
        desNum++;
        memUse -= sizeof(ListNodeB);
        #endif
    }
};

struct ListNodeC {
    static size_t memUse;
    static size_t memUseMax;
    int val;
    shared_ptr<ListNodeC> next;
    ListNodeC(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        memUse += sizeof(ListNodeC);
        if(memUse >= memUseMax)
            memUseMax = memUse;
        #endif
    }
    static shared_ptr<ListNodeC> Create(initializer_list<int>& list) {
        shared_ptr<ListNodeC> head(new ListNodeC(0));
        shared_ptr<ListNodeC> next(head);
        for(auto& i : list){
            next->next.reset(new ListNodeC(i));
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, shared_ptr<ListNodeC>& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, shared_ptr<ListNodeC>&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    static size_t desNum;
    ~ListNodeC() {
        #if HAVE_DES
        desNum++;
        memUse -= sizeof(ListNodeC);
        #endif
    }
};

struct ListNodeD {
    static size_t memUse;
    static size_t memUseMax;
    CLASSLINK(ListNodeD, 2)
    int val;
    RTGC::ShellPtr<ListNodeD> next;
    ListNodeD(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        memUse += sizeof(ListNodeD);
        if(memUse >= memUseMax)
            memUseMax = memUse;
        #endif
    }
    static RTGC::ShellPtr<ListNodeD> Create(initializer_list<int>& list) {
        RTGC::ShellPtr<ListNodeD> head(new RTGC::CorePtr<ListNodeD>(0));
        RTGC::ShellPtr<ListNodeD> next(head);
        for(auto& i : list){
            next->next = new RTGC::CorePtr<ListNodeD>(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, RTGC::ShellPtr<ListNodeD>& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, RTGC::ShellPtr<ListNodeD>&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    static size_t desNum;
    ~ListNodeD() {
        #if HAVE_DES
        desNum++;
        memUse -= sizeof(ListNodeD);
        #endif
    }
};

template<typename TNode, typename TPtr>
class Solution {
public:
    TPtr reverseKGroup(TPtr head, int k) {
        if(k <= 1){
            return head;
        }
        //带头指针
        TPtr oldhead(new TNode(0));
        oldhead->next = head;
        //算出长度
        int n = 0;
        for(TPtr i = oldhead; i->next != nullptr; i = i->next) {
            n++;
        }
        //新头指针
        TPtr newhead(new TNode(0));
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

size_t ListNodeA::desNum = 0;
size_t ListNodeB::desNum = 0;
size_t ListNodeC::desNum = 0;
size_t ListNodeD::desNum = 0;

size_t ListNodeA::memUse = 0;
size_t ListNodeB::memUse = 0;
size_t ListNodeC::memUse = 0;
size_t ListNodeD::memUse = 0;

size_t ListNodeA::memUseMax = 0;
size_t ListNodeB::memUseMax = 0;
size_t ListNodeC::memUseMax = 0;
size_t ListNodeD::memUseMax = 0;


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
    // cout << endl << "Press any key:";
    // cin.get();
    // cout << "Result" << endl;
    // {
    //     for (auto &[list, k] : exams) {
    //         cout << "list: " << vector<int>(list) << endl << "k: "<< k << endl;
    //         cout << "1: " << Solution<ListNodeA, ListNodeA*>().reverseKGroup(ListNodeA::Create(list), k) << endl;
    //         cout << "2: " << Solution<ListNodeC, shared_ptr<ListNodeC>>().reverseKGroup(ListNodeC::Create(list), k) << endl;
    //         cout << "3: " << Solution<RTGC::CorePtr<ListNodeD>, RTGC::ShellPtr<ListNodeD>>().reverseKGroup(ListNodeD::Create(list), k) << endl;
    //         cout << endl;
    //     }
    // }
    // cout << endl << "Press any key:";
    // cin.get();
    // cout << "Test1" << endl;
    // {
    //     const size_t memUse = getCurrentRSS();
    //     const boost::timer timeBegin;
    //     for (int i = 0; i < LOOPSIZE; i++) {
    //         for (auto &[list, k] : exams) {
    //             // auto ret = ListNodeA::Create(list);
    //             auto ret = Solution<ListNodeA, ListNodeA*>().reverseKGroup(ListNodeA::Create(list), k);
    //         }
    //     }
    //     const uint32_t timeBy = timeBegin.elapsed() * 1000;
    //     cout << "time:" << timeBy << "ms" << endl;
    //     cout << "memory use:" << getCurrentRSS() - memUse << endl;
    // }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test2" << endl;
    {
        const size_t memUse = getCurrentRSS();
        const boost::timer timeBegin;
        GC_INIT();
        // GC_set_await_finalize_proc(ListNodeB::finalize);
        const uint32_t timeBy0 = timeBegin.elapsed() * 1000;
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // auto ret = ListNodeB::Create(list);
                auto ret = Solution<ListNodeB, ListNodeB*>().reverseKGroup(ListNodeB::Create(list), k);
            }
        }
        const uint32_t timeBy1 = timeBegin.elapsed() * 1000;
        GC_gcollect();
        const uint32_t timeBy2 = timeBegin.elapsed() * 1000;
        cout << "time0:" << timeBy0 << "ms" << endl;
        cout << "time1:" << timeBy1 << "ms" << endl;
        cout << "time2:" << timeBy2 << "ms" << endl;
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test3" << endl;
    {
        const size_t memUse = getCurrentRSS();
        const boost::timer timeBegin;
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // auto ret = ListNodeC::Create(list);
                auto ret = Solution<ListNodeC, shared_ptr<ListNodeC>>().reverseKGroup(ListNodeC::Create(list), k);
            }
        }
        const uint32_t timeBy = timeBegin.elapsed() * 1000;
        cout << "time:" << timeBy << "ms" << endl;
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test4" << endl;
    {
        const size_t memUse = getCurrentRSS();
        const boost::timer timeBegin;
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // auto ret = ListNodeD::Create(list);
                auto ret = Solution<RTGC::CorePtr<ListNodeD>, RTGC::ShellPtr<ListNodeD>>().reverseKGroup(ListNodeD::Create(list), k);
            }
        }
        const uint32_t timeBy = timeBegin.elapsed() * 1000;
        cout << "time:" << timeBy << "ms" << endl;
        cout << "memory use:" << getCurrentRSS() - memUse << endl;
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Destruct Number:" << endl;
    {
        cout << "1: " << endl;
        cout << ListNodeA::desNum << endl;
        cout << ListNodeA::memUse << endl;
        cout << ListNodeA::memUseMax << endl;
        cout << "2: " << endl;
        cout << ListNodeB::desNum << endl;
        cout << ListNodeB::memUse << endl;
        cout << ListNodeB::memUseMax << endl;
        cout << "3: " << endl;
        cout << ListNodeC::desNum << endl;
        cout << ListNodeC::memUse << endl;
        cout << ListNodeC::memUseMax << endl;
        cout << "4: " << endl;
        cout << ListNodeD::desNum << endl;
        cout << ListNodeD::memUse << endl;
        cout << ListNodeD::memUseMax << endl;
        cout << endl;
    }
}

