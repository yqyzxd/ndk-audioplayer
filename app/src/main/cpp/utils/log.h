//
// Created by wind on 2023/5/6.
//

#ifndef NDK_HANDLER_LOG_H
#define NDK_HANDLER_LOG_H

#include <android/log.h>
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif //NDK_HANDLER_LOG_H
