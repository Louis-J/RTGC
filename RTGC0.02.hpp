#include<iostream>

template<typename T>
class inner_ptr;

template<typename T>
class outer_ptr {
//private:
public:
    inner_ptr<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    outer_ptr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点
    
    //判断是否删除内层
    void DelIn() {
        if (innr != nullptr) {
            if (innr->outr == this) {
                if(ipriv != nullptr)
                    innr->outr = ipriv;
                else if(inext != nullptr)
                    innr->outr = inext;
                else
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
    ~outer_ptr() {
        DelIn();
    }
public:
    outer_ptr(outer_ptr<T> &&o){
        innr = o.innr;
        if(innr != nullptr){
            ipriv = &o;
            inext = o.inext;
            o.inext = this;
            if(inext != nullptr)
                inext->ipriv = this;
        }
    }
    outer_ptr(inner_ptr<T> *i = nullptr){
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr)
                innr->outr = this;
            else {
                ipriv = innr->outr;
                inext = innr->outr->inext;
                innr->outr->inext = this;
                if(inext != nullptr)
                    inext->ipriv = this;
            }
        }
    }
    outer_ptr<T>& operator=(outer_ptr<T> &o){
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
    outer_ptr<T>& operator=(inner_ptr<T> *i){
        DelIn();
        innr = i;
        if(innr != nullptr){
            if(innr->outr == nullptr)
                innr->outr = this;
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

//内层
template<typename T>
class inner_ptr {
//private:
public:
    outer_ptr<T> *outr = nullptr;//所有者结点
    T real;
public:
    template<class... Args>
    inner_ptr(Args&&... args) :real(std::forward<Args>(args)...) {}
};