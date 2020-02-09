#ifndef RTGC_SNAPPTR_HPP
#define RTGC_SNAPPTR_HPP

#include"detail/ShellPtr.hpp"

namespace RTGC {

template<typename T>
class SnapPtr : public detail::ShellPtr<T> {
public:
    SnapPtr(SnapPtr<T> &o) : detail::ShellPtr<T>(nullptr, o) {}
    SnapPtr(SnapPtr<T> &&o) : detail::ShellPtr<T>(nullptr, move(o)) {}
    SnapPtr(detail::ShellPtr<T> &o) : detail::ShellPtr<T>(nullptr, o) {}
    SnapPtr(detail::ShellPtr<T> &&o) : detail::ShellPtr<T>(nullptr, move(o)) {}
    SnapPtr(detail::CorePtr<T> *i = nullptr) : detail::ShellPtr<T>(nullptr, i) {}
    SnapPtr<T>& operator=(SnapPtr<T> &o) {
        detail::ShellPtr<T>::assign(o);
        return *this;
    }
    SnapPtr<T>& operator=(SnapPtr<T> &&o) {
        detail::ShellPtr<T>::assign(o);
        return *this;
    }
    SnapPtr<T>& operator=(detail::CorePtr<T> *i) {
        detail::ShellPtr<T>::assign(i);
        return *this;
    }
};

}

#endif
