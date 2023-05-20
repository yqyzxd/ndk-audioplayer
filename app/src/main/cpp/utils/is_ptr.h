//
// Created by wind on 2023/5/20.
//

#ifndef NDK_AUDIOPLAYER_IS_PTR_H
#define NDK_AUDIOPLAYER_IS_PTR_H
template<typename T>
char isPtr(T*);//函数模板，没有函数体，返回为char

int isPtr(...);//可变参数函数，没有函数体，返回为int

#define ISPTR(v) (sizeof(isPtr(v)) == sizeof(char))

#endif //NDK_AUDIOPLAYER_IS_PTR_H
