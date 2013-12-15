#ifndef _SERVICE_RECEIVERHUB_H
#define _SERVICE_RECEIVERHUB_H

#include <runtime/Intent.h>
#include <runtime/Looper.h>
#include <runtime/IIntentReceiver.h>
#include <runtime/ReceiverInfo.h>
#include "ProcessRecord.h"
#include <service/PackageManager.h>

namespace cdroid {

class ActivityManagerService;
class ReceiverHub;
class ReceiverList;

class BroadcastFilter : public IntentFilter {
public:
    BroadcastFilter(sp<IntentFilter> filter, sp<ReceiverList> rl);
    /*private:*/
    sp<ReceiverList> mReceiverList;
};

class ReceiverList : public Vector<sp<BroadcastFilter> >, public RefBase {
public:
    ReceiverList(sp<ReceiverHub> hub, sp<ProcessRecord> app, int pid, int uid, sp<IIntentReceiver> receiver);

    /*private:*/
    sp<ReceiverHub> mHub;
    sp<ProcessRecord> mApp;
    int mPid;
    int mUid;
    sp<IIntentReceiver> mReceiver;

};

class BroadcastRecord : public RefBase {
public:
    BroadcastRecord(sp<Intent> intent, sp<ProcessRecord> callerApp, Vector<sp<BroadcastFilter> > &receivers, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid);
    BroadcastRecord(sp<Intent> intent, sp<ProcessRecord> callerApp, Vector<sp<ReceiverInfo> > &receivers, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid);

    void addReceiverInfos(Vector<sp<ReceiverInfo> > &receivers);
    void addBroadcastFilters(Vector<sp<BroadcastFilter> > &receivers);



    /*private:*/
    sp<Intent> mIntent;
    sp<ProcessRecord> mApp;
    sp<ProcessRecord> mCurApp;
    sp<Bundle> mMap;
    bool mSerialized;
    bool mSticky;
    int mPid;
    int mUid;

    Vector<sp<BroadcastFilter> > mBroadcastFilter;
    Vector<sp<ReceiverInfo> > mReceiverInfo;


};

class BroadcastMessageQueue : public RefBase {
public:
    BroadcastMessageQueue(sp<ActivityManagerService> ams, String8 name);
    void enqueueParallelBroadcastLocked(sp<BroadcastRecord> r);
    void enqueueOrderedBroadcastLocked(sp<BroadcastRecord> r);
    void scheduleBroadcastLocked();
    void processNextBroadcast(bool fromMsg);
    void processBroadcast(sp<BroadcastRecord> r);
    void processCurBroadcastLocked(sp<BroadcastRecord> r, sp<ProcessRecord> app);
    int attachApplicationLocked(sp<ProcessRecord> app);

private:
    class H : public Handler {
    public:
        H(sp<BroadcastMessageQueue> queue, sp<Looper> looper)
            : mQueue(queue),
              Handler(looper)
        {
        }
        enum {
            ON_SCHEDULE_BROADCAST = 1,
        };
        virtual void handleMessage(const sp<Message>& message);
    private:
        sp<BroadcastMessageQueue> mQueue;
    };
private:
    sp<ActivityManagerService> mAMS;
    String8 mName;
    Vector<sp<BroadcastRecord> > mParallelBroadcasts;
    Vector<sp<BroadcastRecord> > mOrderedBroadcasts;
    
    sp<Handler> mHandler;

    bool mBroadcastScheduled;

    sp<BroadcastRecord> mPendingRecord;
    int mLastPendingIndex;
};

class ReceiverHub : public RefBase
{
public:
    ReceiverHub(sp<ActivityManagerService> ams);
    int registerReceiverLocked(sp<IApplicationThread> caller, sp<IIntentReceiver> receiver, sp<IntentFilter> filter, int pid, int uid);
    int broadcastIntentLocked(sp<IApplicationThread> caller, sp<Intent> intent, sp<IIntentReceiver> resultTo, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid);
    int attachApplicationLocked(sp<ProcessRecord> app);

private:
    sp<ActivityManagerService> mAMS;
    sp<PackageManager> mPM;
    sp<BroadcastMessageQueue> mQueue;
    map<sp<IBinder>, sp<ReceiverList> > mRegisteredReceivers;
    Vector<sp<BroadcastFilter> > mReceivers;

    Mutex mMutex;
};

};

#endif
