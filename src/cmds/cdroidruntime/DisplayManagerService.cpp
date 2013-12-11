#define LOG_TAG "DisplayManagerService"
#include <cutils/log.h>

#include <time.h>

#include "DisplayManagerService.h"

namespace cdroid {
class DispThread : public Thread {
    public:
        sp<Looper> mLooper;
        Condition *mCond;
        DispThread(Condition* cond)
            :Thread(false)
        {
            mCond = cond;
        }
        virtual ~DispThread()
        {
        }

        virtual bool threadLoop(){
            Looper::prepare();

            mLooper = Looper::myLooper();
            //ALOGE("ActivityManagerService threadloop %d %p", Process::myPid(), mLooper.get());
            mCond->signal();

            Looper::loop();

            return true;
        }
};


DisplayManagerService::DisplayManagerService()
{
    Condition* cond = new Condition;
    Mutex mutex;
    DispThread* thr = new DispThread(cond);
    thr->run();

    while(thr->mLooper.get() == NULL) {
        cond->wait(mutex);
    };
    mLooper = thr->mLooper;
    mH = new H(this, mLooper);
}

void DisplayManagerService::instantiate()
{
    android::defaultServiceManager()->addService(android::String16("display"), new DisplayManagerService());
}

int DisplayManagerService::displayText(sp<Text> txt)
{
    sp<Message> msg = new Message(H::DISPLAY_TEXT, txt);
    mH->sendMessage(msg);

    return 0;
}

void DisplayManagerService::handleDisplayTextLocked(sp<Text> txt)
{
    //ALOGI("handleDisplayText %s", txt->getCharSequence());
    fprintf(stderr,"[%s]", txt->getCharSequence());
}

void DisplayManagerService::H::handleMessage(const sp<Message>& message)
{
    switch(message->what) {
        case DISPLAY_TEXT:
            {
                sp<Text> txt = reinterpret_cast<Text*>(message->obj.get());
                mDisp->handleDisplayTextLocked(txt);
            }
            break;
    }
}

};
