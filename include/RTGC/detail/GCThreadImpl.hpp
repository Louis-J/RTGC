#ifndef RTGC_DETAIL_GCTHREADIMPL_HPP
#define RTGC_DETAIL_GCTHREADIMPL_HPP

#include<cstddef>
#include<atomic>
#include<thread>
#include<list>
#include<vector>
#include<condition_variable>

namespace RTGC { namespace detail {

template<typename T>
class ChainPtr;

class GCThread::GCNode {
    char node[sizeof(ChainPtr<int>)];
    void (*func)(void *, std::atomic<int>&);
public:
    template<typename T>
    inline ChainPtr<T>* thisP() {
        return (ChainPtr<T>*)node;
    }
    
    template<typename T>
    inline void Change(const ChainPtr<T> &cp) {
        cp.innr->outr = thisP<T>();

        thisP<T>()->invalid = cp.invalid;
        thisP<T>()->innr = cp.innr;
        if(cp.ipriv != &cp) {
            thisP<T>()->ipriv = cp.ipriv;
            thisP<T>()->inext = cp.inext;
            cp.ipriv->inext = thisP<T>();
            cp.inext->ipriv = thisP<T>();
            cp.ipriv = &cp;
            cp.inext = &cp;
        } else {
            thisP<T>()->inext = thisP<T>();
            thisP<T>()->ipriv = thisP<T>();
        }

        func = (void (*)(void*, std::atomic<int>&))ChainPtr<T>::DelFromOwner;
    }
    inline void Clear(std::atomic<int> &s) {
        func(node, s);
    }
};

template<typename T>
void GCThread::AddNode(const ChainPtr<T> &cp) {
    while(GlobalAtomic.test_and_set()); //得到GlobalAtomic
    auto empty = GlobalList.empty();

    GlobalList.emplace_back(GCNode());
    GlobalList.back().Change(cp);

    GlobalAtomic.clear();
    if(empty) {
        CountWake++;
        GlobalCV.notify_one();
    }
}

// void GCThread::Run() {
//     while(true) {
//         std::unique_lock<std::mutex> lk(GlobalMutex);
//         while(GlobalList.empty())
//             GlobalCV.wait(lk);

//         while(!GlobalList.empty()) {
//             auto &node = GlobalList.front();
//             node.Clear();
//             while(GlobalAtomic.test_and_set()); //得到GlobalAtomic
//             GlobalList.pop_front();
//             GlobalAtomic.clear();
//         }
//     }
// }

}}

#endif
