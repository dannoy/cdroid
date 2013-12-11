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
    context->init(NULL, NULL,mainThread);
    return context;
}

void ContextImpl::init(ActivityManifest* amf, sp<IBinder> token, sp<ActivityThread> thread)
{
    mActivityMF = amf;
    mToken = token;
    mThread= thread;
}

void ContextImpl::setOuterContext(sp<Context> context)
{
    mOuterContext = context;
}



};
