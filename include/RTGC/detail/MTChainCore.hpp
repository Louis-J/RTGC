#ifndef RTGC_DETAIL_MTCHAINCORE_HPP
#define RTGC_DETAIL_MTCHAINCORE_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class MTChainPtr;

//内层
template<typename T>
class MTChainCore {
    friend class MTChainPtr<T>;
    
    template<typename Tp, typename... _Args>
    friend MTChainPtr<Tp> MakeChain(_Args&&... __args);
    
    T real;
    size_t invalid = 0;// 0 为存活, 非0为将亡时删除的起始标记，用来区别不同线程
    std::atomic_flag mInnr = ATOMIC_FLAG_INIT;
    MTChainPtr<T> *outr = nullptr;//所有者结点

    [[gnu::always_inline]] void MTLockSub(size_t tag) {//被动调用，此时outr已上锁, this已上锁，且real需更新链
        if constexpr(decltype(haveMTInvalidate<T>(0))::value) {
            while(mInnr.test_and_set()); //得到mInnr
        }
    }
    [[gnu::always_inline]] void MTInvalidate(size_t tag) {//被动调用，此时outr已上锁, this已上锁，且real需更新链
        invalid = tag;
        if constexpr(decltype(haveMTInvalidate<T>(0))::value) {
            real.MTLockSub()
            mInnr.clear();
            real.MTInvalidate(tag);
        }
    }
    [[gnu::always_inline]] void MTTryValidate() {//被动调用，此时outr已上锁, this已上锁，且real需更新链
        if(outr->invalid == 0) {// 由已存活的outr调用
            invalid = 0;
            outr->mOutr.clear();
        } else if(outr->inext == nullptr) {
            if(outr->invalid != invalid) {// 由已存活的outr调用
                invalid = outr->invalid;
            }
            outr->mOutr.clear();
        } else {
            MTChainPtr<T> *j = nullptr; //用于存储首个可连接到其他线程的节点
            if(outr->invalid != invalid)
                j = outr;
            for(auto i = outr->inext; ; i = i->inext) {
                while(i->mOutr.test_and_set());
                if(i->invalid == 0) {
                    if(j) {
                        if(j == outr->inext) {
                            j->mOutr.clear();
                        } else if(j == i->priv) {
                            j->priv->mOutr.clear();
                        } else {
                            j->mOutr.clear();
                            j->priv->mOutr.clear();
                        }
                    }
                    auto ip = i->ipriv, in = i->inext;
                    ip->inext = in;
                    if(in != nullptr) {
                        while(in->mOutr.test_and_set());
                        in->ipriv = ip;
                        in->mOutr.clear();
                    }
                    if(ip != outr)
                        ip->mOutr.clear();
                    i->ipriv = nullptr;
                    i->inext = outr;
                    outr->ipriv = i;
                    // outr = i;
                    std::swap(outr, i);
                    outr->mOutr.clear();
                    i->mOutr.clear();
                    break;
                } else if(j == nullptr && i->invalid != invalid)
                    j = i;
                if(i != j && i->ipriv != j && i->ipriv != outr)
                    i->ipriv.mOutr.clear();
                if(i->inext = nullptr) {
                    if(j != i)
                        i->mOutr.clear();
                    if(j) { //存在j
                        MTChainPtr<T> *minPtr = (MTChainPtr<T> *)min(invalid, j->invalid);
                        minPtr

                        if(j == outr->inext) {
                            outr->inext = j->inext;
                            outr->ipriv = j->ipriv;
                            j->inext = outr;
                            j->ipriv = nullptr;
                            if(outr->inext != nullptr) {
                                while(outr->inext->mOutr.test_and_set());
                                outr->inext->ipriv = outr;
                                outr->inext->mOutr.clear();
                            }
                            std::swap(outr, j);
                            outr->mOutr.clear();
                            j->mOutr.clear();
                        } else {
                            auto jp = j->ipriv, jn = j->inext;
                            jp->inext = jn;
                            if(jn != nullptr) {
                                while(jn->mOutr.test_and_set());
                                jn->ipriv = jp;
                                jn->mOutr.clear();
                            }
                            jp->mOutr.clear();
                            outr->ipriv = j;
                            j->inext = outr;
                            j->ipriv = nullptr;
                            // outr = j;
                            std::swap(outr, j);
                            outr->mOutr.clear();
                            j->mOutr.clear();
                        }
                    } else { //不存在j
                        outr->mOutr.clear();
                    }
                }
            }
        }
        if constexpr(decltype(haveMTTryValidate<T>(0))::value) {
            real.MTTryValidate(tag, invalid);
        }

    }
    // [[gnu::always_inline]] void MTTryValidate() {//被动调用，此时outr已上锁, this已上锁，且real需更新链
    //     if(outr->invalid == 0) {// 由已存活的outr调用
    //         invalid = 0;
    //         outr->mOutr.clear();
    //     } else if(outr->invalid != invalid) { //outr连接到其他线程
    //         if(auto i = outr->inext; i == nullptr) { //被动连接到其他线程
    //             invalid = outr->invalid;
    //             outr->mOutr.clear();
    //         } else { //i != nullptr
    //             while(i->mOutr.test_and_set());
    //             if(i->invalid == 0) { //连接到存活对象outr->inext，而非其他线程, 交换outr和i
    //                 invalid = 0;
    //                 if(i->inext != nullptr) {
    //                     while(i->inext->mOutr.test_and_set());
    //                     i->inext->ipriv = outr;
    //                     i->inext->mOutr.clear();
    //                 }
    //                 outr->ipriv = i;
    //                 outr->inext = i->inext;
    //                 i->ipriv = nullptr;
    //                 i->inext = outr;
    //                 std::swap(outr, i);
    //                 outr->mOutr.clear();
    //                 i->mOutr.clear();
    //             } else while(true) { //outr->inext不存活, 尝试寻找其他存活节点
    //                 if(i->inext == nullptr) { //无其他存活节点, 被动连接到其他线程
    //                     invalid = outr->invalid;
    //                     outr->mOutr.clear();
    //                     i->mOutr.clear();
    //                     break;
    //                 }
    //                 i = i->next;
    //                 while(i->mOutr.test_and_set());
    //                 if(i->invalid == 0) { //连接到存活对象i，而非其他线程, 交换outr和i
    //                     auto ip = i->ipriv, in = i->inext;
    //                     ip->inext = in;
    //                     if(in != nullptr) {
    //                         while(in->mOutr.test_and_set());
    //                         in->ipriv = ip;
    //                         in->mOutr.clear();
    //                     }
    //                     ip->mOutr.clear();
    //                     i->ipriv = nullptr;
    //                     i->inext = outr;
    //                     outr->ipriv = i;
    //                     // outr = i;
    //                     std::swap(outr, i);
    //                     outr->mOutr.clear();
    //                     i->mOutr.clear();
    //                     break;
    //                 }
    //                 i->ipriv.mOutr.clear();
    //             }
    //         }
    //     } else { //outr失效, 需找其他线程
    //         if(auto i = outr->inext; i == nullptr) { //被动连接到其他线程
    //             invalid = outr->invalid;
    //             outr->mOutr.clear();
    //         } else { //i != nullptr
    //             while(i->mOutr.test_and_set());
    //             if(i->invalid == 0) { //连接到存活对象outr->inext，而非其他线程, 交换outr和i
    //                 invalid = 0;
    //                 if(i->inext != nullptr) {
    //                     while(i->inext->mOutr.test_and_set());
    //                     i->inext->ipriv = outr;
    //                     i->inext->mOutr.clear();
    //                 }
    //                 outr->ipriv = i;
    //                 outr->inext = i->inext;
    //                 i->ipriv = nullptr;
    //                 i->inext = outr;
    //                 std::swap(outr, i);
    //                 outr->mOutr.clear();
    //                 i->mOutr.clear();
    //             } else { //i->invalid != 0
    //                 MTChainPtr<T> *j = nullptr; //用于存储首个可连接到其他线程的节点
    //                 if(i->invalid != invalid) {
    //                     j = i;
    //                 }
    //                 while(true) { //outr->inext不存活, 尝试寻找其他存活节点
    //                     if(i->inext == nullptr) { //无其他存活节点, 被动连接到其他线程
    //                         if(!j) {
    //                             invalid = outr->invalid;
    //                             outr->mOutr.clear();
    //                             i->mOutr.clear();
    //                         } else {
    //                             if(j == outr->inext) {
    //                                 auto min
    //                             }
    //                         }
    //                         break;
    //                     }
    //                     i = i->next;
    //                     while(i->mOutr.test_and_set());
    //                     if(i->invalid == 0) { //连接到存活对象i，而非其他线程, 交换outr和i
    //                         if(j) {
    //                             if(j == outr->inext) {
    //                                 j->mOutr.clear();
    //                             } else if(j == i->priv) {
    //                                 j->priv->mOutr.clear();
    //                             } else {
    //                                 j->mOutr.clear();
    //                                 j->priv->mOutr.clear();
    //                             }
    //                         }
    //                         auto ip = i->ipriv, in = i->inext;
    //                         ip->inext = in;
    //                         if(in != nullptr) {
    //                             while(in->mOutr.test_and_set());
    //                             in->ipriv = ip;
    //                             in->mOutr.clear();
    //                         }
    //                         ip->mOutr.clear();
    //                         i->ipriv = nullptr;
    //                         i->inext = outr;
    //                         outr->ipriv = i;
    //                         // outr = i;
    //                         std::swap(outr, i);
    //                         outr->mOutr.clear();
    //                         i->mOutr.clear();
    //                         break;
    //                     }
    //                     if(j == nullptr && i->invalid != invalid)
    //                         j = i;
    //                     if(j != i && j != i->ipriv)
    //                         i->ipriv.mOutr.clear();
    //                 }
    //     }
    //         auto i = outr;
    //         while(true) {
    //             if(i->inext == )
    //         }
            
