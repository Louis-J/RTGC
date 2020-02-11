#ifndef RTGC_DETAIL_SNAPPTR_HPP
#define RTGC_DETAIL_SNAPPTR_HPP

#include"ShellPtr.hpp"

namespace RTGC { namespace detail {

template<typename T>
class SnapPtr : public ShellPtr<T> {
public:
    SnapPtr(SnapPtr<T> &o) : ShellPtr<T>(nullptr, o) {}
    SnapPtr(SnapPtr<T> &&o) : ShellPtr<T>(nullptr, o) {}
    SnapPtr(ShellPtr<T> &o) : ShellPtr<T>(nullptr, o) {}
    SnapPtr(ShellPtr<T> &&o) : ShellPtr<T>(nullptr, o) {}
    SnapPtr(CorePtr<T> *i = nullptr) : ShellPtr<T>(nullptr, i) {}
    SnapPtr<T>& operator=(SnapPtr<T> &o) {
        ShellPtr<T>::assign(o);
        return *this;
    }
    SnapPtr<T>& operator=(SnapPtr<T> &&o) {
        ShellPtr<T>::assign(o);
        return *this;
    }
    SnapPtr<T>& operator=(CorePtr<T> *i) {
        ShellPtr<T>::assign(i);
        return *this;
    }
};

}}

#endif
