//
// Created by 史浩 on 2023/5/10.
//

#ifndef NDK_CAMERARECORDER_BLOCKING_QUEUE_H
#define NDK_CAMERARECORDER_BLOCKING_QUEUE_H

#include <climits>
#include <pthread.h>
#include "../lock/lock.h"
#define LOG_TAG "BlockingQueue"
template<class T>
class BlockingQueue {
public:
    BlockingQueue(int capacity){
        this->mCapacity=capacity;
        this->mSize=0;
    };
    BlockingQueue():BlockingQueue(INT_MAX){};
    virtual ~BlockingQueue(){};
    /**
     * Inserts the specified element into this queue, waiting if necessary for space to become available.
     * @param elem
     */
    virtual int put(T elem)=0;
    /**
     * Retrieves and removes the head of this queue, waiting if necessary until an element becomes available.
     * @param elem  the head of this queue
     * @return
     */
    virtual int take(T* elem)=0;

    virtual void flush()=0;
    virtual int size()=0;
protected:
    int mCapacity;
    int mSize;
    bool mAbort= false;
};


////////////////////LinkedBlockingQueue////////////////////////////////////////
template<class T>
class Node {
public:
    Node(T item,Node<T>* next){
        this->item=item;
        this->next=next;
    }
    ~Node(){
    }
    T item;
    Node<T>* next= nullptr;
};

template<class T>
class LinkedBlockingQueue : public BlockingQueue<T> {
public:
    LinkedBlockingQueue(int capacity):BlockingQueue<T>(capacity){
        mLock=new Lock();
        mCond= mLock->newCondition();
    };
    LinkedBlockingQueue():LinkedBlockingQueue(INT_MAX){};
    ~LinkedBlockingQueue(){
        delete head;
        delete last;
        delete mLock;
    };

    int put(T elem) override{
        if (this->mAbort){
            return -1;
        }
        mLock->lock();
        if (this->mSize>=this->mCapacity){
            mCond->await();
        }
        Node<T>* node=new Node<T>(elem, nullptr);
        if (head== nullptr){
            head=last=node;
        } else {
            last->next=node;
            last=node;
        }
        this->mSize++;
        mCond->signal();
        mLock->unlock();
        return 0;
    }
    int take(T *elem) override{
        if (this->mAbort){
            return -1;
        }
        mLock->lock();
        if (this->mSize<=0){
            mCond->await();
        }
        if (head== nullptr){
            return -1;
        }
        *elem=(head->item);

        head=head->next;
        this->mSize--;
        mCond->signal();
        mLock->unlock();
        return 0;
    }
    //todo 清空队列
    void flush() override{
        mLock->lock();
        this->mAbort=true;
        head= nullptr;
        mCond->signal();
        this->mSize=0;
        mLock->unlock();

    }
    int size() override{
        return this->mSize;
    }
private:
    Lock* mLock;
    Condition* mCond;

    Node<T>* head= nullptr;
    Node<T>* last= nullptr;

};





#endif //NDK_CAMERARECORDER_BLOCKING_QUEUE_H
