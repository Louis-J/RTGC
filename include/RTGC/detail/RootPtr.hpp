#ifndef RTGC_DETAIL_ROOTPTR_HPP
#define RTGC_DETAIL_ROOTPTR_HPP

#include"ShellPtr.hpp"

namespace RTGC { namespace detail {

template<typename T>
class RootPtr : public ShellPtr<T> {
public:
    RootPtr(RootPtr<T> &o) : ShellPtr<T>(this, o) {}
    RootPtr(RootPtr<T> &&o) : ShellPtr<T>(this, o) {}
    RootPtr(ShellPtr<T> &o) : ShellPtr<T>(this, o) {}
    RootPtr(ShellPtr<T> &&o) : ShellPtr<T>(this, o) {}
    RootPtr(CorePtr<T> *i = nullptr) : ShellPtr<T>(this, i) {}
    RootPtr<T>& operator=(RootPtr<T> &o) {
        ShellPtr<T>::assign(o);
        return *this;
    }
    RootPtr<T>& operator=(RootPtr<T> &&o) {
        ShellPtr<T>::assign(o);
        return *this;
    }
    RootPtr<T>& operator=(CorePtr<T> *i) {
        ShellPtr<T>::assign(i);
        return *this;
    }
};

template<typename>
struct isRootPtr : public std::false_type {};

template<typename V>
struct isRootPtr<RootPtr<V>> : public std::true_type {};

}}

#endif
