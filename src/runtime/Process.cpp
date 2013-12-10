#define LOG_TAG "Process"
#include <cutils/log.h>

#include <runtime/Process.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace cdroid {

#define ZYGOTE_SOCKET "obj/zygote"

int Process::startViaZygote(String8& niceName, int uid, int gid, Vector<int>& gids, Vector<String8>& extraArgs, pid_t *pid)
{
    Vector<String8> args;
    String8 niceNameArg("--nice-name=");
    String8 uidArg("--uid=");
    String8 gidArg("--gid=");
    String8 gidsArg("--gids=");


    niceNameArg += niceName;
    uidArg += String8::format("%d",uid);
    gidArg += String8::format("%d",gid);

    args.push_back(niceNameArg);
    args.push_back(uidArg);
    args.push_back(gidArg);

    for(int i = 0; i < gids.size(); ++i) {
        if(i != 0) {
            gidsArg += ",";
        }
        gidsArg += String8::format("%d", gids[i]);
    }
    args.push_back(gidsArg);

    for(int i = 0; i < extraArgs.size(); ++i) {
        args.push_back(extraArgs[i]);
    }

    return startViaZygoteArgs(args, pid);
}

int Process::startViaZygoteArgs(Vector<String8>& args, pid_t *pid)
{
    struct sockaddr_un addr;
    int fd, ret;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        ALOGE("Failed to open socket '%s': %s\n", ZYGOTE_SOCKET, strerror(errno));
        return -1;
    }

    memset(&addr, 0 , sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, ZYGOTE_SOCKET, sizeof(addr.sun_path));

    if(connect(fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
        ALOGE("connect to %s failed ", ZYGOTE_SOCKET);
    }

    char newline = '\n';

    for(int i = 0; i < args.size(); ++i) {
        write(fd, args[i].string(), strlen(args[i].string()) + 1);
        write(fd,&newline,1);
        //ALOGI("sendargs '%s'", args[i].string());
    }

    pid_t newPid;

    shutdown(fd, SHUT_WR);
    //ALOGI("client read pid- '%d'", newPid);
    read(fd, &newPid, sizeof(newPid));
    //ALOGI("client read pid '%d'", newPid);
    if(pid) {
        *pid = newPid;
    }

    close(fd);

    return 0;
}

pid_t Process::myPid()
{
    return getpid();
}

uid_t Process::myUid()
{
    return getuid();
}

uid_t Process::myEUid()
{
    return geteuid();
}

};
