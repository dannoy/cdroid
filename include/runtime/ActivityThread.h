#ifndef _RUNTIME_ACTIVITY_THREAD_H
#define _RUNTIME_ACTIVITY_THREAD_H

#include <runtime/common.h>
#include <runtime/Looper.h>
#include <runtime/IApplicationThread.h>

#include <runtime/ContextImpl.h>

namespace cdroid {

class ActivityThread : public virtual RefBase{
public:
    ActivityThread();
    sp<Handler> getHandler();
    sp<ContextImpl> getSystemContext();

private:
    class H : public Handler {
        virtual void handleMessage(const Message& message);
    };

    class ApplicationThread : public BnApplicationThread {
    public:
        virtual void schedulePauseActivity(sp<IBinder> token);
        virtual void bindApplication(String8 appName);
    };

    int attach(bool system);

private:
    sp<H> mH;
    bool mSystemThread;
    sp<ApplicationThread> mAppThread;;


// Static
public:
    static int main(Vector<String8>& args);
    static sp<Handler> getMainHandler();
    static sp<ActivityThread> getCurrentActivityThread();
    static sp<ActivityThread> systemMain();
    private:
    static sp<Handler> sMainThreadHandler;
    static sp<ActivityThread> sCurrentActivityThread;
    static sp<ContextImpl> sSystemContext;
};

};


#endif
