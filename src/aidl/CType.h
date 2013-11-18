#ifndef AIDL_CTYPE_H
#define AIDL_CTYPE_H

#include "CAST.h"
#include <string>
#include <vector>

using namespace std;

class CType
{
public:
    // kinds
    enum {
        BUILT_IN,
        USERDATA,
        INTERFACE,
        GENERATED
    };

    // WriteToParcel flags
    enum {
        PARCELABLE_WRITE_RETURN_VALUE = 0x0001
    };

                    CType(const string& name, int kind, bool canWriteToParcel,
                            bool canWriteToRpcData, bool canBeOut);
                    CType(const string& package, const string& name,
                            int kind, bool canWriteToParcel, bool canWriteToRpcData, bool canBeOut,
                            const string& declFile = "", int declLine = -1);
    virtual         ~CType();

    inline string   Namespace() const             { return m_namespace; }
    inline string   Name() const                { return m_name; }
    inline string   QualifiedName() const       { return m_qualifiedName; }
    inline int      Kind() const                { return m_kind; }
    inline string   DeclFile() const            { return m_declFile; }
    inline int      DeclLine() const            { return m_declLine; }
    inline bool     CanWriteToParcel() const    { return m_canWriteToParcel; }
    inline bool     CanWriteToRpcData() const   { return m_canWriteToRpcData; }
    inline bool     CanBeOutParameter() const   { return m_canBeOut; }

    virtual string  IncludeName() const;
    virtual string  InstantiableName() const;

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);
    virtual void    ReadFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

    virtual void    WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, int flags);
    virtual void    CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, CVariable** cl);

protected:
    void SetQualifiedName(const string& qualified);
    CExpression* BuildWriteToParcelFlags(int flags);

private:
    CType();
    CType(const CType&);

    string m_namespace;
    string m_name;
    string m_qualifiedName;
    string m_declFile;
    int m_declLine;
    int m_kind;
    bool m_canWriteToParcel;
    bool m_canWriteToRpcData;
    bool m_canBeOut;
};

class CBasicType : public CType
{
public:
                    CBasicType(const string& name,
                              const string& marshallParcel,
                              const string& unmarshallParcel,
                              const string& marshallRpc,
                              const string& unmarshallRpc);

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

    virtual void    WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, int flags);
    virtual void    CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, CVariable** cl);

private:
    string m_marshallParcel;
    string m_unmarshallParcel;
    string m_marshallRpc;
    string m_unmarshallRpc;
};

class CBooleanType : public CType
{
public:
                    CBooleanType();

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

    virtual void    WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, int flags);
    virtual void    CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, CVariable** cl);
};

class CCharType : public CType
{
public:
                    CCharType();

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

    virtual void    WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, int flags);
    virtual void    CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, CVariable** cl);
};


// choose String8
class CStringType : public CType
{
public:
                    CStringType();

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

    virtual void    WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, int flags);
    virtual void    CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, CVariable** cl);
};

class CRemoteExceptionType : public CType
{
public:
                    CRemoteExceptionType();

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);
};

class CIBinderType : public CType
{
public:
                    CIBinderType();

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);
};

class CIInterfaceType : public CType
{
public:
                    CIInterfaceType();

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);
};

class CParcelableInterfaceType : public CType
{
public:
                    CParcelableInterfaceType();

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);
};


class CUserDataType : public CType
{
public:
                    CUserDataType(const string& _namespace, const string& name,
                            bool builtIn, bool canWriteToParcel, bool canWriteToRpcData,
                            const string& declFile = "", int declLine = -1);

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);
    virtual void    ReadFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

    virtual void    WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, int flags);
    virtual void    CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
                                    CVariable* data, CVariable** cl);
};

// For interface in preprocessed files
// For public interface specific class,eg ITprocess
class CInterfaceType : public CType
{
public:
                    CInterfaceType(const string& _namespace, const string& name,
                            bool builtIn, bool oneway,
                            const string& declFile, int declLine);

    bool            OneWay() const;

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

private:
    bool m_oneway;
};

class CEnumType : public CType
{
public:
                    CEnumType();
    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

}


class CTemplateType : public CType
{
public:
                    CTemplateType(const string& _namespace, const string& name,
                                  const vector<CType*>& args);

    const vector<CType*>& TemplateArgumentTypes() const;
    string          TemplateArguments() const;

    virtual void    WriteToParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, int flags);
    virtual void    CreateFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);
    virtual void    ReadFromParcel(CStatementBlock* addTo, CVariable* v,
                                    CVariable* parcel, CVariable** cl);

private:
    string m_templateArguments;
    vector<CType*> m_args;
};

class CNameContainer
{
public:
            CNameContainer();
            ~CNameContainer();
    void    Add(CType* type);

    // args is the number of template types (what is this called?)
    void    AddTemplateType(const string& _namespace, const string& name, int args);

    // lookup a specific class name
    CType*   Find(const string& name) const;
    CType*   Find(const char* _namespace, const char* name) const;

    // try to search by either a full name or a partial name
    CType*   Search(const string& name);

    void    Dump() const;

private:
    struct _CTemplate {
        string _namespace;
        string name;
        string qualified;
        int args;
    };

    const _CTemplate* search_template(const string& name) const;

    vector<CType*> m_types;
    vector<_CTemplate> m_templates;
};

extern CNameContainer CNAMES;

extern CType* CVOID_TYPE;
extern CType* CBOOLEAN_TYPE;
extern CType* CBYTE_TYPE;
extern CType* CCHAR_TYPE;
extern CType* CINT_TYPE;
extern CType* CLONG_TYPE;
extern CType* CFLOAT_TYPE;
extern CType* CDOUBLE_TYPE;
extern CType* CENUM_TYPE;
/*extern CType* OBJECT_TYPE;*/
extern CType* CSTRING_TYPE;
/*extern CType* CHAR_SEQUENCE_TYPE;*/
/*extern CType* TEXT_UTILS_TYPE;*/
/*extern CType* REMOTE_EXCEPTION_TYPE;*/
/*extern CType* RUNTIME_EXCEPTION_TYPE;*/
extern CType* CIBINDER_TYPE;
extern CType* CIINTERFACE_TYPE;
/*extern CType* BINDER_NATIVE_TYPE;*/
/*extern CType* BINDER_PROXY_TYPE;*/
/*extern CType* CPARCEL_TYPE;*/
/*extern CType* CPARCELABLE_INTERFACE_TYPE;*/

/*extern CType* CONTEXT_TYPE;*/

/*extern CType* RPC_DATA_TYPE;*/
/*extern CType* RPC_ERROR_TYPE;*/
/*extern CType* RPC_CONTEXT_TYPE;*/
/*extern CType* EVENT_FAKE_TYPE;*/

extern CExpression* CNULL_VALUE;
extern CExpression* CTHIS_VALUE;
/*extern CExpression* SUPER_VALUE;*/
extern CExpression* CTRUE_VALUE;
extern CExpression* CFALSE_VALUE;

void cregister_base_types();

#endif // AIDL_CTYPE_H
