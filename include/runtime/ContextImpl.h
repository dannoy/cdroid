#ifndef _RUNTIME_CONTEXT_IMPL_H
#define _RUNTIME_CONTEXT_IMPL_H

#include <runtime/Context.h>

namespace cdroid {

class ActivityThread;
class ContextImpl : public Context {
public:
    ContextImpl();
    static ContextImpl* createSystemContext(ActivityThread *mainThread);
private:
    void init(ActivityThread *mainThread);

    sp<ActivityThread> mMainThread;
};

};

#endif
