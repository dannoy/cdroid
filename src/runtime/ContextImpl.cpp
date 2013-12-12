#define LOG_TAG "ContextImpl"
#include <cutils/log.h>

#include <runtime/ContextImpl.h>
#include <runtime/ActivityThread.h>

#include <service/IActivityManager.h>

namespace cdroid {

ContextImpl::ContextImpl()
{
}

ContextImpl* ContextImpl::createSystemContext(ActivityThread *mainThread)
{
    ContextImpl *context = new ContextImpl;
    context->init(NULL, NULL,mainThread, NULL);
    return context;
}

void ContextImpl::init(ActivityManifest* amf, sp<IBinder> token, sp<ActivityThread> thread, sp<Looper> cmdLooper)
{
    mActivityMF = amf;
    mToken = token;
    mThread= thread;

    mCmdLooper = cmdLooper;
}

void ContextImpl::setOuterContext(sp<Context> context)
{
    mOuterContext = context;
}

sp<Handler> ContextImpl::getCmdHandler()
{
    if(mCmdHandler == NULL) {
        AutoMutex _l(mCmdMutex);
        if(mCmdHandler == NULL) {
            mCmdHandler = new H(this, mCmdLooper);
        }
    }

    return mCmdHandler;
}

int ContextImpl::execInternalCommand(String8 cmd)
{
    sp<Text> txt = new Text(cmd);
    sp<Message> msg = new Message(H::ON_CMD, txt);

    getCmdHandler()->sendMessage(msg);

    return 0;
}

int ContextImpl::startActivity(sp<Intent> intent)
{
    //ALOGI("startActivity %s", intent->getAction().string());

    ActivityManagerNative::getDefault()->startActivity(mThread->getApplicationThread(), mToken, intent, -1);

    return 0;
}

int ContextImpl::startService(sp<Intent> intent)
{
    //ALOGI("startService %s", intent->getAction().string());
    ActivityManagerNative::getDefault()->startService(mThread->getApplicationThread(), intent);
    return 0;
}

int ContextImpl::handleInternalCommand(String8 cmd)
{
    //ALOGI("handleInternalcommand %s", cmd.string());
    const char *space = " ";
    int pos = cmd.find(space);
    //ALOGI("handleInternalcommand pos %d", pos);
    switch(pos) {
        case 13:
            {
                sp<Intent> intent = new Intent(String8(cmd.string() + pos + 1));
                return startActivity(intent);
            }
            break;
        case 12:
            {
                sp<Intent> intent = new Intent(String8(cmd.string() + pos + 1));
                return startService(intent);
            }
            break;
    }

    ALOGI("handleInternalcommand ommit unknow command %s", cmd.string());
    
    return -1;
}

void ContextImpl::H::handleMessage(const sp<Message>& message)
{
    switch(message->what) {
        case ON_CMD:
            {
                sp<Text> txt = reinterpret_cast<Text*>(message->obj.get());
                mContext->handleInternalCommand(txt->getString());
            }
            break;
    }
}


};
