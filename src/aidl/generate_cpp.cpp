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
CVariableFactory::Get(CType* type)
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
// =================================================
int
generate_cpp(const string& headerf, const string &sourcef,
                        const string& originalSrc,
                        interface_type* iface)
{
    CNamespace* ns;

    if (iface->document_item.item_type == INTERFACE_TYPE_BINDER) {
        ns = cgenerate_binder_interface_class(iface);
    }

    CDocument* document = new CDocument;
        document->comment = "";
        document->header_file = headerf;
        document->source_file = sourcef;
        if (iface->package) document->_namespace = package2namespace(iface->package);
        document->originalSrc = originalSrc;
        document->nss.push_back(ns);

    import_info* import = g_imports;
        while (import) {
            char *pos = rfind(const_cast<char *>(import->filename),'/');

            if(pos != NULL) {
                string f(pos+1);
                int n = f.find('.');
                f.replace(n + 1, 4, "h");
                document->includes.insert(f);
            }
            import = import->next;
        }

//    printf("outputting... filename=%s\n", filename.c_str());
    FILE* to;
   /* open file in binary mode to ensure that the tool produces the
    * same output on all platforms !!
    */
    to = fopen(headerf.c_str(), "wb");
    if (to == NULL) {
        fprintf(stderr, "unable to open %s for write\n", headerf.c_str());
        return 1;
    }

    if(g_copyExtra) {
        fprintf(to,"%s", g_copyExtra->data);
    }
    document->WriteToHeader(to);

    fclose(to);

    to = fopen(sourcef.c_str(), "wb");
    if (to == NULL) {
        fprintf(stderr, "unable to open %s for write\n", sourcef.c_str());
        return 1;
    }

    document->WriteToSource(to);

    fclose(to);
    return 0;
}

