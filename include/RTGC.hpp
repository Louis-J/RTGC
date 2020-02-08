#include<type_traits>
#include<utility>
#include<getstructmember.hpp>

namespace RTGC {

template<typename T>
class inner_ptr;

template<typename T>
class outer_ptr {
//private:
public:
    void *ance;
    inner_ptr<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    outer_ptr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
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
    void LinkAnce(void *n, void *o = nullptr) {
        if(ance == o) {
            ance = n;
            if(innr != nullptr)
                innr->LinkAnce(n, o);
        }
    }
    ~outer_ptr() {
        DelIn();
    }
public:
    outer_ptr(void *ance, outer_ptr<T> &o) : ance(ance) {
        innr = o.innr;
        if(innr != nullptr){
            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
    }
    outer_ptr(void *ance, inner_ptr<T> *i = nullptr) : ance(ance) {
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr) {
                innr->outr = this;
                innr->LinkAnce(ance);
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
    void assign(outer_ptr<T> &o) {
        DelIn();
        innr = o.innr;
        if(innr != nullptr){
            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
    }
    void assign(inner_ptr<T> *i){
        DelIn();
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr) {
                innr->outr = this;
                innr->LinkAnce(ance);
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
    outer_ptr<T>& operator=(outer_ptr<T> &o) {
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
    outer_ptr<T>& operator=(outer_ptr<T> &&o) {
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
    outer_ptr<T>& operator=(inner_ptr<T> *i) {
        DelIn();
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr) {
                innr->outr = this;
                innr->LinkAnce(ance);
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
    T& operator*() {
        return innr->real;
    }
    T* operator->() {
        return &(innr->real);
    }
    bool isNull(){
        return innr == nullptr;
    }
};

template<typename T>
class root_ptr : public outer_ptr<T> {
public:
    root_ptr(root_ptr<T> &o) : outer_ptr<T>(this, o) {}
    root_ptr(root_ptr<T> &&o) : outer_ptr<T>(this, move(o)) {}
    root_ptr(outer_ptr<T> &o) : outer_ptr<T>(this, o) {}
    root_ptr(outer_ptr<T> &&o) : outer_ptr<T>(this, move(o)) {}
    root_ptr(inner_ptr<T> *i = nullptr) : outer_ptr<T>(this, i) {}
    root_ptr<T>& operator=(root_ptr<T> &o) {
        outer_ptr<T>::assign(o);
        return *this;
    }
    root_ptr<T>& operator=(root_ptr<T> &&o) {
        outer_ptr<T>::assign(o);
        return *this;
    }
    root_ptr<T>& operator=(inner_ptr<T> *i) {
        outer_ptr<T>::assign(i);
        return *this;
    }
};

template<typename T>
class agent_ptr : public outer_ptr<T> {
public:
    agent_ptr(agent_ptr<T> &o) : outer_ptr<T>(nullptr, o) {}
    agent_ptr(agent_ptr<T> &&o) : outer_ptr<T>(nullptr, move(o)) {}
    agent_ptr(outer_ptr<T> &o) : outer_ptr<T>(nullptr, o) {}
    agent_ptr(outer_ptr<T> &&o) : outer_ptr<T>(nullptr, move(o)) {}
    agent_ptr(inner_ptr<T> *i = nullptr) : outer_ptr<T>(nullptr, i) {}
    agent_ptr<T>& operator=(agent_ptr<T> &o) {
        outer_ptr<T>::assign(o);
        return *this;
    }
    agent_ptr<T>& operator=(agent_ptr<T> &&o) {
        outer_ptr<T>::assign(o);
        return *this;
    }
    agent_ptr<T>& operator=(inner_ptr<T> *i) {
        outer_ptr<T>::assign(i);
        return *this;
    }
};


template<typename U>
static constexpr auto haveLinkAnce(int) -> decltype(U::LinkAnce, std::true_type());

template<typename U>
static constexpr std::false_type haveLinkAnce(...);

//内层
template<typename T>
class inner_ptr {
    friend class outer_ptr<T>;
    
// public:
    outer_ptr<T> *outr = nullptr;//所有者结点

    T real;
    void LinkAnce(void *n, void *o = nullptr){
        if constexpr(std::is_same<decltype(haveLinkAnce<T>(0)), std::true_type>::value) {
        // if constexpr(haveLinkAnce<T>) {
            real.LinkAnce(n, o);
        }
    }
public:
    template<class... Args>
    inner_ptr(Args&&... args) :real(std::forward<Args>(args)...) {}
};


template<typename>
struct isRootPtr : public std::false_type {};

template<typename V>
struct isRootPtr<root_ptr<V>> : public std::true_type {};


template<class MB>
constexpr void linkMemberOne(MB &mb, void *n, void *o) {
    static_assert(
        !isRootPtr<MB>::value,
        "====================> RTGC: cannot use root_ptr in a class!"
    );
    if constexpr(std::is_same<decltype(haveLinkAnce<MB>(0)), std::true_type>::value) {
        mb.LinkAnce(n, o);
    }
}

template<class Tuple, std::size_t... Is>
constexpr void linkMembersImpl(const Tuple& t, void *n, void *o, std::index_sequence<Is...>) {
    (linkMemberOne(std::get<Is>(t), n, o), ...);
}

template<class... Args>
constexpr void linkMembers(void *n, void *o, const std::tuple<Args...>& t)
{
    linkMembersImpl(t, n, o, std::index_sequence_for<Args...>{});
}

}

#define CLASSLINK(cName, mNum)                                                                                  \
using thisT = cName;                                                                                            \
friend class RTGC::reflect::toTuple<thisT>;                                                                     \
public:                                                                                                         \
constexpr inline void LinkAnce(void *n, void *o = nullptr) {                                                    \
    auto &&tieTuple = RTGC::reflect::toTuple<thisT>::tie_as_tuple(*this, RTGC::reflect::const_size_t<mNum>());  \
    RTGC::linkMembers(n, o, tieTuple);                                                                          \
}
