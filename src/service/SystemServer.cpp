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
#include "WindowManagerService.h"
#include "DisplayManagerService.h"


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

        ActivityManagerService::main();

        PackageManagerService::instantiate();
        WindowManagerService::instantiate();
        DisplayManagerService::instantiate();

        ActivityManagerService::setSystemProcess();

        ActivityManagerService::self()->systemReady();

        Looper::loop();

        return true;
    }
};

int SystemServer::main(int argc, char *argv[])
{
    /*
     * Create a thread to enable handle binder request while
     * we are starting servers
     */
    sp<ServerThread> thread = new ServerThread;
    thread->run();

    android::ProcessState::self()->startThreadPool();
    android::IPCThreadState::self()->joinThreadPool();
    ALOGE("SystemServer stop %d", Process::myPid());

    return 0;
}

};
