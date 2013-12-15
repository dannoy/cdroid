#ifndef _RUNTIME_CONTEXT_IMPL_H
#define _RUNTIME_CONTEXT_IMPL_H

#include <runtime/Context.h>
#include <runtime/ApplicationLoader.h>
#include <runtime/IServiceConnection.h>
#include <runtime/IIntentReceiver.h>

namespace cdroid {

class ActivityThread;

class ServiceDispatcher : public RefBase {
public:
    ServiceDispatcher(sp<ServiceConnection> conn, sp<Context> context, sp<Handler> mainHandler);
    sp<IServiceConnection> getIServiceConnection();

private:
    class RunConnection : public Runnable {
    public:
        RunConnection(sp<ServiceConnection> conn, sp<ComponentName> comp, sp<IBinder> service, int what)
            : mComp(comp),
            mConnection(conn),
            mService(service),
            mWhat(what)
        {
        }

        virtual int run()
        {
            /*ALOGI("TODO post run connection");*/
            mConnection->onServiceConnected(mComp, mService);
        }

        sp<ComponentName> mComp;
        sp<IBinder> mService;
        sp<ServiceConnection> mConnection;
        int mWhat;
    };
    class InnerServiceConnection : public BnServiceConnection {
    public:
        InnerServiceConnection(sp<ServiceConnection> conn, sp<Handler> handler);
        virtual void connected(sp<ComponentName> name, sp<IBinder> service);
    private:
        sp<Handler> mMainThreadHandler;
        sp<ServiceConnection> mConnection;
    };

private:
    sp<InnerServiceConnection> mIServiceConnection;
    sp<Handler> mMainThreadHandler;
    sp<Context> mContext;
    sp<ServiceConnection> mConnection;
};

class ServiceConnectionManager : public RefBase {
public:
    ServiceConnectionManager();
    sp<IServiceConnection> getServiceDispatcher(sp<ServiceConnection> conn, sp<Context> context, sp<Handler> handler, int flags);

private:
    Mutex mMutex;
    map<sp<Context>, map<sp<ServiceConnection>, sp<ServiceDispatcher> >*  > mServices;
};

class ReceiverDispatcher : public RefBase {
public:
    ReceiverDispatcher(sp<BroadcastReceiver> receiver, sp<Context> context, sp<Handler> mainHandler);
    sp<IIntentReceiver> getIIntentReceiver();

private:
    class InnerIntentReceiver: public BnIntentReceiver {
    public:
        InnerIntentReceiver(sp<BroadcastReceiver> receiver, sp<Context> context, sp<Handler> mainHandler);
        virtual void performReceive(sp<Intent> intent, sp<Bundle> extra, bool ordered, bool sticky);
    private:
        sp<Handler> mMainThreadHandler;
        sp<BroadcastReceiver> mReceiver;
        sp<Context> mContext;
    };

private:
    sp<BroadcastReceiver> mReceiver;
    sp<Context> mContext;
    sp<Handler> mMainHandler;
    sp<InnerIntentReceiver> mIntentReceiver;
};

class ReceiverManager : public RefBase {
public:
    ReceiverManager();
    sp<IIntentReceiver> getReceiverDispatcher(sp<BroadcastReceiver> receiver, sp<Context> context, sp<Handler> handler);

private:
    Mutex mMutex;
    map<sp<Context>, map<sp<BroadcastReceiver>, sp<ReceiverDispatcher> >*  > mReceivers;
};


class ContextImpl : public Context {
public:
    // init methos
    ContextImpl();
    static ContextImpl* createSystemContext(ActivityThread *mainThread);
    void init(ActivityManifest* amf, sp<IBinder> token, sp<ActivityThread> thread, sp<Looper> cmdLooper);
    void init(ServiceManifest* smf, sp<IBinder> token, sp<ActivityThread> thread);
    void setOuterContext(sp<Context> context);
    sp<Context> getOuterContext();

    // Context interfaces
    virtual int execInternalCommand(String8 cmd);
    virtual int startActivity(sp<Intent> intent);
    virtual int startService(sp<Intent> intent);
    virtual int bindService(sp<Intent> intent, sp<ServiceConnection> conn);
    virtual int registerReceiver(sp<BroadcastReceiver> receiver, sp<IntentFilter> filter);
    virtual int sendBroadcast(sp<Intent> intent);


private:
    class H : public Handler {
    public:
        H(sp<ContextImpl> ctx, sp<Looper> looper)
            : mContext(ctx),
              Handler(looper)
        {
        }
        enum {
            ON_CMD = 1,
        };
        virtual void handleMessage(const sp<Message>& message);
    private:
        sp<ContextImpl> mContext;
    };
private:
    sp<Handler> getCmdHandler();
    int handleInternalCommand(String8 cmd);

private:

    ActivityManifest* mActivityMF;
    ServiceManifest* mServiceMF;
    sp<IBinder> mToken;
    sp<ActivityThread> mThread;
    sp<Context> mOuterContext;

    sp<Looper> mCmdLooper;
    sp<Handler> mCmdHandler;
    sp<ServiceConnectionManager> mServiceConnectionMgr;
    sp<ReceiverManager> mReceiverMgr;


    Mutex mCmdMutex;
};

};

#endif
