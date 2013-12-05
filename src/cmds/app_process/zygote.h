#ifndef _APP_PROCESS_ZYGOTE_H
#define _APP_PROCESS_ZYGOTE_H

namespace cdroid {

class Zygote {
public:
    static int registerZygoteSocket();
    static int runZygoteLoop();
    static int handleZygoteClient(int);

private:
    static int mZygoteSocket;
};

};

#endif
