#define LOG_TAG "SystemServer"

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
#include <sys/file.h>

#include <iostream>

#include "service/SystemServer.h"


namespace cdroid {

int SystemServer_Run()
{
    android::ProcessState::self()->startThreadPool();
    //Looper.loop();

    return 0;
}

};
