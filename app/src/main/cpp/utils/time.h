//
// Created by 史浩 on 2023/5/13.
//

#ifndef NDK_CAMERARECORDER_TIME_H
#define NDK_CAMERARECORDER_TIME_H

static inline long currentTimeMills(){
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}



#endif //NDK_CAMERARECORDER_TIME_H
