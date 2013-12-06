#define LOG_TAG "Zygote"

#include <utils/Log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <errno.h>

#include "zygote.h"
#include <runtime/Looper.h>
#include <runtime/AppRuntime.h>

#define ZYGOTE_SOCKET_ENV "cdroid_service_socket_zygote"

namespace cdroid {


int Zygote::mZygoteSocket = -1;
int Zygote::mArgc = -1;
char** Zygote::mArgv = NULL;

int Zygote::registerZygoteSocket()
{
    int ret = 0;

    char *cSocket = getenv(ZYGOTE_SOCKET_ENV);
    //ALOGI("zygote socket fd %s",cSocket);

    mZygoteSocket = atoi(cSocket);

    return ret;
}

int Zygote::init(int argc, char *argv[], int fd)
{
    int ret = 0;

    mArgc = argc;
    mArgv = argv;
    close(fd);

    return ret;
}


int Zygote::handleZygoteClient(int fd)
{
    int ret = 0;
    char *line = NULL;
    size_t len;
    size_t read;
    FILE *fp = fdopen(fd, "r+");
    Vector<String8> args;
    while ((read = getline(&line, &len, fp)) != -1) {
        args.push_back(String8(line));
        //ALOGI("zygote read args %s",line);
    }


    pid_t pid;
    pid = fork();

    if (pid == 0) {
        for(int i = 1; i < mArgc; ++i) {
            memset(mArgv[i], 0, strlen(mArgv[i]));
        }

        return AppRuntime::main(args, mArgc, mArgv);
    }

    ret = write(fd, &pid, sizeof(pid));
    //ALOGI("zygote write pid %d", pid);

    return ret;
}

class ZygoteSocketLooperCallback : public LooperCallback {
    virtual int handleEvent(int fd, int events, void* data){
        union {
            struct sockaddr address;
            struct sockaddr_un un_address;
        } sa;
        socklen_t addrlen;
        int cfd;

        //ALOGI("zygote socket fd %d event %d",fd, events);
        do {
            addrlen = sizeof(sa);
            cfd = accept(fd, &(sa.address), &addrlen);
        } while (cfd < 0 && errno == EINTR);

        if(cfd < 0) {
            ALOGE("zygote client socket %d < 0",fd);

            return cfd;
        }

        Zygote::handleZygoteClient(cfd);

        // return 1 to poll next event
        return 1;
    }
};

int Zygote::runZygoteLoop()
{
    int ret = 0;

    sp<Looper> l = Looper::myLooper();

    ::listen(mZygoteSocket, 5);
    l->addFd(mZygoteSocket, LOOPER_EVENT_INPUT, new ZygoteSocketLooperCallback, &mZygoteSocket);

    Looper::loop();

    ALOGE("Zygote's loop exit unexpectedly!!");

    return ret;
}

};
