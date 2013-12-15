#define LOG_TAG "ReceiverHub"
#include <cutils/log.h>


#include "ReceiverHub.h"
#include "ActivityManagerService.h"

namespace cdroid {

class BroadcastThread : public Thread {
public:
    sp<Looper> mLooper;
    Condition *mCond;
    BroadcastThread(Condition* cond)
        :Thread(false)
    {
        mCond = cond;
    }
    virtual ~BroadcastThread()
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

ReceiverList::ReceiverList(sp<ReceiverHub> hub, sp<ProcessRecord> app, int pid, int uid, sp<IIntentReceiver> receiver)
    : mHub(hub),
    mApp(app),
    mPid(pid),
    mUid(uid),
    mReceiver(receiver)
{
}

BroadcastFilter::BroadcastFilter(sp<IntentFilter> filter, sp<ReceiverList> rl)
    : IntentFilter(filter),
    mReceiverList(rl)
{
}

BroadcastRecord::BroadcastRecord(sp<Intent> intent, sp<ProcessRecord> callerApp, Vector<sp<BroadcastFilter> > &receivers, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid)
    : mIntent(intent),
    mApp(callerApp),
    mMap(map),
    mSerialized(serialized),
    mSticky(sticky),
    mPid(pid),
    mUid(uid)
{
    for(Vector<sp<BroadcastFilter> >::iterator it = receivers.begin(); it != receivers.end(); ++it) {
        mBroadcastFilter.push_back(*it);
    }
}

BroadcastRecord::BroadcastRecord(sp<Intent> intent, sp<ProcessRecord> callerApp, Vector<sp<ReceiverInfo> > &receivers, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid)
    : mIntent(intent),
    mApp(callerApp),
    mMap(map),
    mSerialized(serialized),
    mSticky(sticky),
    mPid(pid),
    mUid(uid)
{
    for(Vector<sp<ReceiverInfo> >::iterator it = receivers.begin(); it != receivers.end(); ++it) {
        mReceiverInfo.push_back(*it);
    }
}


void BroadcastRecord::addReceiverInfos(Vector<sp<ReceiverInfo> > &receivers)
{
    for(Vector<sp<ReceiverInfo> >::iterator it = receivers.begin(); it != receivers.end(); ++it) {
        mReceiverInfo.push_back(*it);
    }
}

void BroadcastRecord::addBroadcastFilters(Vector<sp<BroadcastFilter> > &receivers)
{
    for(Vector<sp<BroadcastFilter> >::iterator it = receivers.begin(); it != receivers.end(); ++it) {
        mBroadcastFilter.push_back(*it);
    }
}

void BroadcastMessageQueue::H::handleMessage(const sp<Message>& message)
{
    switch(message->what) {
        case ON_SCHEDULE_BROADCAST:
            {
                mQueue->processNextBroadcast(true);
            }
            break;
    }
}

BroadcastMessageQueue::BroadcastMessageQueue(sp<ActivityManagerService> ams, String8 name)
    : mAMS(ams),
    mName(name),
    mBroadcastScheduled(false)
{
    Condition* cond = new Condition;
    Mutex mutex;
    BroadcastThread* thr = new BroadcastThread(cond);;
    thr->run();
    while(thr->mLooper.get() == NULL) {
        cond->wait(mutex);
    };

    mHandler = new H(this, thr->mLooper);
}

void BroadcastMessageQueue::enqueueParallelBroadcastLocked(sp<BroadcastRecord> r)
{
    mParallelBroadcasts.push_back(r);
}
void BroadcastMessageQueue::enqueueOrderedBroadcastLocked(sp<BroadcastRecord> r)
{
    mOrderedBroadcasts.push_back(r);
}

void BroadcastMessageQueue::scheduleBroadcastLocked()
{
    sp<Message> msg = new Message(H::ON_SCHEDULE_BROADCAST, this);

    mHandler->sendMessage(msg);
}

void BroadcastMessageQueue::processCurBroadcastLocked(sp<BroadcastRecord> r, sp<ProcessRecord> app)
{
    app->thread->scheduleReceiver(r->mIntent, r->mMap, false, r->mSticky);
    scheduleBroadcastLocked();
}

void BroadcastMessageQueue::processBroadcast(sp<BroadcastRecord> r)
{
    for(Vector<sp<BroadcastFilter> >::iterator it = r->mBroadcastFilter.begin(); it != r->mBroadcastFilter.end(); ++it) {
        sp<BroadcastFilter> bf = *it;
        bf->mReceiverList->mReceiver->performReceive(r->mIntent, r->mMap, false, r->mSticky);
    }

    r->mBroadcastFilter.clear();

    int i = 0;
    for(Vector<sp<ReceiverInfo> >::iterator it = r->mReceiverInfo.begin(); it != r->mReceiverInfo.end(); ++it) {
        if(i <= mLastPendingIndex) {
            continue;
        }

        sp<ReceiverInfo> ri = *it;
        sp<ProcessRecord> app = mAMS->getProcessRecordLocked(ri->mApplicationName);

        if(app == NULL) {
            r->mCurApp = app = mAMS->startProcessLocked(ri->mApplicationName);
            mPendingRecord = r;
            mLastPendingIndex = it - r->mReceiverInfo.begin();
        } else {
            processCurBroadcastLocked(r, app);
        }

    }

    mLastPendingIndex = -1;
}

int BroadcastMessageQueue::attachApplicationLocked(sp<ProcessRecord> app)
{
    if(mPendingRecord != NULL && mPendingRecord->mCurApp->pid == app->pid) {
        mPendingRecord = NULL;
        processCurBroadcastLocked(mPendingRecord, app);
        return 0;
    }

    return -1;
}

void BroadcastMessageQueue::processNextBroadcast(bool fromMsg)
{
    if(fromMsg) {
        mBroadcastScheduled = false;
    }

    while(mParallelBroadcasts.size() > 0) {
        sp<BroadcastRecord> r = mParallelBroadcasts[0];
        processBroadcast(r);
        mParallelBroadcasts.removeAt(0); // TODO: free the memory after remove
    }

    //TODO:handle ordered message

}

ReceiverHub::ReceiverHub(sp<ActivityManagerService> ams)
    : mAMS(ams)
{
    mPM = new PackageManager();
    mQueue = new BroadcastMessageQueue(ams, String8("BROADCAST QUEUE"));
}

int ReceiverHub::registerReceiverLocked(sp<IApplicationThread> caller, sp<IIntentReceiver> receiver, sp<IntentFilter> filter, int pid, int uid)
{
    AutoMutex _l(mMutex);

    sp<ProcessRecord> callerApp = mAMS->getProcessRecordLocked(caller);

    if(callerApp == NULL) {
        ALOGE("ERROR: callerApp == NULL with pid %d", pid);
        return -1;
    }

    map<sp<IBinder>, sp<ReceiverList> >::iterator it =  mRegisteredReceivers.find(receiver->asBinder());
    sp<ReceiverList> rl;
    if(it == mRegisteredReceivers.end()) {
        rl = new ReceiverList(this, callerApp, pid, uid, receiver);
        mRegisteredReceivers.insert(pair<sp<IBinder>, sp<ReceiverList> >(receiver->asBinder(), rl));

    } else {
        rl = it->second;
    }

    sp<BroadcastFilter> bf = new BroadcastFilter(filter, rl);
    rl->push_back(bf);
    mReceivers.push_back(bf);

    return 0;
}

int ReceiverHub::broadcastIntentLocked(sp<IApplicationThread> caller, sp<Intent> intent, sp<IIntentReceiver> resultTo, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid)
{
    AutoMutex _l(mMutex);

    sp<ProcessRecord> callerApp = mAMS->getProcessRecordLocked(caller);

    if(callerApp == NULL) {
        ALOGE("ERROR: callerApp == NULL");
        return -1;
    }

    Vector<sp<BroadcastFilter> > registeredReceivers;
    for(Vector<sp<BroadcastFilter> >::iterator it = mReceivers.begin(); it != mReceivers.end(); ++it) {
        if((*it)->matchIntent(intent)) {
            registeredReceivers.push_back(*it);
        }
    }

    Vector<sp<ReceiverInfo> > *receivers = mPM->resolveIntentReceivers(intent);

    sp<BroadcastRecord> r = new BroadcastRecord(intent,callerApp, registeredReceivers, map, serialized, sticky, pid, uid);
    if(serialized) {
        if(receivers) {
            r->addReceiverInfos(*receivers);
        }
        mQueue->enqueueOrderedBroadcastLocked(r);
    } else {
        sp<BroadcastRecord> r2 = new BroadcastRecord(intent,callerApp, *receivers, map, serialized, sticky, pid, uid);
        mQueue->enqueueParallelBroadcastLocked(r);
        mQueue->enqueueParallelBroadcastLocked(r2);
    }

    mQueue->scheduleBroadcastLocked();
}

int ReceiverHub::attachApplicationLocked(sp<ProcessRecord> app)
{
    AutoMutex _l(mMutex);

    return mQueue->attachApplicationLocked(app);
}

};
