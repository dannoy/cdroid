#define LOG_TAG "CDROIDRUNTIME"

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
    input_init();
    //output_init();
    return ret;
}
};

int main()
{
    int ret = 0;

    cdroid::system_init();
    cdroid::io_init();
    android::ProcessState::self()->startThreadPool();
    android::IPCThreadState::self()->joinThreadPool();

    return ret;
}




