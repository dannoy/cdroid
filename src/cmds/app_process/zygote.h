#ifndef _APP_PROCESS_ZYGOTE_H
#define _APP_PROCESS_ZYGOTE_H

namespace cdroid {

class Zygote {
public:
    static int init(int argc, char *argv[], int fd);
    static int registerZygoteSocket();
    static int runZygoteLoop();
    static int handleZygoteClient(int);

private:
    static int mZygoteSocket;
    static int mArgc;
    static char **mArgv;
};

};

#endif
