#ifndef RTGC_DETAIL_MTCHAINPTR2_HPP
#define RTGC_DETAIL_MTCHAINPTR2_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

// 前提: 同一个MTChainPtr2不会由多个线程同时拥有
// 假设: 拥有A B, 同时指向C，保证多线程安全, A拥有C的所有权
//       =>不存在A、B同时对C操作的情况(C保证线程安全，同一时刻仅由一个线程访问)
// 特点: 对指向同一个MTChainCore2的MTChainPtr2，同一时刻仅有一个线程可以释放
//       操作时，标记为invalid后释放所有权，防止死锁; 因此其他线程可以对invalid的节点进行操作
//       TryValidate会有逆向扫描，通过破坏"保持"条件解决死锁
// 场景: 1. A正在释放, 要转移至B; B正valid，未删除
//       2. B正在释放          ; A正valid，未删除
//       3. A准备释放, 要转移至B; B正在释放, 已invalid，暂未删除(Invalidate/TryValidate)
//       4. B准备释放          ; A正在释放, 已invalid，暂未删除(Invalidate)
//       5. B准备释放          ; A正在释放, 已invalid，暂未删除(TryValidate)

// 具体:
//   1. innr 控制指向head的所有权
//   2. 默认顺序为从前往后, 往前获取所有权失败时放弃自身的修改所有权并重新获取

template<typename T>
class MTChainCore2;

template<typename T>
class MTChainPtr2;

template<typename T>
class MTEdge {
public:
    size_t invalid = 0;// 0 为存活; 非0为将亡时删除的起始标记，用来区别不同线程; -1时为已死亡
    std::atomic_flag mOutr = ATOMIC_FLAG_INIT;
    mutable MTChainPtr2<T> *next = nullptr; //子层上一结点,下一结点
// public:
    ~MTEdge() {
        if(next != nullptr) {
            while(next->mOutr.test_and_set());
            if(next->invalid == -1)
                delete next;
            else
                next->mOutr.clear();
        }
    }
}

template<typename T>
class MTChainPtr2 {
    friend class MTChainCore2<T>;
private:
    MTChainCore2<T> *innr = nullptr; //指向的内节点，即指向的堆地址
    size_t invalid = 0;// 0 为存活; 非0为将亡时删除的起始标记，用来区别不同线程; -1时为已死亡
    std::atomic_flag mOutr = ATOMIC_FLAG_INIT;
    mutable MTChainPtr2<T> *next = nullptr; //子层上一结点,下一结点
    
    void DelIn() {
        if(edge == nullptr)
            return;
        while(edge->mOutr.test_and_set());
        if(innr->outr != this) {
            edge->invalid = -1;
            edge->mOutr.clear();
            innr = nullptr;
            edge = nullptr;
        } else {
            while(innr->mInnr.test_and_set()) {//未得到mInnr
                edge->mOutr.clear();
                while(edge->mOutr.test_and_set());
            }
            edge->mOutr.clear();
            innr->MTInvalidate((size_t)this);
            innr->MTTryValidate((size_t)this);
            // while(edge->mOutr.test_and_set());
            delete edge;
            if(innr->outr == this)
                delete innr;
            innr = nullptr;
            edge = nullptr;
        }
    }
public:
    ~MTChainPtr2() {
        if(edge == nullptr) //主动删
            return;
        while(edge->mOutr.test_and_set());
        if(edge->invalid) { //被动删
            delete edge;
            if(innr->outr == this)
                delete innr;
        } else if(innr->outr != this) { //主动删
            edge->invalid = -1;
            edge->mOutr.clear();
        } else {
            while(innr->mInnr.test_and_set()) { //未得到mInnr
                edge->mOutr.clear();
                while(edge->mOutr.test_and_set());
            } //得到内部的锁
            edge->mOutr.clear();
            innr->MTInvalidate((size_t)this);
            innr->MTTryValidate((size_t)this);
            // while(edge->mOutr.test_and_set());
            delete edge;
            if(innr->outr == this)
                delete innr;
        }
    }
public:
    MTChainPtr2() {}
    MTChainPtr2(const MTChainPtr2<T> &o) { //this与o应同一线程, 否则应先复制一个新指针, 再将新指针传递给所需线程
        if(o.innr != nullptr) {
            while(o.edge->mOutr.test_and_set());
            innr = o.innr;

            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
            o.mOutr.clear();
        }
    }
    MTChainPtr2(MTChainCore2<T> *i) {
        if(i != nullptr){
            innr = i;
            innr->outr = this;
        }
    }

