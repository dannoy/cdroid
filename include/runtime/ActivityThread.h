#ifndef _RUNTIME_ACTIVITY_THREAD_H
#define _RUNTIME_ACTIVITY_THREAD_H

#include <runtime/common.h>
#include <runtime/Looper.h>

namespace cdroid {

class ActivityThread : public virtual RefBase{
public:
    ActivityThread();
    sp<Handler> getHandler();

private:
    class H : public Handler {
        virtual void handleMessage(const Message& message);
    };
    int attach(bool system);

private:
    sp<H> mH;
    bool mSystemThread;


// Static
public:
    static int main(Vector<String8>& args);
    static sp<Handler> getMainHandler();
private:
    static sp<Handler> sMainThreadHandler;
};

};


#endif
