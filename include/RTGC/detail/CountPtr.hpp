#ifndef RTGC_DETAIL_COUNTPTR_HPP
#define RTGC_DETAIL_COUNTPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class CountCore;

template<typename T>
class CountPtr {
    friend class CountCore<T>;
private:
    CountCore<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    
    //判断是否删除内层
    void DelIn() {
        if(innr != nullptr) {
            if(--innr->rc == 0)
                delete innr;
            innr = nullptr;
        }
    }
public:
    ~CountPtr() {
        if(innr != nullptr) {
            if(--innr->rc == 0)
                delete innr;
        }
    }
public:
    CountPtr() {}
    CountPtr(CountPtr<T> &o) {
        if(o.innr != nullptr) {
            innr->rc++;
            innr = o.innr;
        }
    }
    CountPtr(const CountPtr<T> &o) {
        if(o.innr != nullptr) {
            innr->rc++;
            innr = o.innr;
        }
    }
    CountPtr(CountCore<T> *i) {
        if(i != nullptr){
            innr->rc++;
            innr = i;
        }
    }

    CountPtr<T>& operator=(CountPtr<T> &o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr) {
                innr->rc++;
                innr = o.innr;
            }
        }
        return *this;
    }
    CountPtr<T>& operator=(CountPtr<T> &&o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr) {
                innr->rc++;
                innr = o.innr;
            }
        }
        return *this;
    }
    CountPtr<T>& operator=(std::nullptr_t) {
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
    operator CountPtr<_Tp>() {
        return *(CountPtr<_Tp>*)this;
    }

    friend bool operator==(const CountPtr<T> &a, const CountPtr<T> &b) {
        return a.innr == b.innr;
    }
    friend bool operator==(const CountPtr<T> &s, std::nullptr_t) {
        return s.innr == nullptr;
    }
    friend bool operator==(std::nullptr_t, const CountPtr<T> &s) {
        return s.innr == nullptr;
    }

    friend bool operator!=(const CountPtr<T> &a, const CountPtr<T> &b) {
        return a.innr != b.innr;
    }
    friend bool operator!=(const CountPtr<T> &s, std::nullptr_t) {
        return s.innr != nullptr;
    }
    friend bool operator!=(std::nullptr_t, const CountPtr<T> &s) {
        return s.innr != nullptr;
    }

	//仅可用于 == this
    // friend bool operator!=(const CountPtr<T> &a, const T *b) {
    //     return a.innr != (CountCore<T>*)b - CountCore<T>*;
    // }
    // friend bool operator!=(const T *b, const CountPtr<T> &a) {
    //     return a.innr != (CountCore<T>*)b;
    // }
    // friend bool operator==(const CountPtr<T> &a, const T *b) {
    //     return a.innr == (CountCore<T>*)b;
    // }
    // friend bool operator==(const T *b, const CountPtr<T> &a) {
    //     return a.innr == (CountCore<T>*)b;
    // }
};

template<typename>
struct isCountPtr : public std::false_type {};

template<typename V>
struct isCountPtr<CountPtr<V>> : public std::true_type {};


template<typename T, typename... _Args>
inline CountPtr<T> MakeCount(_Args&&... __args) {
    return CountPtr<T>(new CountCore<T>(std::forward<_Args>(__args)...));
}


}}

#endif
