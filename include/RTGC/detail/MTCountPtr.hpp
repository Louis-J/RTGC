#ifndef RTGC_DETAIL_MTCOUNTPTR_HPP
#define RTGC_DETAIL_MTCOUNTPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class MTCountCore;

template<typename T>
class MTCountPtr {
    friend class MTCountCore<T>;
private:
    MTCountCore<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    
    //判断是否删除内层
    void DelIn() {
        if(innr != nullptr) {
            if(--innr->rc == 0)
                delete innr;
            innr = nullptr;
        }
    }
public:
    ~MTCountPtr() {
        if(innr != nullptr) {
            if(--innr->rc == 0)
                delete innr;
        }
    }
public:
    MTCountPtr() {}
    MTCountPtr(MTCountPtr<T> &o) {
        if(o.innr != nullptr) {
            innr->rc++;
            innr = o.innr;
        }
    }
    MTCountPtr(const MTCountPtr<T> &o) {
        if(o.innr != nullptr) {
            innr->rc++;
            innr = o.innr;
        }
    }
    MTCountPtr(MTCountCore<T> *i) {
        if(i != nullptr){
            innr->rc++;
            innr = i;
        }
    }

    MTCountPtr<T>& operator=(MTCountPtr<T> &o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr) {
                innr->rc++;
                innr = o.innr;
            }
        }
        return *this;
    }
    MTCountPtr<T>& operator=(MTCountPtr<T> &&o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr) {
                innr->rc++;
                innr = o.innr;
            }
        }
        return *this;
    }
    MTCountPtr<T>& operator=(std::nullptr_t) {
        DelIn();
        return *this;
    }

    T& operator*() {
        return innr->real;
    }
    T* operator->() {
        return &(innr->real);
    }
    
    //TODO: 避免多继承
    template<typename _Tp,
        typename = typename std::enable_if<std::is_base_of<_Tp, T>::value>::type>
    operator MTCountPtr<_Tp>() {
        return *(MTCountPtr<_Tp>*)this;
    }

    friend bool operator==(const MTCountPtr<T> &a, const MTCountPtr<T> &b) {
        return a.innr == b.innr;
    }
    friend bool operator==(const MTCountPtr<T> &s, std::nullptr_t) {
        return s.innr == nullptr;
    }
    friend bool operator==(std::nullptr_t, const MTCountPtr<T> &s) {
        return s.innr == nullptr;
    }

    friend bool operator!=(const MTCountPtr<T> &a, const MTCountPtr<T> &b) {
        return a.innr != b.innr;
    }
    friend bool operator!=(const MTCountPtr<T> &s, std::nullptr_t) {
        return s.innr != nullptr;
    }
    friend bool operator!=(std::nullptr_t, const MTCountPtr<T> &s) {
        return s.innr != nullptr;
    }

	//仅可用于 == this
    // friend bool operator!=(const MTCountPtr<T> &a, const T *b) {
    //     return a.innr != (MTCountCore<T>*)b - MTCountCore<T>*;
    // }
    // friend bool operator!=(const T *b, const MTCountPtr<T> &a) {
    //     return a.innr != (MTCountCore<T>*)b;
    // }
    // friend bool operator==(const MTCountPtr<T> &a, const T *b) {
    //     return a.innr == (MTCountCore<T>*)b;
    // }
    // friend bool operator==(const T *b, const MTCountPtr<T> &a) {
    //     return a.innr == (MTCountCore<T>*)b;
    // }
};

template<typename>
struct isMTCountPtr : public std::false_type {};

template<typename V>
struct isMTCountPtr<MTCountPtr<V>> : public std::true_type {};


template<typename T, typename... _Args>
inline MTCountPtr<T> MakeMTCount(_Args&&... __args) {
    return MTCountPtr<T>(new MTCountCore<T>(std::forward<_Args>(__args)...));
}


}}

#endif
