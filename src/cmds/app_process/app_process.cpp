#define LOG_TAG "ZYGOTE"

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

static int _ensure_single_instance()
{

    int pid_file = open("obj/app_process.pid", O_CREAT | O_RDWR, 0666);
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
    bool startZygote = false;
    bool startSystemServer = false;

    if(_ensure_single_instance() != 0) {
        return -1;
    }

    for(int i = 1; i < argc; ++i) {
        ALOGI("argv[%d] %s", i, argv[i]);
        std::string opt(argv[i]);
        if(opt == "--zygote") {
            startZygote = true;
        }
        else if(opt == "--start-system-server") {
            startSystemServer = true;
        }
    }

    if(startSystemServer) {
    }


    return ret;
}




