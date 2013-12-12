#define LOG_TAG "ActivityStack"
#include <cutils/log.h>

#include <service/PackageManager.h>

#include "ActivityStack.h"
#include "ProcessRecord.h"
#include "ActivityManagerService.h"

namespace cdroid {

class ActivityStackHandler : public Handler {
public:
    ActivityStackHandler(sp<Looper> looper)
        : Handler(looper)
    {
    }
    virtual void handleMessage(const sp<Message>& message) {
        switch(message->what) {
        }
    }
};

ActivityStack::ActivityStack(sp<ActivityManagerService> service, sp<Context> context, bool mainStack, sp<Looper> looper)
{
    mService = service;
    mContext = context;
    mMainStack = mainStack;
    mHandler = new ActivityStackHandler(looper);
    mPM = new PackageManager();
}

bool ActivityStack::resumeTopActivityLocked(sp<ActivityRecord> prev)
{
    sp<ActivityRecord> next = topRunningActivityLocked(NULL);

    if(next == NULL) {
        if(mMainStack) {
            mService->startHomeActivityLocked();
        }
    }

    return true;
}

sp<ActivityRecord> ActivityStack::topRunningActivityLocked(sp<ActivityRecord> notTop)
{
    return NULL;
}

int ActivityStack::indexOfTokenLocked(sp<IBinder> token)
{
    sp<ActivityRecord::Token> t = reinterpret_cast<ActivityRecord::Token *>(token != NULL ? interface_cast<IApplicationToken>(token).get() : NULL);
    if(t != NULL) {
        sp<ActivityRecord> r = t->mActivity;
        for(Vector<sp<ActivityRecord> >::iterator it = mHistory.begin(); it != mHistory.end(); ++it) {
            if(it->get() == r.get()) {
                return it - mHistory.begin();
            }
        }
    }

    return -1;
}


int ActivityStack::startActivityLocked(sp<IApplicationThread> caller, sp<Intent> intent, sp<IBinder> resultTo, int callingPid, int callingUid, int requestCode)
{
    sp<ProcessRecord> callerApp;
    if(caller != NULL) {
        callerApp = mService->getRecordForAppLocked(caller);
    }

    sp<ActivityRecord> sourceRecord;
    sp<ActivityRecord> resultRecord;

    if(resultTo != NULL) {
        int index = indexOfTokenLocked(resultTo);
        if(index >= 0) {
            sourceRecord = mHistory[index];
            if(requestCode >= 0) {
                resultRecord = sourceRecord;
            }
        }
    }

    sp<ActivityInfo> ai = mPM->resolveActivityInfo(intent->getAction());

    if(ai == NULL) {
        ALOGE("Cannot find any activity with %s", intent->getAction().string());
        return -1;
    }
    ALOGI("Found activity %s:%s define in %s with %s", ai->mApplicationName.string(),
                                                    ai->mName.string(),
                                                    ai->mFilename.string(),
                                                    intent->getAction().string());

    sp<ActivityRecord> r = new ActivityRecord(mService, this, ai, intent, callerApp, resultRecord);

    return startActivityLocked(r, sourceRecord);
}

int ActivityStack::startActivityLocked(sp<ActivityRecord> r, sp<ActivityRecord> sourceRecord)
{
    if(sourceRecord != NULL && sourceRecord->mApp != NULL
            && sourceRecord->mApp->thread != NULL) {
        sourceRecord->mApp->thread->schedulePauseActivity(sourceRecord->mToken.get());
    }

    sp<ProcessRecord> app = mService->getProcessRecordLocked(r->mActivityInfo->mApplicationName);

    // Add before start process
    mHistory.push_back(r);

    if(app == NULL) {
        mService->startProcessLocked(r->mActivityInfo->mApplicationName);
        return 0;
    }


    return realStartActivityLocked(r, app);
}

int ActivityStack::realStartActivityLocked(sp<ActivityRecord> r, sp<ProcessRecord> app)
{
    if(r == NULL && app != NULL) {
        for(Vector<sp<ActivityRecord> >::iterator it = mHistory.begin(); it != mHistory.end(); ++it) {
            if((*it)->mActivityInfo->mApplicationName == app->name) {
                r = *it;
                break;
            }
        }
    }

    if(r == NULL) {
        ALOGE("No activity request process pid %d", app->pid);
        Process::killProcessByPid(app->pid);
        return -1;
    }

    r->mApp = app;
    //ALOGI("Activity %s request process pid %d started", r->mActivityInfo->mName.string(), app->pid);

    app->thread->scheduleLaunchActivity(r->mActivityInfo, r->mToken, r->mIntent);
    return 0;
}

};
