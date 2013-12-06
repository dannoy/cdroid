#define LOG_TAG "AppRuntime"
#include <cutils/log.h>
#include <cutils/process_name.h>

#include <runtime/Process.h>
#include <runtime/AppRuntime.h>
#include <runtime/ActivityThread.h>

namespace cdroid {


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

int AppRuntime::onStart()
{
    sp<ProcessState> proc = ProcessState::self();
    ALOGV("App process: starting thread pool.\n");
    proc->startThreadPool();
}

virtual void onExit(int code)
{
    if (mClassName == NULL) {
        IPCThreadState::self()->stopProcess();
    }

    AndroidRuntime::onExit(code);
}


};



