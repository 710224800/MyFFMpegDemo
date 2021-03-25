//
// Created by lyhao on 20-10-12.
//

#ifndef XPLAY_XTHREAD_H
#define XPLAY_XTHREAD_H

void XSleep(int mis);

class XThread {
public:
    virtual bool start();

    virtual void stop();

    virtual void setPause(bool isP);

    virtual bool IsPause(){
        isPausing = isPause;
        return isPause;
    }

    //入口主函数
    virtual void main(){}

protected:
    bool isExit = false;
    bool isRunning = false;
    bool isPause = false;
    bool isPausing = false;

private:
    void threadMain();
};


#endif //XPLAY_XTHREAD_H
