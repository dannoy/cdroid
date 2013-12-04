#ifndef _CDROIDRUNTIME_SERVICE_H
#define _CDROIDRUNTIME_SERVICE_H

#include <cutils/list.h>
#include <sys/stat.h>

struct service{
    struct listnode slist;

    const char *name;
    const char *file;
    time_t time_started;    /* time of last start */
    pid_t pid;
    gid_t gid;
    gid_t supp_gids[10];
    struct socketinfo *sockets;
    const char *args[3];
};
struct socketinfo {
    struct socketinfo *next;
    const char *name;
    const char *type;
    uid_t uid;
    gid_t gid;
    int perm;
};

extern int service_init();

#endif
