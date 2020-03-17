#ifndef RTGC_DETAIL_SHELLPTR_HPP
#define RTGC_DETAIL_SHELLPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class CorePtr;

template<typename T>
class ShellPtr {
    friend class CorePtr<T>;
private:
    bool valid = true;
    std::atomic_flag mut = ATOMIC_FLAG_INIT;
    CorePtr<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    ShellPtr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
    void Erase() {
        if(ipriv != nullptr) {
            ipriv->inext = inext;
        }
        if(inext != nullptr) {
            inext->ipriv = ipriv;
        }
        ipriv = nullptr;
        inext = nullptr;
    }
    //判断是否删除内层
    void DelIn() {
        if(valid) {
            Invalidate();
            if(innr != nullptr) {
                if(innr->outr == this) {
                    innr->TryValidate();
                    Erase();
                    if(innr->outr == this && !innr->valid)
                        delete innr;
                    innr = nullptr;
                } else {
                    Erase();
                    innr = nullptr;
                }
            }
            valid = true;
        } else {
            Erase();
            if(innr != nullptr && innr->outr == this)
                delete innr;
            innr = nullptr;
            valid = true;
        }
    }
public:
    ShellPtr() {}
    ShellPtr(ShellPtr<T> &o) {
        innr = o.innr;
        if(innr != nullptr){
            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
    }
    ShellPtr(const ShellPtr<T> &o) {
        innr = o.innr;
        if(innr != nullptr){
            ipriv = const_cast<ShellPtr<T>*>(&o);
            inext = o.inext;
            const_cast<ShellPtr<T>*>(&o)->inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
    }
    ShellPtr(CorePtr<T> *i) {
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr) {
                innr->outr = this;
                // innr->LinkAnce(ance, nullptr);
            }
            else {
                ipriv = innr->outr;
                inext = innr->outr->inext;
                innr->outr->inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;
            }
        }
    }

    ~ShellPtr() {
        DelIn();
    }

    ShellPtr<T>& operator=(ShellPtr<T> &o) {
        DelIn();
        innr = o.innr;
        if(innr != nullptr){
            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
        return *this;
    }
    ShellPtr<T>& operator=(ShellPtr<T> &&o) {
        DelIn();
        innr = o.innr;
        if(innr != nullptr){
            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
        return *this;
    }
    ShellPtr<T>& operator=(CorePtr<T> *i) {
        DelIn();
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr) {
                innr->outr = this;
            }
            else {
                ipriv = innr->outr;
                inext = innr->outr->inext;
                innr->outr->inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;
            }
        }
        return *this;
    }

    void Invalidate() {
        if(valid) {
            valid = false;
            if(innr != nullptr && innr->outr == this)
                innr->Invalidate();
        }
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

    friend bool operator==(const ShellPtr<T> &a, const ShellPtr<T> &b) {
        return a.innr == b.innr;
    }
    friend bool operator==(const ShellPtr<T> &s, std::nullptr_t) {
        return s.innr == nullptr;
    }
    friend bool operator==(std::nullptr_t, const ShellPtr<T> &s) {
        return s.innr == nullptr;
    }

    friend bool operator!=(const ShellPtr<T> &a, const ShellPtr<T> &b) {
        return a.innr != b.innr;
    }
    friend bool operator!=(const ShellPtr<T> &s, std::nullptr_t) {
        return s.innr != nullptr;
    }
    friend bool operator!=(std::nullptr_t, const ShellPtr<T> &s) {
        return s.innr != nullptr;
    }
};

template<typename>
struct isShellPtr : public std::false_type {};

template<typename V>
struct isShellPtr<ShellPtr<V>> : public std::true_type {};


template<typename T, typename... _Args>
inline ShellPtr<T> MakeShell(_Args&&... __args) {
    return ShellPtr<T>(new CorePtr<T>(std::forward<_Args>(__args)...));
}


}}

#endif
