#ifndef GENERATE_CPP_H
#define GENERATE_CPP_H

#include "aidl_language.h"
#include "aidl_common.h"
#include "CAST.h"

#include <string>

using namespace std;

int generate_cpp(const string& headerf, const string &sourcef,
                const string& originalSrc,
                interface_type* iface);

CNamespace* cgenerate_binder_interface_class(const interface_type* iface);

string gather_comments(extra_text_type* extra);
string append(const char* a, const char* b);

class CVariableFactory
{
public:
    CVariableFactory(const string& base); // base must be short
    CVariable* Get(CType* type);
    CVariable* Get(int index);
private:
    vector<CVariable*> m_vars;
    string m_base;
    int m_index;
};

#endif // GENERATE_JAVA_H

