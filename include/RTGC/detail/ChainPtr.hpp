#ifndef RTGC_DETAIL_CHAINPTR_HPP
#define RTGC_DETAIL_CHAINPTR_HPP

#include<cstddef>
#include<atomic>
#include<iostream>

namespace RTGC { namespace detail {

// 前提: 同一个ChainPtr不会由多个线程同时拥有
// 假设: 拥有A B, 同时指向C，保证多线程安全, A拥有C的所有权
//       =>不存在A、B同时对C操作的情况(C保证线程安全，同一时刻仅由一个线程访问)
// 特点: 对指向同一个ChainCore的ChainPtr，同一时刻仅有一个线程可以释放
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
class ChainCore;

template<typename T>
class ChainPtr {
    friend class ChainCore<T>;

    template<typename Tp, typename... _Args>
    friend ChainPtr<Tp> MakeChain(_Args&&... __args);

    friend class GCThread::GCNode;
public:
// private:
    bool invalid = false;
    ChainCore<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    mutable const ChainPtr<T> *ipriv = this, *inext = this;//子层上一结点,下一结点
    
    void EraseMin() {
        ipriv->inext = inext;
        inext->ipriv = ipriv;
    }

    void Erase() {
        if(ipriv != this)
            EraseMin();
    }

    void EraseMax() {
        if(ipriv != this) {
            EraseMin();
            ipriv = this;
            inext = this;
        }
    }

    //判断是否删除内层
    void DelIn() {
        if(innr != nullptr) { // 主动删且有innr，此时this未上锁，innr未上锁
            innr->LockAtomic();
            if(innr->outr == this) // 有强引用innr，需交给GC线程更新链
                GCThread::AddNode(*this);
            Erase();
            innr->UnlockAtomic();
            if(ipriv != this) {
                ipriv = this;
                inext = this;
            }
            innr = nullptr;
        }
    }

public: // 核心算法: 如何去初步标记/尝试连接/最终清扫
    // 由GC调用
    static void DelFromOwner(ChainPtr<T> *cp, std::atomic<int> &s) {
        cp->invalid = true;
        s.store(10);
        cp->innr->Invalidate();
        s.store(20);
        for (int cnt = 0; cp->innr->mInnr.test_and_set(); ++cnt) {
            if(cnt >= 10000) {
                std::cout << "BAD 20 !!!\n";
                break;
            }
        }
        cp->innr->UnlockAtomic();
        
        cp->innr->TryValidate();
        s.store(30);
        cp->innr->LockAtomic();
        cp->EraseMax();
        cp->innr->UnlockAtomic();
        if(cp->innr->outr == cp && cp->innr->invalid) {//innr废弃
            delete cp->innr;
        }
        s.store(40);
    }

    // 由GC调用
    static void ReLinkInnr(ChainPtr<T> *outr, ChainCore<T> *innr) {
        innr->Invalidate();
        innr->TryValidate();
        if(innr->outr == outr)
            delete innr;
    }

    // 由GC调用
    void Invalidate() {
        invalid = true;
        if(innr != nullptr) {
            innr->LockAtomic();
            if(innr->outr == this) {
                innr->UnlockAtomic();
                innr->Invalidate();
            } else {
                // 放到循环链表末尾
                auto &oend = innr->outr->ipriv;
                if(oend != this) {
                    ipriv->inext = inext;
                    inext->ipriv = ipriv;
                    inext = innr->outr;
                    ipriv = oend;
                    oend->inext = this;
                    oend = this;
                }
                innr->UnlockAtomic();
            }
        }
    }

    // 由GC调用
    void TryValidate(bool &pInvalid) {
        if(pInvalid) {
            if(innr != nullptr) {
                innr->LockAtomic();
                if(innr->outr == this) {
                    innr->UnlockAtomic();
                    innr->TryValidate();
                } else
                    innr->UnlockAtomic();
            }
        } else {
            this->invalid = false;
            if(innr != nullptr) {
                innr->LockAtomic();
                if(innr->outr == this) {
                    innr->UnlockAtomic();
                    innr->TryValidate();
                } else if(innr->invalid) {
                    auto &ioutr = innr->outr;
                    if(ioutr->inext->invalid == true) {
                        // 放到循环链表开头
                        ipriv->inext = inext;
                        inext->ipriv = ipriv;
                        inext = ioutr->inext;
                        ipriv = ioutr;
                        ioutr->inext = this;
                        ioutr = this;
                    }
                    innr->UnlockAtomic();
                    innr->TryValidate();
                } else
                    innr->UnlockAtomic();
            }
        }
    }

