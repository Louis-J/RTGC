#ifndef RTGC_DETAIL_SHELLPTR_HPP
#define RTGC_DETAIL_SHELLPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class CorePtr;

#ifdef RTGCDEBUG
#define RTGC_P(mut, num)                    \
for(auto j = 0; (mut).test_and_set(); j++) {\
    if(j%100 == 0)                          \
        std::cout << "\nstucked "#num"!";   \
}
#else
#define RTGC_P(mut, num) while((mut).test_and_set()){}
#endif

template<typename T>
class ShellPtr {
    friend class CorePtr<T>;
private:
    bool valid = true;
    std::atomic_flag mut = ATOMIC_FLAG_INIT;
    CorePtr<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    ShellPtr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
    void EraseMin() {
        if(ipriv != nullptr)
            ipriv->inext = inext;
        if(inext != nullptr)
            inext->ipriv = ipriv;
    }
    void Erase() {
        if(ipriv != nullptr)
            ipriv->inext = inext;
        if(inext != nullptr)
            inext->ipriv = ipriv;
        ipriv = nullptr;
        inext = nullptr;
    }
    //判断是否删除内层
    void DelIn() {
        if(innr != nullptr) {//主动删且有innr，此时this未上锁，innr未上锁
            RTGC_P(innr->mut, 0)
            if(innr->outr == this) {//有强引用innr，需更新链
                valid = false;
                innr->mut.clear();

                innr->Invalidate();
                innr->TryValidate();
                Erase();
                if(innr->outr == this && !innr->valid) {//innr废弃
                    delete innr;
                } else {//innr更新
                    valid = true;
                    innr->mut.clear();
                }
            } else {//无强引用innr
                Erase();
                innr->mut.clear();
            }
            innr = nullptr;
        }
    }
public:
    ~ShellPtr() {
        if(!valid) {//被动删，此时this上锁，innr上锁
            EraseMin();
            if(innr != nullptr && innr->outr == this)
                delete innr;
        } else if(innr != nullptr) {//主动删且有innr，此时this未上锁，innr未上锁
            RTGC_P(innr->mut, 1)
            if(innr->outr == this) {//有强引用innr，需更新链
                valid = false;
                innr->Invalidate();
                innr->TryValidate();
                if(innr->outr == this && !innr->valid) {//innr废弃
                    EraseMin();
                    delete innr;
                } else {//innr更新
                    Erase();
                    innr->mut.clear();
                }
            } else {//无强引用innr
                EraseMin();
                innr->mut.clear();
            }
        }
        //主动删且无innr
    }
public:
    ShellPtr() {}
    ShellPtr(ShellPtr<T> &o) {
        if(o.innr != nullptr) {
            innr = o.innr;
            RTGC_P(innr->mut, 2)

            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;

            innr->mut.clear();
        }
    }
    ShellPtr(const ShellPtr<T> &o) {
        if(o.innr != nullptr){
            innr = o.innr;
            RTGC_P(innr->mut, 3)

            ipriv = const_cast<ShellPtr<T>*>(&o);
            inext = o.inext;
            const_cast<ShellPtr<T>*>(&o)->inext = this;
            if(inext != nullptr)
                inext->ipriv = this;

            innr->mut.clear();
        }
    }
    ShellPtr(CorePtr<T> *i) {
        if(i != nullptr){
            innr = i;
            innr->outr = this;
        }
    }

    ShellPtr<T>& operator=(ShellPtr<T> &o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr){
                innr = o.innr;
                RTGC_P(innr->mut, 4)
                
                ipriv = &o;
                inext = o.inext;
                o.inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;

                innr->mut.clear();
            }
        }
        return *this;
    }
    ShellPtr<T>& operator=(ShellPtr<T> &&o) {
        if(innr != o.innr) {
            DelIn();
            if(o.innr != nullptr){
                innr = o.innr;
                RTGC_P(innr->mut, 5)
                
                ipriv = &o;
                inext = o.inext;
                o.inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;

                innr->mut.clear();
            }
        }
        return *this;
    }
    ShellPtr<T>& operator=(std::nullptr_t) {
        DelIn();
        return *this;
    }

    void Invalidate() {//被动调用，此时this未上锁，innr已上锁，且innr需更新链
        valid = false;
        if(innr != nullptr && innr->outr == this)
            innr->Invalidate();
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
