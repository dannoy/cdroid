#define LOG_TAG "ContextImpl"
#include <cutils/log.h>

#include <runtime/ContextImpl.h>
#include <runtime/ActivityThread.h>

namespace cdroid {

ContextImpl::ContextImpl()
{
}

ContextImpl* ContextImpl::createSystemContext(ActivityThread *mainThread)
{
    ContextImpl *context = new ContextImpl;
    context->init(mainThread);
    return context;
}

void ContextImpl::init(ActivityThread *mainThread)
{
    mMainThread = mainThread;
}



};
