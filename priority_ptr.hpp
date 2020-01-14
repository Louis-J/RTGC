#include<iostream>

template<typename T>
class outer_ptr{
//private:
public:
    int priority;//当前节点的优先级，由上层提供
    outer_ptr<T> *priv, *next;//父层上一结点,下一结点
    inner_ptr<T> *ptrI;//指向的内节点，即指向的堆地址
    //释放子节点
    void fDel(){
        //非空
        if(ptrI != nullptr){
            //为所有者时
            if(priv == nullptr){
                //为唯一使用者时
                if(next == nullptr){
                    delete ptrI;
                }
                else{
                    //有其他使用者时，转移所有权，更新子层状态
                    next->priv = nullptr;
                    ptrI->head = next;
                    next = nullptr;
                    ptrI = nullptr;
                    ptrI->sRfs();
                }
            }
            //不为所有者时，仅更新父层信息
            else{
                priv->next = next;
                if(next != nullptr){
                    next->priv = priv;
                }
            }
        }
    }
    //加入到子节点
    void fAdd(){
        //悬挂的子节点
        if(ptrI->head == nullptr){
            priv = nullptr;
            next = nullptr;
            ptrI->head = this;
            ptrI->sRfs();
        }
        //非悬挂的，但为最高层级节点，作为所有者插入最前
        else if(priority <= ptrI->head->priority){
            priv = nullptr;
            next = ptrI->head;
            ptrI->head->prev = this;
            ptrI->head = this;
            ptrI->sRfs();
        }
        //需要排序插入
        else{
            auto h = ptrI->head;
            for(; h->next != nullptr && h->next->priority < priority; h = h->next){}
            next = h->next;
            prev = h;
            h->next = this;
            if(next != nullptr){
                next->prev = this;
            }
        }
    }
    //TODO:自身需要更新(由于父节点)
    void fRfs(int priNew){
    }
public:
    // template<class ...Args>
    // outer_ptr(bool ifNull, Args &&...args){
    //     if(ifNull){
    //         ptr = nullptr;
    //     }else{
    //         ptr = new T(this, std::forward<Args>(args)...);
    //     }
    // }
    // outer_ptr(outer_ptr<T> &o){
    //     fDel();
    //     if(o.ptr != nullptr){
    //         ptr=o.ptr;
    //         fAdd();
    //     }
    //     return *this;
    // }
    template<typename Prnt_t>
    outer_ptr(outer_ptr<Prnt_t> *p, inner_ptr<T> *i){
        if(ifNull){
            ptr = nullptr;
        }else{
            ptr = new T(this, std::forward<Args>(args)...);
        }
    }
    static void *operator new(size_t size) {
        //TODO:
    }
    ~outer_ptr(){
        fDel();
    }
    outer_ptr<T>& operator=(outer_ptr<T> &o){
        fDel();
        if(o.ptr != nullptr){
            ptr=o.ptr;
            fAdd();
        }
        return *this;
    }
    int getPri(){
        return priority;
    }
}


template<typename T>
class root_ptr{
//private:
public:
}



template<typename T>
class agent_ptr{
//private:
public:
}



template<typename T>
class inner_ptr{
//private:
public:
    int priority;//所有者的优先级
    outer_ptr<T> *nodeO;//所有者结点
    outer_ptr<T> *nodeI;//子层下一结点
    T real;
    //TODO:自身需要更新(由于优先级变化)
    void sRfs(){
        if(priority != head->priority + 1){
            priority = head->priority + 1;
            for(auto i = nodeI; i!= NULL; i = i->nextI){
                i->fRfs();
            }
        }
    }
public:
    template<class... Args>
    inner_ptr(outer_ptr<T> *f, Args&&... args):priority(f.getPri),
        nodeO(f),
        nodeI(nullptr),
        T(std::forward<Args>(args)...){}
}