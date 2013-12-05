#define LOG_TAG "Zygote"
#include <utils/Log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdlib.h>
#include <errno.h>

#include "zygote.h"
#include "runtime/Looper.h"

#define ZYGOTE_SOCKET_ENV "cdroid_service_socket_zygote"

namespace cdroid {

using android::String8;

int Zygote::mZygoteSocket = -1;

int Zygote::registerZygoteSocket()
{
    int ret = 0;

    char *cSocket = getenv(ZYGOTE_SOCKET_ENV);
    ALOGI("zygote socket fd %s",cSocket);

    mZygoteSocket = atoi(cSocket);

    return ret;
}

int Zygote::handleZygoteClient(int fd)
{
    vector<string> arg;
}

class ZygoteSocketLooperCallback : public LooperCallback {
    virtual int handleEvent(int fd, int events, void* data){
        union {
            struct sockaddr address;
            struct sockaddr_un un_address;
        } sa;
        socklen_t addrlen;
        int cfd;

        ALOGI("zygote socket fd %d event %d",fd, events);
        do {
            addrlen = sizeof(sa);
            cfd = accept(fd, &(sa.address), &addrlen);
        } while (cfd < 0 && errno == EINTR);

        if(cfd < 0) {
            ALOGE("zygote client socket %d < 0",fd, events);

            return cfd;
        }

        Zygote::handleZygoteClient(cfd);
    }
};

int Zygote::runZygoteLoop()
{
    int ret = 0;

    sp<Looper> l = Looper::myLooper();

    ::listen(mZygoteSocket, 5);
    l->addFd(mZygoteSocket, LOOPER_EVENT_INPUT, new ZygoteSocketLooperCallback, &mZygoteSocket);

    Looper::loop();


    return ret;
}

};
