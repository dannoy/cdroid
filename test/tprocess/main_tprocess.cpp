/*
 * Author:lijin@routon
 * Date:20121225
 */

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

#include "TProcessService.h"

using namespace android;

class PoolThread : public Thread
{
public:
    PoolThread(bool isMain)
        : mIsMain(isMain)
    {
    }

protected:
    virtual bool threadLoop()
    {
        IPCThreadState::self()->joinThreadPool(mIsMain);
        return false;
    }

    const bool mIsMain;
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
    act.sa_mask = 0;
    act.sa_restorer = NULL;
    sigaction(SIGCHLD, &act, 0);
}

int main(int argc, char** argv)
{
    sp<ProcessState> proc(ProcessState::self());

    //signal_init();

    TProcessService::instantiate();

    ProcessState::self()->startThreadPool();

    /*
     * We start another one thread, so that we have 3 threads can
     * serve the request from clients now.
     */
    sp<Thread> t = new PoolThread(false);
    t->run("TProcess #3");

    IPCThreadState::self()->joinThreadPool();
    return 0;
}
