#define LOG_TAG "SystemServer"

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
#include <sys/file.h>

#include <iostream>

#include <service/SystemServer.h>
#include <runtime/Looper.h>
#include <runtime/Process.h>

#include "ActivityManagerService.h"
#include "PackageManagerService.h"


namespace cdroid {

class ServerThread : public Thread {
public:
    ServerThread()
        :Thread(false)
    {
    }
    virtual ~ServerThread()
    {
    }

    virtual bool threadLoop(){
        Looper::prepareMainLooper();

    ALOGE("SystemServer threadloop0 %d", Process::myPid());
        ActivityManagerService::main();

    ALOGE("SystemServer threadloop1 %d", Process::myPid());
        PackageManagerService::instantiate();

    ALOGE("SystemServer threadloop2 %d", Process::myPid());
        ActivityManagerService::setSystemProcess();

    ALOGE("SystemServer threadloop3 %d", Process::myPid());
        ActivityManagerService::self()->systemReady();
    ALOGE("SystemServer threadloop4 %d", Process::myPid());

        Looper::loop();

        return true;
    }
};

int SystemServer::main(int argc, char *argv[])
{
    ALOGE("SystemServer start %d", Process::myPid());

    /*
     * Create a thread to enable handle binder request while
     * we are starting servers
     */
    sp<ServerThread> thread = new ServerThread;
    ALOGE("SystemServer start2 %d", Process::myPid());
    thread->run();
    ALOGE("SystemServer start3 %d", Process::myPid());

    android::ProcessState::self()->startThreadPool();
    ALOGE("SystemServer start4 %d", Process::myPid());
    android::IPCThreadState::self()->joinThreadPool();
    ALOGE("SystemServer stop %d", Process::myPid());

    return 0;
}

};
