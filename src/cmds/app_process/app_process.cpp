#define LOG_TAG "Zygote"

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

#include "service/SystemServer.h"
#include "zygote.h"
#include "runtime/Looper.h"

namespace cdroid {

int startSystemServer()
{
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        SystemServer_Run();
        ALOGE("SystemServer stopped!!!");
        exit(-1);
    }

    return 0;
}

};

static int _ensure_single_instance(const char *id)
{

    char buf[128];
    snprintf(buf,sizeof(buf),"obj/%s.pid",id);
    int pid_file = open(buf, O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno)
            return -1; // another instance is running
    }
    else {
            // this is the first instance
        return 0;
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;
    ALOGI("app_process started");
    bool bStartZygote = false;
    bool bStartSystemServer = false;

    if(_ensure_single_instance("app_process") != 0) {
        ALOGE("app_process has already started, stop!!");
        return -1;
    }



    for(int i = 1; i < argc; ++i) {
        //ALOGI("argv[%d] %s", i, argv[i]);
        std::string opt(argv[i]);
        if(opt == "--zygote") {
            bStartZygote = true;
        }
        else if(opt == "--start-system-server") {
            bStartSystemServer = true;
        }
    }

    cdroid::Looper::prepare();

    if(bStartSystemServer) {
        cdroid::startSystemServer();
    }

    if(bStartZygote) {
        cdroid::Zygote::registerZygoteSocket();
        cdroid::Zygote::runZygoteLoop();
        //android::ProcessState::self()->startThreadPool();
        //android::IPCThreadState::self()->joinThreadPool();
    }

    ALOGE("app_process exit unexpectedly!!");

    return ret;
}




