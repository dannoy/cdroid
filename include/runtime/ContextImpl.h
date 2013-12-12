#ifndef _RUNTIME_CONTEXT_IMPL_H
#define _RUNTIME_CONTEXT_IMPL_H

#include <runtime/Context.h>
#include <runtime/ApplicationLoader.h>

namespace cdroid {

class ActivityThread;
class ContextImpl : public Context {
public:
    // init methos
    ContextImpl();
    static ContextImpl* createSystemContext(ActivityThread *mainThread);
    void init(ActivityManifest* amf, sp<IBinder> token, sp<ActivityThread> thread, sp<Looper> cmdLooper);
    void init(ServiceManifest* smf, sp<IBinder> token, sp<ActivityThread> thread);
    void setOuterContext(sp<Context> context);

    // Context interfaces
    virtual int execInternalCommand(String8 cmd);
    virtual int startActivity(sp<Intent> intent);
    virtual int startService(sp<Intent> intent);


private:
    class H : public Handler {
    public:
        H(sp<ContextImpl> ctx, sp<Looper> looper)
            : mContext(ctx),
              Handler(looper)
        {
        }
        enum {
            ON_CMD = 1,
        };
        virtual void handleMessage(const sp<Message>& message);
    private:
        sp<ContextImpl> mContext;
    };
private:
    sp<Handler> getCmdHandler();
    int handleInternalCommand(String8 cmd);

private:

    ActivityManifest* mActivityMF;
    ServiceManifest* mServiceMF;
    sp<IBinder> mToken;
    sp<ActivityThread> mThread;
    sp<Context> mOuterContext;

    sp<Looper> mCmdLooper;
    sp<Handler> mCmdHandler;
    Mutex mCmdMutex;
};

};

#endif