    ~ChainPtr() {
        if(invalid) { // 被动删, 由GC调用
            Erase();
            if(innr != nullptr && innr->outr == this)
                delete innr;
        } else if(innr != nullptr) { // 主动删且有innr，此时this未上锁，innr未上锁
            innr->LockAtomic(); //得到mInnr
            if(innr->outr == this) { // 有强引用innr，需交给GC线程更新链
                GCThread::AddNode(*this);
            } else { // 无强引用innr, 不需要交给GC线程
                Erase();
            }
            innr->UnlockAtomic();
        }
        //主动删且无innr
    }
private:
    ChainPtr(ChainCore<T> *i) {
        if(i != nullptr){
            innr = i;
            innr->outr = this;
        }
    }

public:
    ChainPtr() {}
    ChainPtr(std::nullptr_t) {}

    ChainPtr(const ChainPtr<T> &o) {
        if(o.innr != nullptr) {
            o.innr->UnlockAtomic();
            innr = o.innr;
            auto &oend = innr->outr->ipriv;
            ipriv = oend;
            inext = oend->inext;
            oend->inext = this;
            oend = this;

            innr->UnlockAtomic();
        }
    }

    ChainPtr<T>& operator=(ChainPtr<T> &o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr){
                o.innr->UnlockAtomic();
                innr = o.innr;
                auto &oend = innr->outr->ipriv;
                ipriv = oend;
                inext = oend->inext;
                oend->inext = this;
                oend = this;

                innr->UnlockAtomic();
            }
        }
        return *this;
    }
    ChainPtr<T>& operator=(ChainPtr<T> &&o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr){
                o.innr->UnlockAtomic();
                innr = o.innr;
                auto &oend = innr->outr->ipriv;
                ipriv = oend;
                inext = oend->inext;
                oend->inext = this;
                oend = this;

                innr->UnlockAtomic();
            }
        }
        return *this;
    }
    ChainPtr<T>& operator=(std::nullptr_t) {
        DelIn();
        return *this;
    }

    T& operator*() {
        return innr->real;
    }
    T* operator->() {
        return &(innr->real);
    }
    
    const T& operator*() const {
        return innr->real;
    }
    const T* operator->() const {
        return &(innr->real);
    }
    
    //TODO: 避免多继承
    template<typename _Tp,
        typename = typename std::enable_if<std::is_base_of<_Tp, T>::value, int>::type>
    operator ChainPtr<_Tp>() {
        return *(ChainPtr<_Tp>*)this;
    }

    friend bool operator==(const ChainPtr<T> &a, const ChainPtr<T> &b) {
        return a.innr == b.innr;
    }
    friend bool operator==(const ChainPtr<T> &s, std::nullptr_t) {
        return s.innr == nullptr;
    }
    friend bool operator==(std::nullptr_t, const ChainPtr<T> &s) {
        return s.innr == nullptr;
    }

    friend bool operator!=(const ChainPtr<T> &a, const ChainPtr<T> &b) {
        return a.innr != b.innr;
    }
    friend bool operator!=(const ChainPtr<T> &s, std::nullptr_t) {
        return s.innr != nullptr;
    }
    friend bool operator!=(std::nullptr_t, const ChainPtr<T> &s) {
        return s.innr != nullptr;
    }

    // 仅可用于 == this
    friend bool operator!=(const ChainPtr<T> &a, const T *b) {
        return a.innr != (ChainCore<T>*)b;
    }
    friend bool operator!=(const T *b, const ChainPtr<T> &a) {
        return a.innr != (ChainCore<T>*)b;
    }
    friend bool operator==(const ChainPtr<T> &a, const T *b) {
        return a.innr == (ChainCore<T>*)b;
    }
    friend bool operator==(const T *b, const ChainPtr<T> &a) {
        return a.innr == (ChainCore<T>*)b;
    }
};

template<typename>
struct isChainPtr : public std::false_type {};

template<typename V>
struct isChainPtr<ChainPtr<V>> : public std::true_type {
    using type = V;
};

template<typename T, typename... _Args>
inline ChainPtr<T> MakeChain(_Args&&... __args) {
    return ChainPtr<T>(new ChainCore<T>(std::forward<_Args>(__args)...));
}


}}

#endif
