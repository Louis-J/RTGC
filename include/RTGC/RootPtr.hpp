#ifndef RTGC_ROOTPTR_HPP
#define RTGC_ROOTPTR_HPP

#include"detail/ShellPtr.hpp"

namespace RTGC {

template<typename T>
class RootPtr : public detail::ShellPtr<T> {
public:
    RootPtr(RootPtr<T> &o) : detail::ShellPtr<T>(this, o) {}
    RootPtr(RootPtr<T> &&o) : detail::ShellPtr<T>(this, move(o)) {}
    RootPtr(detail::ShellPtr<T> &o) : detail::ShellPtr<T>(this, o) {}
    RootPtr(detail::ShellPtr<T> &&o) : detail::ShellPtr<T>(this, move(o)) {}
    RootPtr(detail::CorePtr<T> *i = nullptr) : detail::ShellPtr<T>(this, i) {}
    RootPtr<T>& operator=(RootPtr<T> &o) {
        detail::ShellPtr<T>::assign(o);
        return *this;
    }
    RootPtr<T>& operator=(RootPtr<T> &&o) {
        detail::ShellPtr<T>::assign(o);
        return *this;
    }
    RootPtr<T>& operator=(detail::CorePtr<T> *i) {
        detail::ShellPtr<T>::assign(i);
        return *this;
    }
};

template<typename>
struct isRootPtr : public std::false_type {};

template<typename V>
struct isRootPtr<RootPtr<V>> : public std::true_type {};

}

#endif
