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

#include "ActivityManagerService.h"


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

        ActivityManagerService::setSystemProcess();


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

    return 0;
}

};
