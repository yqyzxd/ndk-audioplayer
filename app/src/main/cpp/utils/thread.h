//
// Created by wind on 2023/5/6.
//

#ifndef NDK_HANDLER_THREAD_H
#define NDK_HANDLER_THREAD_H
#include <pthread.h>
#include "runnable.h"
void* threadCallback(void* ctx);

class Thread {

public:
    Thread(Runnable* runnable);
    Thread():Thread((Runnable*)nullptr){};
    virtual ~Thread();

    /**启动线程*/
    virtual void start();

    /**线程函数*/
    virtual void run();

    virtual void join();

private:
    pthread_t threadId;
    Runnable* runnable;
   friend void* threadCallback(void* ctx);
};


#endif //NDK_HANDLER_THREAD_H
