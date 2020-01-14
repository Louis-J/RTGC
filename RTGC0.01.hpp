#include<iostream>

// //父层
// class HEAP_Ppac {
// public:
//     HEAP_Cpac *pChldNext = nullptr;//父层子女
//     bool isValid = true;

//     //加入到子节点
//     void PAdd(HEAP_Cpac *newChld){
//         if(pChldNext == nullptr) {
//             pChldNext = newChld;
//         } else {
//             // pChldNext->CAdd(newChld);
//             newChld->pChldNext = pChldNext;
//             newChld->pChldPriv = this;
//         }
//     }
//     // ~HEAP_Ppac() {
//     //     isValid = false;
//     //     while (pChldNext != nullptr) {
//     //         del pChldNext;//->CDel();
//     //     }
//     // }
// };

// //子层
// class HEAP_Cpac {
// public:
//     HEAP_Ppac *prnt;//父层
//     HEAP_Cpac *ppriv, *pnext;//父层兄弟
//     HEAP_Cpac *pChldPriv = nullptr;//父层兄弟
//     HEAP_Cpac *pChldNext = nullptr;//父层兄弟

//     // //加入到兄弟节点
//     // void PAdd(HEAP_Cpac *newChld){
//     //     if(pChldNext == nullptr) {
//     //         pChldNext = newChld;
//     //     } else {
//     //         pChldNext->CAdd(newChld);
//     //     }
//     // }
//     virtual void DelIn();
//     //删除子层，判断是否删除内层
//     ~HEAP_Cpac(){
//         DelIn();
//     }
// };


template<typename T>
class inner_ptr;

template<typename T>
class outer_ptr {//: HEAP_Cpac{
//private:
public:
    inner_ptr<T> *innr = nullptr;//指向的内节点，即指向的堆地址
    outer_ptr<T> *ipriv = nullptr, *inext = nullptr;//子层上一结点,下一结点

    // virtual void DelIn() {
    //     if (innr != nullptr) {
    //         if (innr->outr == innr) {
    //             if(ipriv != nullptr)
    //                 innr->outr = ipriv;
    //             else if(inext != nullptr)
    //                 innr->outr = ipriv;
    //             else
    //                 delete innr;
    //         }
    //     }
    //     if(ipriv != nullptr)
    //         ipriv->inext = ipriv;
    //     if(inext != nullptr)
    //         inext->ipriv = inext;
    // }
    
    //判断是否删除内层
    void DelIn() {
        if (innr != nullptr) {
            if (innr->outr == this) {
                if(ipriv != nullptr)
                    innr->outr = ipriv;
                else if(inext != nullptr)
                    innr->outr = ipriv;
                else
                    delete innr;
            }
        }
        if(ipriv != nullptr)
            ipriv->inext = ipriv;
        if(inext != nullptr)
            inext->ipriv = inext;
        innr = nullptr;
        ipriv = nullptr;
        inext = nullptr;
    }
    ~outer_ptr() {
        DelIn();
    }
public:
    // template<typename Prnt_t>
    // outer_ptr(outer_ptr<Prnt_t> *p, inner_ptr<T> *i){
    //     prnt = p;
    //     if(ifNull){
    //         ptr = nullptr;
    //     }else{
    //         ptr = new T(this, std::forward<Args>(args)...);
    //     }
    // }
    outer_ptr(inner_ptr<T> *i = nullptr){
        innr = i;
        if(innr != nullptr && innr->outr == nullptr){
            innr->outr = this;
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