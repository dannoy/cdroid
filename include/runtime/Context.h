#ifndef _RUNTIME_CONTEXT_H
#define _RUNTIME_CONTEXT_H

#include <runtime/common.h>
#include <runtime/Intent.h>
#include <runtime/IntentFilter.h>

namespace cdroid {

class Context;

class ServiceConnection : public RefBase {
public:
    virtual void onServiceConnected(sp<ComponentName> name, sp<IBinder> service) = 0;
    virtual void onServiceDisconnected(sp<ComponentName> name) = 0;
};

class BroadcastReceiver : public RefBase {
public:
    virtual void onReceive(sp<Context> context, sp<Intent> intent) = 0;
};

class Context : public RefBase {
public:
    virtual int startActivity(sp<Intent> intent) = 0;
    virtual int startService(sp<Intent> intent) = 0;
    virtual int bindService(sp<Intent> intent, sp<ServiceConnection> conn) = 0;
    virtual int registerReceiver(sp<BroadcastReceiver> receiver, sp<IntentFilter> filter) = 0;
    virtual int sendBroadcast(sp<Intent> intent) = 0;
    virtual int execInternalCommand(String8 cmd) = 0;
};

class ContextWrapper : public Context {
public:
    int attachBaseContext(sp<Context> base);

    virtual int execInternalCommand(String8 cmd);
    virtual int startActivity(sp<Intent> intent);
    virtual int startService(sp<Intent> intent);
    virtual int bindService(sp<Intent> intent, sp<ServiceConnection> conn);
    virtual int registerReceiver(sp<BroadcastReceiver> receiver, sp<IntentFilter> filter);
    virtual int sendBroadcast(sp<Intent> intent);

private:
    sp<Context> mBase;
};

};

#endif
