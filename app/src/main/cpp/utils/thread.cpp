//
// Created by wind on 2023/5/6.
//

#include "thread.h"

Thread::Thread(Runnable* runnable) {
    this->runnable=runnable;
}
Thread::~Thread() {
    if(runnable!= nullptr){
        delete runnable;
        runnable= nullptr;
    }
}

void *threadCallback(void *ctx) {
    Thread* thread=(Thread*)ctx;
    thread->run();
    return 0;
}

void Thread::run() {
    if(runnable!= nullptr){
        runnable->run();
    }
}

void Thread::start() {
    pthread_create(&threadId,0,threadCallback,this);
}

void Thread::join() {
    pthread_join(threadId,0);
}