    MTChainPtr2<T>& operator=(MTChainPtr2<T> &o) {
        if(innr != o.innr) {
            DelIn();
            while(o.mOutr.test_and_set());
            if(o.innr != nullptr){
                innr = o.innr;
                
                ipriv = &o;
                inext = o.inext;
                o.inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;
            }
            o.mOutr.clear();
        }
        return *this;
    }
    MTChainPtr2<T>& operator=(MTChainPtr2<T> &&o) {
        if(innr != o.innr) {
            DelIn();
            while(o.mOutr.test_and_set());
            if(o.innr != nullptr){
                innr = o.innr;
                
                ipriv = &o;
                inext = o.inext;
                o.inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;
            }
            o.mOutr.clear();
        }
        return *this;
    }
    MTChainPtr2<T>& operator=(std::nullptr_t) {
        DelIn();
        return *this;
    }

    void Invalidate() {//被动调用，此时this未上锁，innr已上锁，且innr需更新链
        invalid = true;
        if(innr != nullptr && innr->outr == this)
            innr->Invalidate();
    }
    void TryValidate(bool &pInvalid) {
        if(pInvalid) {
            if(innr != nullptr && innr->outr == this)
                innr->TryValidate();
        } else {
            this->invalid = false;
            if(innr != nullptr && innr->invalid)
                innr->TryValidate();
        }
    }

    T& operator*() {
        return innr->real;
    }
    T* operator->() {
        return &(innr->real);
    }
    
    //TODO: 避免多继承
    template<typename _Tp,
        typename = typename std::enable_if<std::is_base_of<_Tp, T>::value, int>::type>
    operator MTChainPtr2<_Tp>() {
        return *(MTChainPtr2<_Tp>*)this;
    }

    friend bool operator==(const MTChainPtr2<T> &a, const MTChainPtr2<T> &b) {
        return a.innr == b.innr;
    }
    friend bool operator==(const MTChainPtr2<T> &s, std::nullptr_t) {
        return s.innr == nullptr;
    }
    friend bool operator==(std::nullptr_t, const MTChainPtr2<T> &s) {
        return s.innr == nullptr;
    }

    friend bool operator!=(const MTChainPtr2<T> &a, const MTChainPtr2<T> &b) {
        return a.innr != b.innr;
    }
    friend bool operator!=(const MTChainPtr2<T> &s, std::nullptr_t) {
        return s.innr != nullptr;
    }
    friend bool operator!=(std::nullptr_t, const MTChainPtr2<T> &s) {
        return s.innr != nullptr;
    }

	//仅可用于 == this
    friend bool operator!=(const MTChainPtr2<T> &a, const T *b) {
        return a.innr != (MTChainCore2<T>*)b;
    }
    friend bool operator!=(const T *b, const MTChainPtr2<T> &a) {
        return a.innr != (MTChainCore2<T>*)b;
    }
    friend bool operator==(const MTChainPtr2<T> &a, const T *b) {
        return a.innr == (MTChainCore2<T>*)b;
    }
    friend bool operator==(const T *b, const MTChainPtr2<T> &a) {
        return a.innr == (MTChainCore2<T>*)b;
    }
};

template<typename>
struct isMTChainPtr2 : public std::false_type {};

template<typename V>
struct isMTChainPtr2<MTChainPtr2<V>> : public std::true_type {};


template<typename T, typename... _Args>
inline MTChainPtr2<T> MakeChain(_Args&&... __args) {
    return MTChainPtr2<T>(new MTChainCore2<T>(std::forward<_Args>(__args)...));
}


}}

#endif
