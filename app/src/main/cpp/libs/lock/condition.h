//
// Created by 史浩 on 2023/5/10.
//

#ifndef NDK_CAMERARECORDER_CONDITION_H
#define NDK_CAMERARECORDER_CONDITION_H

#include <pthread.h>
class Condition {
public:
    Condition(pthread_mutex_t* mutex);
    ~Condition();

    int await();
    int signal();
private:
    pthread_mutex_t* mutex;
    pthread_cond_t cond;
};


#endif //NDK_CAMERARECORDER_CONDITION_H
