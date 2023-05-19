//
// Created by 史浩 on 2023/5/10.
//

#ifndef NDK_CAMERARECORDER_LOCK_H
#define NDK_CAMERARECORDER_LOCK_H

#include <pthread.h>
#include "condition.h"


class Lock {

public:
    Lock();

    ~Lock();

    int lock();

    int unlock();

    Condition* newCondition();

private:
    pthread_mutex_t mutex;

};


#endif //NDK_CAMERARECORDER_LOCK_H
