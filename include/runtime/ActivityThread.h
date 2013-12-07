#ifndef _RUNTIME_ACTIVITY_THREAD_H
#define _RUNTIME_ACTIVITY_THREAD_H

#include <runtime/common.h>
#include <runtime/Looper.h>
#include <runtime/IApplicationThread.h>

namespace cdroid {

class ActivityThread : public virtual RefBase{
public:
    ActivityThread();
    sp<Handler> getHandler();

private:
    class H : public Handler {
        virtual void handleMessage(const Message& message);
    };

    class ApplicationThread : public BnApplicationThread {
    };

    int attach(bool system);

private:
    sp<H> mH;
    bool mSystemThread;
    sp<IApplicationThread> mAppThread;;


// Static
public:
    static int main(Vector<String8>& args);
    static sp<Handler> getMainHandler();
    static sp<ActivityThread> getCurrentActivityThread();
private:
    static sp<Handler> sMainThreadHandler;
    static sp<ActivityThread> sCurrentActivityThread;
};

};


#endif
