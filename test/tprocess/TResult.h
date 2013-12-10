#ifndef COM_ROUTON_OS_TRESULT_H
#define COM_ROUTON_OS_TRESULT_H
#include <binder/Parcelable.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

namespace com {
    namespace routon {
        namespace os {
            using android::String8;
            using android::Parcelable;
            using android::Parcel;
            class TResult : public virtual Parcelable{
                public:
                    TResult(int rc, const String8& _stdout, const String8& _stderr):
                        mStatus(rc), mStdoutStr(_stdout), mStderrStr(_stderr){
                        }
                    ~TResult();

                    int describeContents();
                    void writeToParcel(Parcel *out, int flags);
                    TResult *createFromParcel(const Parcel& source);
                    void readFromParcel(const Parcel& source);
                    std::vector<Parcelable *> newArray(int size);

                public:
                    int mStatus;
                    String8 mStdoutStr;
                    String8 mStderrStr;
            };
        };
    };
};

#endif
