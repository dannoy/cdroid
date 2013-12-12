#include <iostream>

#include <runtime/ActivityInfo.h>
using namespace cdroid;
using namespace std;

int main()
{
    sp<ActivityInfo> ai = new ActivityInfo(String8("name"), String8("file"));
    sp<RefBase> ref = ai;
    sp<ActivityInfo> aref = ai;

    cout << ai.get() << endl;
    cout << ref.get() << endl;
    cout << aref.get() << endl;


    return 0;
}