    //         for(; i != nullptr; i = i->inext) {
    //             while(i->mOutr.test_and_set());
    //             if(i->ipriv != outr)
    //                 i->ipriv->mOutr.clear();
    //             if(i->invalid == 0) { //连接到存活对象，而非其他线程
    //                 invalid = false;
    //                 auto ip = i->ipriv, in = i->inext;
    //                 ip->inext = in;
    //                 if(in != nullptr) {
    //                     in->ipriv = ip;
    //                 }
    //                 i->ipriv = nullptr;
    //                 i->inext = outr;
    //                 outr->ipriv = i;
    //                 outr = i;
    //                 break;
    //             }
    //         }
    //     } else for(auto i = outr->inext; i != nullptr; i = i->inext) {
    //         if(!i->invalid) {
    //             invalid = false;
    //             auto ip = i->ipriv, in = i->inext;
    //             ip->inext = in;
    //             if(in != nullptr) {
    //                 in->ipriv = ip;
    //             }
    //             i->ipriv = nullptr;
    //             i->inext = outr;
    //             outr->ipriv = i;
    //             outr = i;
    //             break;
    //         }
    //     }
    //     if constexpr(decltype(haveMTTryValidate<T>(0))::value) {
    //         real.TryValidate(tag, invalid);
    //     }
    // }
    template<class... Args>
    MTChainCore(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
