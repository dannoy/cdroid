#ifndef GENERATE_CPP_H
#define GENERATE_CPP_H

#include "aidl_language.h"
#include "CAST.h"

#include <string>

using namespace std;

int generate_cpp(const string& filename, const string& originalSrc,
                interface_type* iface);

Class* cgenerate_binder_interface_class(const interface_type* iface);
Class* cgenerate_rpc_interface_class(const interface_type* iface);

string gather_comments(extra_text_type* extra);
string append(const char* a, const char* b);

class VariableFactory
{
public:
    VariableFactory(const string& base); // base must be short
    Variable* Get(Type* type);
    Variable* Get(int index);
private:
    vector<Variable*> m_vars;
    string m_base;
    int m_index;
};

#endif // GENERATE_JAVA_H

