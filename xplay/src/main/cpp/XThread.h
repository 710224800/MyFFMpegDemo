//
// Created by lyhao on 20-10-12.
//

#ifndef XPLAY_XTHREAD_H
#define XPLAY_XTHREAD_H


class XThread {
public:
    virtual void Start();

    virtual void Stop();
    //入口主函数
    virtual void Main(){}

private:
    void ThreadMain();
};


#endif //XPLAY_XTHREAD_H
