#include "generate_cpp.h"
#include "CType.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =================================================
CVariableFactory::CVariableFactory(const string& base)
    :m_base(base),
     m_index(0)
{
}

CVariable*
CVariableFactory::Get(Type* type)
{
    char name[100];
    sprintf(name, "%s%d", m_base.c_str(), m_index);
    m_index++;
    CVariable* v = new CVariable(type, name);
    m_vars.push_back(v);
    return v;
}

CVariable*
CVariableFactory::Get(int index)
{
    return m_vars[index];
}

// =================================================
string
gather_comments(extra_text_type* extra)
{
    string s;
    while (extra) {
        if (extra->which == SHORT_COMMENT) {
            s += extra->data;
        }
        else if (extra->which == LONG_COMMENT) {
            s += "/*";
            s += extra->data;
            s += "*/";
        }
        extra = extra->next;
    }
    return s;
}

string
append(const char* a, const char* b)
{
    string s = a;
    s += b;
    return s;
}

// =================================================
int
generate_cpp(const string& filename, const string& originalSrc,
                interface_type* iface)
{
    CNamespace* ns;

    if (iface->document_item.item_type == INTERFACE_TYPE_BINDER) {
        ns = generate_binder_interface_class(iface);
    }

    CDocument* document = new Document;
        document->comment = "";
        if (iface->package) document->package = iface->package;
        document->originalSrc = originalSrc;
        document->nss.push_back(ns);

//    printf("outputting... filename=%s\n", filename.c_str());
    FILE* to;
    if (filename == "-") {
        to = stdout;
    } else {
       /* open file in binary mode to ensure that the tool produces the
        * same output on all platforms !!
        */
        to = fopen(filename.c_str(), "wb");
        if (to == NULL) {
            fprintf(stderr, "unable to open %s for write\n", filename.c_str());
            return 1;
        }
    }

    document->Write(to);

    fclose(to);
    return 0;
}

