

#include <runtime/IApplicationThread.h>

namespace cdroid {

class BpApplicationThread : public BpInterface<IApplicationThread> {
public:
    BpApplicationThread(const sp<IBinder>& impl)
        : BpInterface<IApplicationThread>(impl)
    {
    }

};


IMPLEMENT_META_INTERFACE(ApplicationThread, "com::cdroid::runtime::IApplicationThread");

};
