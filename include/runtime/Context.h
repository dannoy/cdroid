#ifndef _RUNTIME_CONTEXT_H
#define _RUNTIME_CONTEXT_H

#include <runtime/common.h>

namespace cdroid {

class Context : public RefBase {
public:

};

class ContextWrapper : public Context {
public:
    int attachBaseContext(sp<Context> base);

private:
    sp<Context> mBase;
};

};

#endif
