#ifndef RTGC_DETAIL_SHELLPTR_HPP
#define RTGC_DETAIL_SHELLPTR_HPP

#include<cstddef>

namespace RTGC { namespace detail {

template<typename T>
class CorePtr;

template<typename T>
class ShellPtr {
private:
    void *ance;
    CorePtr<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    ShellPtr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
    //转移所有权
    bool SwitchL() {
        if(ipriv != nullptr) {
            if(ipriv->ance == ance) {
                return ipriv->SwitchL();
            } else {
                innr->LinkAnce(ipriv->ance, ance);
                innr->outr = ipriv;
                return true;
            }
        }
        return false;
    }
    bool SwitchR() {
        if(inext != nullptr) {
            if(inext->ance == ance) {
                return inext->SwitchR();
            } else {
                innr->outr = inext;
                innr->LinkAnce(inext->ance, ance);
                return true;
            }
        }
        return false;
    }
    //判断是否删除内层
    void DelIn() {
        if (innr != nullptr) {
            if (innr->outr == this) {
                if(!SwitchL() && !SwitchR())
                    delete innr;
            }
        }
        if(ipriv != nullptr)
            ipriv->inext = inext;
        if(inext != nullptr)
            inext->ipriv = ipriv;
        innr = nullptr;
        ipriv = nullptr;
        inext = nullptr;
    }
public:
    ShellPtr(ShellPtr<T> &o) : ance(this) {
        innr = o.innr;
        if(innr != nullptr){
            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
    }
    ShellPtr(CorePtr<T> *i = nullptr) : ance(this) {
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr) {
                innr->outr = this;
                innr->LinkAnce(ance, nullptr);
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
                innr->LinkAnce(ance, nullptr);
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

    void LinkInit(void *n) {
        void *o = ance;
        ance = n;
        if(innr != nullptr)
            innr->LinkAnce(n, o);
    }
    void LinkAnce(void *n, void *o) {
        if(ance == o) {
            ance = n;
            if(innr != nullptr)
                innr->LinkAnce(n, o);
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

}}

#endif
