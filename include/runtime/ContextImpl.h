#ifndef _RUNTIME_CONTEXT_IMPL_H
#define _RUNTIME_CONTEXT_IMPL_H

#include <runtime/Context.h>
#include <runtime/ApplicationLoader.h>

namespace cdroid {

class ActivityThread;
class ContextImpl : public Context {
public:
    ContextImpl();
    static ContextImpl* createSystemContext(ActivityThread *mainThread);
    void init(ActivityManifest* amf, sp<IBinder> token, sp<ActivityThread> thread);
    void setOuterContext(sp<Context> context);
private:

    ActivityManifest* mActivityMF;
    sp<IBinder> mToken;
    sp<ActivityThread> mThread;
    sp<Context> mOuterContext;
};

};

#endif
