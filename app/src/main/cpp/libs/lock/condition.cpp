//
// Created by 史浩 on 2023/5/10.
//

#include "condition.h"
Condition::Condition(pthread_mutex_t* mutex) {
    this->mutex=mutex;
    pthread_cond_init(&cond,0);
}

Condition::~Condition() {
    pthread_cond_destroy(&cond);
}

int Condition::await() {
    return pthread_cond_wait(&cond,mutex);
}

int Condition::signal() {
    return pthread_cond_signal(&cond);
}