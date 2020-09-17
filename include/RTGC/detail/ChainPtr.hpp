#ifndef RTGC_DETAIL_CHAINPTR_HPP
#define RTGC_DETAIL_CHAINPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class ChainCore;

template<typename T>
class ChainPtr {
    friend class ChainCore<T>;
private:
    bool invalid = false;
    ChainCore<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    mutable const ChainPtr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
    void EraseMin() {
        if(ipriv != nullptr)
            ipriv->inext = inext;
        if(inext != nullptr)
            inext->ipriv = ipriv;
    }
    //判断是否删除内层
    void DelIn() {
        if(innr != nullptr) {//主动删且有innr，此时this未上锁，innr未上锁
            if(innr->outr == this) {//有强引用innr，需更新链
                invalid = true;
                innr->Invalidate();
                innr->TryValidate();
                EraseMin();
                ipriv = nullptr;
                inext = nullptr;
                if(innr->outr == this && innr->invalid) {//innr废弃
                    delete innr;
                }
                invalid = false;
            } else {//无强引用innr
                EraseMin();
                ipriv = nullptr;
                inext = nullptr;
            }
            innr = nullptr;
        }
    }
public:
    void IN2() {
        if(!invalid && innr != nullptr && innr->outr == this) {
            invalid = true;
            innr->Invalidate();
        }
    }
    void TR2() {
        if(invalid) {
            innr->TryValidate();
        }
    }

    // ~ChainPtr() {
    //     if(invalid) {//被动删，此时this上锁，innr上锁
    //         if(ipriv != nullptr && !ipriv->invalid)
    //             ipriv->inext = inext;
    //         if(inext != nullptr && !inext->invalid)
    //             inext->ipriv = ipriv;
    //         if(innr != nullptr && innr->outr == this)
    //             delete innr;
    //     } else if(innr != nullptr) {//主动删且有innr，此时this未上锁，innr未上锁
    //         if(innr->outr == this) {//有强引用innr，需更新链
    //             invalid = true;
    //             innr->Invalidate();
    //             innr->TryValidate();
    //             if(innr->outr == this && innr->invalid) {//innr废弃
    //                 delete innr;
    //             } else {
    //                 if(ipriv != nullptr && !ipriv->invalid)
    //                     ipriv->inext = inext;
    //                 if(inext != nullptr && !inext->invalid)
    //                     inext->ipriv = ipriv;
    //             }
    //         } else {//无强引用innr
    //             if(ipriv != nullptr && !ipriv->invalid)
    //                 ipriv->inext = inext;
    //             if(inext != nullptr && !inext->invalid)
    //                 inext->ipriv = ipriv;
    //         }
    //     }
    //     //主动删且无innr
    // }
    ~ChainPtr() {
        if(invalid) {//被动删，此时this上锁，innr上锁
            EraseMin();
            if(innr != nullptr && innr->outr == this)
                delete innr;
        } else if(innr != nullptr) {//主动删且有innr，此时this未上锁，innr未上锁
            if(innr->outr == this) {//有强引用innr，需更新链
                invalid = true;
                innr->Invalidate();
                innr->TryValidate();
                EraseMin();
                if(innr->outr == this && innr->invalid) {//innr废弃
                    delete innr;
                }
            } else {//无强引用innr
                EraseMin();
            }
        }
        //主动删且无innr
    }
public:
    ChainPtr() {}
    // ChainPtr(ChainPtr<T> &o) {
    //     if(o.innr != nullptr) {
    //         innr = o.innr;

    //         ipriv = &o;
    //         inext = o.inext;
    //         o.inext = this;
    //         if(inext != nullptr)
    //             inext->ipriv = this;
    //     }
    // }
    // ChainPtr(const ChainPtr<T> &o) {
    //     if(o.innr != nullptr){
    //         innr = o.innr;

    //         ipriv = const_cast<ChainPtr<T>*>(&o);
    //         inext = o.inext;
    //         const_cast<ChainPtr<T>*>(&o)->inext = this;
    //         if(inext != nullptr)
    //             inext->ipriv = this;
    //     }
    // }
    ChainPtr(const ChainPtr<T> &o) {
        if(o.innr != nullptr) {
            innr = o.innr;

            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
    }
    ChainPtr(ChainCore<T> *i) {
        if(i != nullptr){
            innr = i;
            innr->outr = this;
        }
    }

    ChainPtr<T>& operator=(ChainPtr<T> &o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr){
                innr = o.innr;
                
                ipriv = &o;
                inext = o.inext;
                o.inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;
            }
        }
        return *this;
    }
    ChainPtr<T>& operator=(ChainPtr<T> &&o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr){
                innr = o.innr;
                
                ipriv = &o;
                inext = o.inext;
                o.inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;
            }
        }
        return *this;
    }
    ChainPtr<T>& operator=(std::nullptr_t) {
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

	//仅可用于 == this
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
