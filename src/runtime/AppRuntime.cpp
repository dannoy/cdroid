#define LOG_TAG "AppRuntime"
#include <cutils/log.h>
#include <cutils/process_name.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>

#include <runtime/Process.h>
#include <runtime/AppRuntime.h>
#include <runtime/ActivityThread.h>

namespace cdroid {

using android::ProcessState;
using android::IPCThreadState;

int AppRuntime::main(Vector<String8>& args, int argc, char *argv[])
{
    Vector<String8> activityArgs;
    const char *assign = "=";
    for(int i = 0; i < args.size(); ++i) {
        int pos = args[i].find(assign);
        if(pos == -1) {
            activityArgs.push_back(args[i]);
        }
        else if(pos == 11) { // --nice-name
            const char *value = args[i].string() + pos + 1;
            set_process_name(value, argv);
            //ALOGI("set name %s", value);
        }
        else if(pos == 5) {
            const char *value = args[i].string() + pos + 1;

            if(0 == strncmp(args[i].string(), "--uid", 5)) {
                //ALOGI("set uid %s", value);
            }
            else if(0 == strncmp(args[i].string(), "--gid", 5)) {
                //ALOGI("set gid %s", value);
            }
        }
        else if(pos == 6) { // --gids
            const char *value = args[i].string() + pos + 1;
            //ALOGI("set gids %s", value);
        }

        //ALOGI("'%s' pos %d", args[i].string(), pos);
    }

    onStart();

    ActivityThread::main(activityArgs);
}

void AppRuntime::onStart()
{
    sp<ProcessState> proc = ProcessState::self();
    ALOGI("Process %s: onStart.\n",get_process_name());
    proc->startThreadPool();
}

void AppRuntime::onExit()
{
    IPCThreadState::self()->stopProcess();
}


};



