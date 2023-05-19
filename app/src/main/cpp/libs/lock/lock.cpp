//
// Created by 史浩 on 2023/5/10.
//

#include "lock.h"
#include "../../utils/log.h"
#define LOG_TAG "Lock"
Lock::Lock() {
    pthread_mutex_init(&mutex,0);
}

Lock::~Lock() {
    pthread_mutex_destroy(&mutex);
}

int Lock::lock() {
   return pthread_mutex_lock(&mutex);
}

int Lock::unlock() {
    return pthread_mutex_unlock(&mutex);
}

Condition *Lock::newCondition() {
    return new Condition(&mutex);
}


