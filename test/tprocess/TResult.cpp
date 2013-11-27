#include "TResult.h"

namespace com {
    namespace routon {
        namespace os {
            void TResult::writeToParcel(Parcel *out, int flags) {
                out->writeInt32(mStatus);
                out->writeString8(mStdoutStr);
                out->writeString8(mStderrStr);
                /*fprintf(stderr, " writeToParcel  %d out %s err %s\n", */
                /*mStatus, String8(mStdoutStr).string(),*/
                /*String8(mStderrStr).string());*/

            }

            int TResult::describeContents() {
                return 0;
            }

            TResult *TResult::createFromParcel(Parcel& source){
                String8 err = source.readString8();
                String8 ou = source.readString8();
                int st = source.readInt32();
                return new TResult(st,ou, err);
            }

            void TResult::readFromParcel(Parcel& source) {
                this->mStderrStr =  source.readString8();
                this->mStdoutStr =  source.readString8();
                this->mStatus =  source.readInt32();
            }

            std::vector<Parcelable *> TResult::newArray(int size) {
                std::vector<Parcelable *> v;
                for (int i = 0; i < size; ++i) {
                    v.push_back(new TResult(0,String8(""),String8("")));
                }

                return v;
            }
        };
    };
};
