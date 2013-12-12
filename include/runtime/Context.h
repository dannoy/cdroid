#ifndef _RUNTIME_CONTEXT_H
#define _RUNTIME_CONTEXT_H

#include <runtime/common.h>
#include <runtime/Intent.h>

namespace cdroid {

class Context : public RefBase {
public:
    virtual int startActivity(sp<Intent> intent) = 0;
    virtual int startService(sp<Intent> intent) = 0;
    virtual int execInternalCommand(String8 cmd) = 0;
};

class ContextWrapper : public Context {
public:
    int attachBaseContext(sp<Context> base);

    virtual int execInternalCommand(String8 cmd);
    virtual int startActivity(sp<Intent> intent);
    virtual int startService(sp<Intent> intent);

private:
    sp<Context> mBase;
};

};

#endif
