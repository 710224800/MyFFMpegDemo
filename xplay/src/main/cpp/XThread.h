//
// Created by lyhao on 20-10-12.
//

#ifndef XPLAY_XTHREAD_H
#define XPLAY_XTHREAD_H

void XSleep(int mis);

class XThread {
public:
    virtual void start();

    virtual void stop();
    //入口主函数
    virtual void main(){}

protected:
    bool isExit = false;
    bool isRunning = false;

private:
    void threadMain();
};


#endif //XPLAY_XTHREAD_H
