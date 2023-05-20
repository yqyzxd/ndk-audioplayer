//
// Created by wind on 2023/5/20.
//

#ifndef NDK_AUDIOPLAYER_READ_STRATEGY_H
#define NDK_AUDIOPLAYER_READ_STRATEGY_H

#include "../utils/log.h"
#define LOG_TAG "ReadStrategy"
/**
 * 读取解码出来的pcm音频数据
 * 方案一：DataSizeReadStrategy每次读取一个AudioFrame中的数据，如果readSamples的size小于AudioFrame的size，那么多余的将会被抛弃。优点是实现简单
 * 方案二：每次按readSamples的size进行读取，不舍弃AudioFrame中的数据。缺点是实现稍复杂
 */
template<typename T>
class ReadStrategy {

public:
    ReadStrategy(){};
    virtual ~ReadStrategy(){};

    virtual int read(T* srcData,int srcSize,T* readData,int readSize)=0;
};

template<typename T>
class DataSizeReadStrategy : public ReadStrategy<T>{

public:
    DataSizeReadStrategy(){};
    ~DataSizeReadStrategy(){};

    int read(T* srcData,int srcSize,T* readData,int readSize){
        int actualSize=srcSize;
        if (srcSize > readSize) {
            LOGE("the readSize parameter is too small");
            actualSize=readSize;
        }

        LOGI("readSamples size:%d,actualSize:%d",readSize,actualSize);
        //memcpy 的第三个参数是字节个数，因为这里是short，占用2个自己，所以要乘以2
        LOGI("sizeof(T):%d",sizeof(T));
        memcpy(readData, srcData, actualSize*sizeof(T));

        return actualSize;
    }
};

template<typename T>
class ReadSizeReadStrategy : public ReadStrategy<T> {
private:
    int readPos;
    T* data;
public:
    ReadSizeReadStrategy();

    ~ReadSizeReadStrategy();

    int read(T* srcData,int srcSize,T* readData,int readSize){

        return 0;
    }

};
#endif //NDK_AUDIOPLAYER_READ_STRATEGY_H
