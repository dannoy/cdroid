#define LOG_TAG "CDROID::Looper"

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <binder/BinderService.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "runtime/Looper.h"

namespace cdroid {

Handler::Handler()
{
    mLooper = Looper::myLooper();
}

void Handler::handleMessage(const android::Message &message)
{
    handleMessage((const Message&)message);
}

void Handler::sendMessage(const Message& message)
{
    nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
    sendMessageAtTime(now, message);
}

void Handler::sendMessageDelayed(nsecs_t uptimeDelay, const Message& message)
{
    nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
    sendMessageAtTime(now + uptimeDelay, message);
}

void Handler::sendMessageAtTime(nsecs_t uptime, const Message& message)
{
    mLooper->sendMessageAtTime(uptime, this, message);
}

sp<Looper> Looper::mMainLooper;

Looper::Looper()
    : android::Looper(false)
{
}

void Looper::prepare()
{
    sp<android::Looper> looper = android::Looper::getForThread();
    if (looper == NULL) {
        looper = new Looper();
        android::Looper::setForThread(looper);
    }
}

void Looper::prepareMainLooper()
{
    prepare();
    setMainLooper(myLooper());
}

void Looper::setMainLooper(sp<Looper> l)
{
    mMainLooper = l;
}

sp<Looper> Looper::getMainLooper()
{
    return mMainLooper;
}

sp<Looper> Looper::myLooper()
{
    return reinterpret_cast<Looper *>(Looper::getForThread().get());
}

int Looper::addFd(int fd, int events, const sp<LooperCallback>& callback, void* data)
{
    return android::Looper::addFd(fd, (int)ALOOPER_POLL_CALLBACK, events, callback, data);
}
void Looper::loop()
{
    myLooper().get()->pollAll(-1);
}

};
