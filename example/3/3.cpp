//LeetCode 0025
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

// 稍复杂的示意, 以LeetCode 0025展示了不同指针下的链表操作的性能

#define HAVE_CNS 1
#define HAVE_DES 0

struct ListNodeA {
    using TPtr = ListNodeA*;
    static auto Alloc(int val) {
        return new ListNodeA(val);
    }

    static size_t cnsNum;
    int val;
    ListNodeA *next;
    ListNodeA(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
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
        if(next)
            delete next;
    }
};

struct ListNodeB {
    using TPtr = shared_ptr<ListNodeB>;
    static auto Alloc(int val) {
        return make_shared<ListNodeB>(val);
    }

    static size_t cnsNum;
    int val;
    shared_ptr<ListNodeB> next;
    ListNodeB(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
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
    constexpr static bool RTGC_MayCirRef = true;
    RTGC_AutoChainLink(ListNodeC, 2);
    RTGC_AutoCRDetectIn(ListNodeC, 2);
    
    using TPtr = SmarterPtr<ListNodeC>;
    static auto Alloc(int val) {
        return MakeChain<ListNodeC>(val);
    }

    static size_t cnsNum;
    int val;
    SmarterPtr<ListNodeC> next;
    ListNodeC(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    static SmarterPtr<ListNodeC> Create(initializer_list<int>& list) {
        SmarterPtr<ListNodeC> head(MakeChain<ListNodeC>(0));
        SmarterPtr<ListNodeC> next(head);
        for(auto& i : list){
            next->next = MakeChain<ListNodeC>(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, SmarterPtr<ListNodeC>& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, SmarterPtr<ListNodeC>&& l) {
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
RTGC_AutoCRDetectOut(ListNodeC, ListNodeC::RTGC_MayCirRef);

struct ListNodeD {
    using TPtr = CountPtr<ListNodeD>;
    static auto Alloc(int val) {
        return MakeCount<ListNodeD>(val);
    }

    static size_t cnsNum;
    int val;
    CountPtr<ListNodeD> next;
    ListNodeD(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    static CountPtr<ListNodeD> Create(initializer_list<int>& list) {
        CountPtr<ListNodeD> head(MakeCount<ListNodeD>(0));
        CountPtr<ListNodeD> next(head);
        for(auto& i : list){
            next->next = MakeCount<ListNodeD>(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, CountPtr<ListNodeD>& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, CountPtr<ListNodeD>&& l) {
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

struct ListNodeE {
    using TPtr = MTCountPtr<ListNodeE>;
    static auto Alloc(int val) {
        return MakeMTCount<ListNodeE>(val);
    }

    static size_t cnsNum;
    int val;
    MTCountPtr<ListNodeE> next;
    ListNodeE(int x) : val(x), next(NULL) {
        #if HAVE_CNS
        cnsNum ++;
        #endif
    }
    static MTCountPtr<ListNodeE> Create(initializer_list<int>& list) {
        MTCountPtr<ListNodeE> head(MakeMTCount<ListNodeE>(0));
        MTCountPtr<ListNodeE> next(head);
        for(auto& i : list) {
            next->next = MakeMTCount<ListNodeE>(i);
            next = next->next;
        }
        return head->next;
    }
    friend ostream& operator<<(ostream& ostr, MTCountPtr<ListNodeE>& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    friend ostream& operator<<(ostream& ostr, MTCountPtr<ListNodeE>&& l) {
        ostr << l->val;
        if(l->next != nullptr)
            ostr << "->" << l->next;
        return ostr;
    }
    ~ListNodeE() {
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
size_t ListNodeE::cnsNum = 0;


#define LOOPSIZE 200000
int main() {
    vector<pair<initializer_list<int>, int>> exams = {
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
            cout << "5: " << Solution<ListNodeE>().reverseKGroup(ListNodeE::Create(list), k) << endl;
            cout << endl;
        }
    }
    cout << endl << "Press any key:";
    cin.get();
    cout << "Test1: ListNodeA*" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // ListNodeA::Create(list);
                delete Solution<ListNodeA>().reverseKGroup(ListNodeA::Create(list), k);
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
    cout << "Test2: shared_ptr<ListNodeB>" << endl;
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
    cout << "Test3: ChainPtr<ListNodeC>" << endl;
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
    cout << "Test4: CountPtr<ListNodeD>" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // ListNodeD::Create(list);
                Solution<ListNodeD>().reverseKGroup(ListNodeD::Create(list), k);
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
    cout << "Test5: MTCountPtr<ListNodeE>" << endl;
    {
        const size_t memUse = getCurrentRSS();
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LOOPSIZE; i++) {
            for (auto &[list, k] : exams) {
                // ListNodeE::Create(list);
                Solution<ListNodeE>().reverseKGroup(ListNodeE::Create(list), k);
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
    cout << "Construct Number:" << endl;
    {
        cout << "1: " << endl;
        cout << ListNodeA::cnsNum << endl;
        cout << "2: " << endl;
        cout << ListNodeB::cnsNum << endl;
        cout << "3: " << endl;
        cout << ListNodeC::cnsNum << endl;
        cout << "4: " << endl;
        cout << ListNodeD::cnsNum << endl;
        cout << "5: " << endl;
        cout << ListNodeE::cnsNum << endl;
        cout << endl;
    }
}

