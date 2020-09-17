#ifndef RTGC_DETAIL_MTCHAINPTR_HPP
#define RTGC_DETAIL_MTCHAINPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

// 前提: 同一个MTChainPtr不会由多个线程同时拥有
// 假设: 拥有A B, 同时指向C，保证多线程安全, A拥有C的所有权
//       =>不存在A、B同时对C操作的情况(C保证线程安全，同一时刻仅由一个线程访问)
// 特点: 对指向同一个MTChainCore的MTChainPtr，同一时刻仅有一个线程可以释放
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
class MTChainCore;

template<typename T>
class MTChainPtr {
    friend class MTChainCore<T>;
private:
    bool invalid = false;
    std::atomic_flag mOutr = ATOMIC_FLAG_INIT;
    MTChainCore<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    mutable MTChainPtr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
    void DelIn() {
        if(innr == nullptr)
            return;
        while(mOutr.test_and_set());
        while(ipriv != nullptr && ipriv->mOutr.test_and_set()) {
            mOutr.clear();
            while(mOutr.test_and_set());
        }
        if(ipriv != nullptr) { //获取到两个锁，说明并非强引用
            while(inext != nullptr && inext->mOutr.test_and_set());
            ipriv->inext = inext;
            if(inext != nullptr) {
                inext->ipriv = ipriv;
                inext->mOutr.clear();
            }
            ipriv->mOutr.clear();
            mOutr.clear();
            innr = nullptr;
            ipriv = nullptr;
            inext = nullptr;
        } else { //获取到一个锁，说明是强引用
            while(innr->mInnr.test_and_set()); //得到mInnr
            mOutr.clear();
            innr->MTInvalidate((size_t)this);
            innr->MTTryValidate((size_t)this);
            if(innr->outr == this)
                delete innr;
            innr = nullptr;
            ipriv = nullptr;
            inext = nullptr;
        }
    }
public:
    ~MTChainPtr() {
        if(innr == nullptr) //主动删
            return;
        while(mOutr.test_and_set());
        while(ipriv != nullptr && ipriv->mOutr.test_and_set()) {
            mOutr.clear();
            while(mOutr.test_and_set());
        }
        if(ipriv != nullptr) { //获取到两个锁，说明并非强引用
            if(invalid) { //弱引用被动删
                while(inext != nullptr && inext->mOutr.test_and_set());
                ipriv->inext = inext;
                if(inext != nullptr) {
                    inext->ipriv = ipriv;
                    inext->mOutr.clear();
                }
                mOutr.clear();
                ipriv->mOutr.clear();
            } else { //弱引用主动删
                while(inext != nullptr && inext->mOutr.test_and_set());
                ipriv->inext = inext;
                if(inext != nullptr) {
                    inext->ipriv = ipriv;
                    inext->mOutr.clear();
                }
                mOutr.clear();
                ipriv->mOutr.clear();
            }
        } else { //获取到一个锁，说明是被动删, 或强引用主动删
            if(invalid) { //强引用或弱引用被动删
                while(inext != nullptr && inext->mOutr.test_and_set());
                if(inext != nullptr) {
                    inext->ipriv = ipriv;
                    inext->mOutr.clear();
                }
                // mOutr.clear();
                if(innr->outr == this)
                    delete innr;
            } else { //强引用或弱引用被动删
                // while(innr->mInnr.test_and_set()); //得到mInnr控制权
                innr->MTLockSub(); //得到mInnr控制权
                mOutr.clear();
                innr->MTInvalidate((size_t)this);

                while(mOutr.test_and_set());
                innr->MTTryValidate();
                if(innr->outr == this)
                    delete innr;
            }
        }
    }
public:
    MTChainPtr() {}
    MTChainPtr(const MTChainPtr<T> &o) {
        if(o.innr != nullptr) {
            //尝试获取o、o.next两个锁
            while(o.mOutr.test_and_set());
            innr = o.innr;
            ipriv = const_cast<MTChainPtr<T>*>(&o);
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr) {
                while(inext->mOutr.test_and_set());
                inext->ipriv = this;
                inext->mOutr.clear();
            }
            o.mOutr.clear();
        }
    }
    MTChainPtr(MTChainCore<T> *i) { //仅出现在构造阶段，无关多线程
        if(i != nullptr){
            innr = i;
            innr->outr = this;
        }
    }

    MTChainPtr<T>& operator=(MTChainPtr<T> &o) { //用户需保证o指向的值不应变化(可使用A = MTChainPtr(B)代替 A = B)
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr){
                while(o.mOutr.test_and_set());
                innr = o.innr;
                
                ipriv = &o;
                inext = o.inext;
                o.inext = this;
                if(inext != nullptr) {
                    while(inext->mOutr.test_and_set());
                    inext->ipriv = this;
                    inext->mOutr.clear();
                }
                o.mOutr.clear();
            }
        }
        return *this;
    }
    MTChainPtr<T>& operator=(MTChainPtr<T> &&o) { //用户需保证o为将亡量(操作会更改o本身)
        // std::swap(*this, o);
        return *this;
    }
    MTChainPtr<T>& operator=(std::nullptr_t) {
        DelIn();
        return *this;
    }

    void MTLockSub() {//被动调用，此时this未上锁
        while(mOutr.test_and_set());
    }
    void MTInvalidate(size_t tag) {//被动调用，此时this已上锁
        invalid = tag;
        if(innr != nullptr && innr->outr == this) {
            innr->MTLockSub();
            mOutr.clear();
            innr->MTInvalidate();
        }
    }
    void MTTryValidate(size_t tag, size_t &pInvalid) {
        if(pInvalid == 0) { //已存活
            this->invalid = 0;
            if(innr != nullptr && innr->invalid == tag) {
                innr->MTTryValidate();
            }
        } else if(pInvalid != invalid) { //连接到其他线程
            this->invalid = pInvalid;
            if(innr != nullptr && innr->invalid == tag) {
                innr->MTTryValidate();
            }
        } else { //重连接失败
            if(innr != nullptr && innr->outr == this) {
                innr->MTTryValidate();
            }
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
    operator MTChainPtr<_Tp>() {
        return *(MTChainPtr<_Tp>*)this;
    }

    friend bool operator==(const MTChainPtr<T> &a, const MTChainPtr<T> &b) {
        return a.innr == b.innr;
    }
    friend bool operator==(const MTChainPtr<T> &s, std::nullptr_t) {
        return s.innr == nullptr;
    }
    friend bool operator==(std::nullptr_t, const MTChainPtr<T> &s) {
        return s.innr == nullptr;
    }

    friend bool operator!=(const MTChainPtr<T> &a, const MTChainPtr<T> &b) {
        return a.innr != b.innr;
    }
    friend bool operator!=(const MTChainPtr<T> &s, std::nullptr_t) {
        return s.innr != nullptr;
    }
    friend bool operator!=(std::nullptr_t, const MTChainPtr<T> &s) {
        return s.innr != nullptr;
    }

	//仅可用于 == this
    friend bool operator!=(const MTChainPtr<T> &a, const T *b) {
        return a.innr != (MTChainCore<T>*)b;
    }
    friend bool operator!=(const T *b, const MTChainPtr<T> &a) {
        return a.innr != (MTChainCore<T>*)b;
    }
    friend bool operator==(const MTChainPtr<T> &a, const T *b) {
        return a.innr == (MTChainCore<T>*)b;
    }
    friend bool operator==(const T *b, const MTChainPtr<T> &a) {
        return a.innr == (MTChainCore<T>*)b;
    }
};

template<typename>
struct isMTChainPtr : public std::false_type {};

template<typename V>
struct isMTChainPtr<MTChainPtr<V>> : public std::true_type {};


template<typename T, typename... _Args>
inline MTChainPtr<T> MakeChain(_Args&&... __args) {
    return MTChainPtr<T>(new MTChainCore<T>(std::forward<_Args>(__args)...));
}


}}

#endif
