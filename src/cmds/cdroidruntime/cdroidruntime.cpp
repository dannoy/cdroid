#define LOG_TAG "CDroidRuntime"

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

#include "service.h"
#include "input.h"


namespace cdroid{


int system_init()
{
    int ret = 0;

    ret = service_init();

    return ret;
}


int io_init()
{
    int ret = 0;
    output_init();
    input_init();
    return ret;
}
};

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

int main()
{
    int ret = 0;
    android::ProcessState::self()->startThreadPool();

    signal_init();
    cdroid::system_init();
    cdroid::io_init();
    android::IPCThreadState::self()->joinThreadPool();

    return ret;
}




