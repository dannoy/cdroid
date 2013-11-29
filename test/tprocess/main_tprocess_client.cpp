#include <iostream>
#include <binder/IServiceManager.h>
#include <ITProcess_aidl.h>

using namespace std;
using namespace android;
using namespace com::routon::os;

int main(int argc, char *argv[])
{
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> const service = sm->checkService(String16("tprocess"));
    //sp<ITProcess> tp = interface_cast<ITProcess>(service);
    sp<ITProcess> tp = ITProcess::asInterface(service);

    String8 cmd(argv[1]);
    sp<TResult> res = tp->exec(cmd, false);

    cerr << res->mStatus << endl
         << "Stdout:" << endl
         << res->mStdoutStr << endl
         << "Stderr:" << endl
         << res->mStderrStr << endl;

    return 0;
}

