#define LOG_TAG "CDROIDRUNTIME"
#include <cutils/log.h>

#define SYSTEM_BIN_DIR "bin"
#define SYSTEM_LIB_DIR "lib"
#define SYSTEM_SOCKET_DIR "bin/runtime/dev/socket"
#define SYSTEM_SOCKET_ENV_PREFIX "cdroid_service_socket_"

#include <cutils/list.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "service.h"


static struct listnode svc_list;


struct service service_manager = {
    {NULL,NULL},
    "servicemanager",
    SYSTEM_BIN_DIR"/servicemanager",
    0,
    0,
    0,
    {0},
    NULL,
    {0}
};

struct socketinfo zygote_socket = {
    NULL,
    "zygote",
    "stream",
    0,0,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
};

struct service zygote = {
    {NULL,NULL},
    "zygote",
    SYSTEM_BIN_DIR"/app_process",
    0,
    0,
    0,
    {0},
    &zygote_socket,
    {"--zygote", "--start-system-server", 0}
};

struct service *svcs[] = {&service_manager, &zygote};
static const char *ENV[32];

/* add_environment - add "key=value" to the current environment */
int add_environment(const char *key, const char *val)
{
    int n;

    for (n = 0; n < 31; n++) {
        if (!ENV[n]) {
            size_t len = strlen(key) + strlen(val) + 2;
            char *entry = (char *)malloc(len);
            snprintf(entry, len, "%s=%s", key, val);
            ENV[n] = entry;
            return 0;
        }
    }

    return 1;
}
/*
 * gettime() - returns the time in seconds of the system's monotonic clock or
 * zero on error.
 */
time_t gettime(void)
{
    struct timespec ts;
    int ret;

    ret = clock_gettime(CLOCK_MONOTONIC, &ts);
    if (ret < 0) {
        ALOGE("clock_gettime(CLOCK_MONOTONIC) failed: %s\n", strerror(errno));
        return 0;
    }

    return ts.tv_sec;
}
/*
 * create_socket - creates a Unix domain socket in ANDROID_SOCKET_DIR
 * ("/dev/socket") as dictated in init.rc. This socket is inherited by the
 * daemon. We communicate the file descriptor's value via the environment
 * variable SYSTEM_SOCKET_ENV_PREFIX<name> ("ANDROID_SOCKET_foo").
 */
int create_socket(const char *name, int type, mode_t perm, uid_t uid, gid_t gid)
{
    struct sockaddr_un addr;
    int fd, ret;
    char *secon;

    fd = socket(PF_UNIX, type, 0);
    if (fd < 0) {
        ALOGE("Failed to open socket '%s': %s\n", name, strerror(errno));
        return -1;
    }

    memset(&addr, 0 , sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), SYSTEM_SOCKET_DIR"/%s",
             name);

    ret = unlink(addr.sun_path);
    if (ret != 0 && errno != ENOENT) {
        ALOGE("Failed to unlink old socket '%s': %s\n", name, strerror(errno));
        goto out_close;
    }


    ret = bind(fd, (struct sockaddr *) &addr, sizeof (addr));
    if (ret) {
        ALOGE("Failed to bind socket '%s': %s\n", name, strerror(errno));
        goto out_unlink;
    }

    chown(addr.sun_path, uid, gid);
    chmod(addr.sun_path, perm);

    ALOGI("Created socket '%s' with mode '%o', user '%d', group '%d'\n",
         addr.sun_path, perm, uid, gid);

    return fd;

out_unlink:
    unlink(addr.sun_path);
out_close:
    close(fd);
    return -1;
}

static void publish_socket(const char *name, int fd)
{
    char key[64] = SYSTEM_SOCKET_ENV_PREFIX;
    char val[64];

    strncpy(key + sizeof(SYSTEM_SOCKET_ENV_PREFIX) - 1,
            name,
            sizeof(key) - sizeof(SYSTEM_SOCKET_ENV_PREFIX));
    snprintf(val, sizeof(val), "%d", fd);
    add_environment(key, val);

    /* make sure we don't close-on-exec */
    fcntl(fd, F_SETFD, 0);
}

void service_start(struct service *svc)
{
    struct stat s;
    pid_t pid;
    int n;
    int rc;

    if (stat(svc->file, &s) != 0) {
        ALOGE("cannot find '%s', disabling '%s'\n", svc->file, svc->name);
        return;
    }

    ALOGI("starting '%s'\n", svc->name);

    pid = fork();

    if (pid == 0) {
        struct socketinfo *si;
        struct svcenvinfo *ei;
        char tmp[32];
        int fd, sz;

        umask(077);

        for (si = svc->sockets; si; si = si->next) {
            int socket_type = (
                    !strcmp(si->type, "stream") ? SOCK_STREAM :
                        (!strcmp(si->type, "dgram") ? SOCK_DGRAM : SOCK_SEQPACKET));
            int s = create_socket(si->name, socket_type,
                                  si->perm, si->uid, si->gid);
            if (s >= 0) {
                publish_socket(si->name, s);
            }
        }

        setpgid(0, getpid());

    /* as requested, set our gid, supplemental gids, and uid */
        if (svc->gid) {
            if (setgid(svc->gid) != 0) {
                ALOGE("setgid failed: %s\n", strerror(errno));
                _exit(127);
            }
        }
        const char *arg_ptrs[10];
        arg_ptrs[0] = svc->file;
        arg_ptrs[1] = svc->args[0];
        arg_ptrs[2] = svc->args[1];
        arg_ptrs[3] = '\0';
        if (execve(svc->file, (char**) arg_ptrs, (char**) ENV) < 0) {
            ALOGE("cannot execve('%s'): %s\n", svc->args[0], strerror(errno));
        }
        _exit(127);
    }

    if (pid < 0) {
        ALOGE("failed to start '%s'\n", svc->name);
        svc->pid = 0;
        return;
    }

    svc->time_started = gettime();
    svc->pid = pid;
}

void service_for_each(void (*func)(struct service *svc))
{
    struct listnode *node;
    struct service *svc;
    list_for_each(node, &svc_list) {
        svc = node_to_item(node, struct service, slist);
        func(svc);
    }
}

int service_init()
{
    list_init(&svc_list);
    add_environment("LD_LIBRARY_PATH", "lib");
    int N = sizeof(svcs)/sizeof(svcs[0]);
    for (int i = 0; i < N; ++i) {
        list_add_tail(&svc_list, &svcs[i]->slist);
    }
    service_for_each(service_start);
}
