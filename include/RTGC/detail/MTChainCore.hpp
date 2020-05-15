#ifndef RTGC_DETAIL_MTCHAINCORE_HPP
#define RTGC_DETAIL_MTCHAINCORE_HPP

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

template<typename T>
class MTChainCore;

template<typename T>
class MTChainPtr {
    friend class MTChainCore<T>;
private:
    bool valid = true;
    std::atomic_flag mOutr = ATOMIC_FLAG_INIT;
    MTChainCore<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    MTChainPtr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
    void EraseMin() {
        if(ipriv != nullptr)
            ipriv->inext = inext;
        if(inext != nullptr)
            inext->ipriv = ipriv;
    }
    void Erase() {
        if(ipriv != nullptr)
            ipriv->inext = inext;
        if(inext != nullptr)
            inext->ipriv = ipriv;
        ipriv = nullptr;
        inext = nullptr;
    }
    //判断是否删除内层
    void DelIn() {
        if(innr != nullptr) {//主动删且有innr，此时this未上锁，innr未上锁
            if(innr->outr == this) {//有强引用innr，需更新链
                valid = false;
                innr->Invalidate();
                innr->TryValidate();
                Erase();
                if(innr->outr == this && !innr->valid) {//innr废弃
                    delete innr;
                }
                valid = true;
            } else {//无强引用innr
                Erase();
            }
            innr = nullptr;
        }
    }
    enum class MutexStatus: unsigned char {
        Fail,
        One,
        Two,
    };
    MutexStatus TryMutexDouble() {
        while(mOutr.test_and_set());
        if(innr == nullptr) {
            return MutexStatus::One;
        }
        if(innr->mInnr.test_and_set()) {//未得到mInnr
            mOutr.clear();
            return MutexStatus::Fail;
        } else {//得到mInnr
            return MutexStatus::Two;
        }
    }
    MutexStatus GetMutexDouble() {
        MutexStatus ret = TryMutexDouble();
        while(ret == MutexStatus::Fail) {
            ret = TryMutexDouble();
        }
        return ret;
    }
public:
    ~MTChainPtr() {
        if(GetMutexDouble() == MutexStatus::One) {//仅一个锁，说明指向nullptr，应链表、内部均为空
            mOutr.clear();
            return;
        }
        //两个锁，说明指向!nullptr，此时this上锁，innr上锁
        if(!valid) {//被动删
            EraseMin();
            if(innr != nullptr && innr->outr == this)
                delete innr;
        } else if(innr != nullptr) {//主动删且有innr，此时this未上锁，innr未上锁
            if(innr->outr == this) {//有强引用innr，需更新链
                valid = false;
                innr->Invalidate();
                innr->TryValidate();
                if(innr->outr == this && !innr->valid) {//innr废弃
                    EraseMin();
                    delete innr;
                } else {//innr更新
                    Erase();
                }
            } else {//无强引用innr
                EraseMin();
            }
        }
        //主动删且无innr
    }
public:
    MTChainPtr() {}
    MTChainPtr(MTChainPtr<T> &o) {
        if(o.innr != nullptr) {
            while(o.mOutr.test_and_set());
            innr = o.innr;

            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
            o.mOutr.clear();
        }
    }
    MTChainPtr(const MTChainPtr<T> &o) {
        if(o.innr != nullptr){
            while(o.mOutr.test_and_set());
            innr = o.innr;

            ipriv = const_cast<MTChainPtr<T>*>(&o);
            inext = o.inext;
            const_cast<MTChainPtr<T>*>(&o)->inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
            o.mOutr.clear();
        }
    }
    MTChainPtr(MTChainCore<T> *i) {
        if(i != nullptr){
            innr = i;
            innr->outr = this;
        }
    }

    MTChainPtr<T>& operator=(MTChainPtr<T> &o) {
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
    MTChainPtr<T>& operator=(MTChainPtr<T> &&o) {
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
    MTChainPtr<T>& operator=(std::nullptr_t) {
        DelIn();
        return *this;
    }

    void Invalidate() {//被动调用，此时this未上锁，innr已上锁，且innr需更新链
        valid = false;
        if(innr != nullptr && innr->outr == this)
            innr->Invalidate();
    }
    void TryValidate(bool &pValid) {
        if(!pValid) {
            if(innr != nullptr && innr->outr == this)
                innr->TryValidate();
        } else {
            this->valid = true;
            if(innr != nullptr && !innr->valid)
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
