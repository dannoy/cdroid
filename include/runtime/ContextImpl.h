#ifndef _RUNTIME_CONTEXT_IMPL_H
#define _RUNTIME_CONTEXT_IMPL_H

#include <runtime/Context.h>
#include <runtime/ApplicationLoader.h>
#include <runtime/IServiceConnection.h>

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
private:
public:
    ServiceConnectionManager();
    sp<IServiceConnection> getServiceDispatcher(sp<ServiceConnection> conn, sp<Context> context, sp<Handler> handler, int flags);

private:
    Mutex mMutex;
    map<sp<Context>, map<sp<ServiceConnection>, sp<ServiceDispatcher> >*  > mServices;
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


    Mutex mCmdMutex;
};

};

#endif
