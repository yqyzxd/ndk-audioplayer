//
// Created by wind on 2023/3/30.
//

#ifndef NDK_PNG_RUNNABLE_H
#define NDK_PNG_RUNNABLE_H
class Runnable {
public:
    Runnable(void (*runFp)(void*),void* params):runFp(runFp),params(params){}
    ~Runnable(){ runFp = nullptr;}
    void run(){
        runFp(params);
    }

private:
    void (*runFp)(void*);
    void* params;


};
#endif //NDK_PNG_RUNNABLE_H
