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
#include "runtime/Process.h"
#include <cutils/process_name.h>

namespace cdroid {
static int gPidFile = -1;

int startSystemServer(int argc, char *argv[])
{
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        close(gPidFile);
        for(int i = 1; i < argc; ++i) {
            memset(argv[i], 0, strlen(argv[i]));
        }
        set_process_name("SystemServer", argv);
        ALOGE("SystemServer stopped!!!");
        {
            Vector<int> gids;
            Vector<String8> args;
            String8 name("testProg");
            Process::startViaZygote(name, 0, 0, gids, args);
        }
        SystemServer::main(argc, argv);
        exit(-1);
        ALOGE("SystemServer forbidden path!!!");
    }

    return 0;
}

};

static int _ensure_single_instance(const char *id)
{

    char buf[128];
    snprintf(buf,sizeof(buf),"obj/%s.pid",id);
    cdroid::gPidFile = open(buf, O_CREAT | O_RDWR, 0666);
    int rc = flock(cdroid::gPidFile, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno)
            return -1; // another instance is running
    }
    else {
            // this is the first instance
        return 0;
    }
}
static void sigchld_handler(int s)
{
    pid_t pid;
    int status;

    /* no block */
    while ( (pid = waitpid(-1, &status, WNOHANG)) == -1 && errno == EINTR );

    ALOGD("pid %d exit", pid);
}

static void signal_init(void)
{
    int s[2];

    struct sigaction act;

    act.sa_handler = sigchld_handler;
    act.sa_flags = SA_NOCLDSTOP;
    //act.sa_mask = {0};
    act.sa_restorer = NULL;
    sigaction(SIGCHLD, &act, 0);
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
    signal_init();

    cdroid::Looper::prepare();

    if(bStartSystemServer) {
        cdroid::startSystemServer(argc, argv);
    }

    if(bStartZygote) {
        cdroid::Zygote::init(argc, argv, cdroid::gPidFile);
        cdroid::Zygote::registerZygoteSocket();
        cdroid::Zygote::runZygoteLoop();
        //android::ProcessState::self()->startThreadPool();
        //android::IPCThreadState::self()->joinThreadPool();
    }

    ALOGE("app_process exit unexpectedly!!");

    return ret;
}




