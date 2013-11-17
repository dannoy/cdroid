#include "CType.h"

CNamespace CCNAMES;

CType* CVOID_TYPE;
CType* CBOOLEAN_TYPE;
CType* CBYTE_TYPE;
CType* CCHAR_TYPE;
CType* CINT_TYPE;
CType* CLONG_TYPE;
CType* CFLOAT_TYPE;
CType* CDOUBLE_TYPE;
CType* CSTRING_TYPE;
//Type* OBJECT_TYPE;
//Type* CHAR_SEQUENCE_TYPE;
//Type* TEXT_UTILS_TYPE;
//Type* REMOTE_EXCEPTION_TYPE;
//Type* RUNTIME_EXCEPTION_TYPE;
CType* CIBINDER_TYPE;
CType* CIINTERFACE_TYPE;
//Type* BNINTERFACE_TYPE;
//Type* BPINTERFACE_TYPE;
//Type* BINDER_NATIVE_TYPE;
//Type* BINDER_PROXY_TYPE;
//CType* CPARCEL_TYPE;
CType* CPARCELABLE_INTERFACE_TYPE;
//Type* CONTEXT_TYPE;
//CType* MAP_TYPE;
//CType* LIST_TYPE;
//CType* CLASSLOADER_TYPE;
//Type* RPC_DATA_TYPE;
//Type* RPC_ERROR_TYPE;
//Type* EVENT_FAKE_TYPE;

CExpression* CCNULL_VALUE;
CExpression* CTHIS_VALUE;
//Expression* SUPER_VALUE;
CExpression* CTRUE_VALUE;
CExpression* CFALSE_VALUE;

void
cregister_base_types()
{
    CVOID_TYPE = new CBasicType("void",
            "XXX", "XXX", "XXX", "XXX");
    CCNAMES.Add(CVOID_TYPE);

    CBOOLEAN_TYPE = new CBooleanType();
    CCNAMES.Add(CBOOLEAN_TYPE);

    CBYTE_TYPE = new CBasicType("byte",
            "writeByte", "readByte",
            "putByte", "getByte");
    CCNAMES.Add(CBYTE_TYPE);

    CCHAR_TYPE = new CCharType();
    CCNAMES.Add(CCHAR_TYPE);

    CINT_TYPE = new CBasicType("int",
            "writeInt32", "readInt32",
            "putInteger", "getInteger");
    CCNAMES.Add(CINT_TYPE);

    CLONG_TYPE = new CBasicType("long",
            #ifdef LONG_BITS_32
            "writeInt32", "readInt32",
            #endif
            #ifdef LONG_BITS_64
            "writeInt64", "readInt64",
            #endif
            "putLong", "getLong");
    CCNAMES.Add(CLONG_TYPE);

    CFLOAT_TYPE = new CBasicType("float",
            "writeFloat", "readFloat",
            "putFloat", "getFloat");
    CCNAMES.Add(CFLOAT_TYPE);

    CDOUBLE_TYPE = new CBasicType("double",
            "writeDouble", "readDouble",
            "putDouble", "getDouble");
    CCNAMES.Add(CDOUBLE_TYPE);

    CSTRING_TYPE = new CStringType();
    CCNAMES.Add(CSTRING_TYPE);


    CIBINDER_TYPE = new CIBinderType();
    CCNAMES.Add(CIBINDER_TYPE);

    CIINTERFACE_TYPE = new CIInterfaceType();
    CCNAMES.Add(CIINTERFACE_TYPE);

    //PARCEL_TYPE = new ParcelType();
    //CNAMES.Add(PARCEL_TYPE);

    CPARCELABLE_INTERFACE_TYPE = new CParcelableInterfaceType();
    CNAMES.Add(CPARCELABLE_INTERFACE_TYPE);


    CCNULL_VALUE = new CLiteralExpression("null");
    CTHIS_VALUE = new CLiteralExpression("this");
    CTRUE_VALUE = new CLiteralExpression("true");
    CFALSE_VALUE = new CLiteralExpression("false");

    CCNAMES.AddTemplateType("", "vector", 1);
}

static CType*
make_template_type(const string& _namespace, const string& name,
                    const vector<CType*>& args)
{
    return NULL;
    //return new GenericType(package, name, args);
}

// ================================================================

CType::CType(const string& name, int kind, bool canWriteToParcel, bool canWriteToRpcData,
        bool canBeOut)
    :m_namespace(),
     m_name(name),
     m_declFile(""),
     m_declLine(-1),
     m_kind(kind),
     m_canWriteToParcel(canWriteToParcel),
     m_canWriteToRpcData(canWriteToRpcData),
     m_canBeOut(canBeOut)
{
    m_qualifiedName = name;
}

CType::CType(const string& _namespace, const string& name,
            int kind, bool canWriteToParcel, bool canWriteToRpcData,
            bool canBeOut, const string& declFile, int declLine)
    :m_namespace(_namespace),
     m_name(name),
     m_declFile(declFile),
     m_declLine(declLine),
     m_kind(kind),
     m_canWriteToParcel(canWriteToParcel),
     m_canWriteToRpcData(canWriteToRpcData),
     m_canBeOut(canBeOut)
{
    if (_namespace.length() > 0) {
        m_qualifiedName = _namespace;
        m_qualifiedName += "::";
    }
    m_qualifiedName += name;
}

CType::~CType()
{
}

string
CType::IncludeName() const
{
    //TODO
    return "";
}


string
CType::InstantiableName() const
{
    return QualifiedName();
}


void
CType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%sn",
            __FILE__, __LINE__, m_qualifiedName.c_str());
    addTo->Add(new CLiteralExpression("/* WriteToParcel error "
                + m_qualifiedName + " */"));
}

void
CType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n",
            __FILE__, __LINE__, m_qualifiedName.c_str());
    addTo->Add(new CLiteralExpression("/* CreateFromParcel error "
                + m_qualifiedName + " */"));
}

void
CType::ReadFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n",
            __FILE__, __LINE__, m_qualifiedName.c_str());
    addTo->Add(new CLiteralExpression("/* ReadFromParcel error "
                + m_qualifiedName + " */"));
}

void
CType::WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
        CVariable* data, int flags)
{
    fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n",
            __FILE__, __LINE__, m_qualifiedName.c_str());
    addTo->Add(new CLiteralExpression("/* WriteToRpcData error "
                + m_qualifiedName + " */"));
}

void
CType::CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v, CVariable* data,
        CVariable** cl)
{
    fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n",
            __FILE__, __LINE__, m_qualifiedName.c_str());
    addTo->Add(new CLiteralExpression("/* ReadFromRpcData error "
                + m_qualifiedName + " */"));
}

void
CType::SetQualifiedName(const string& qualified)
{
    m_qualifiedName = qualified;
}

CExpression*
CType::BuildWriteToParcelFlags(int flags)
{
    if (flags == 0) {
        return new CLiteralExpression("0");
    }
    if ((flags&PARCELABLE_WRITE_RETURN_VALUE) != 0) {
        return new CFieldVariable(CPARCELABLE_INTERFACE_TYPE,
                "PARCELABLE_WRITE_RETURN_VALUE");
    }
    return new CLiteralExpression("0");
}

// ================================================================

CBasicType::CBasicType(const string& name, const string& marshallParcel,
          const string& unmarshallParcel,
          const string& marshallRpc, const string& unmarshallRpc)
    :CType(name, BUILT_IN, true, true, false),
     m_marshallParcel(marshallParcel),
     m_unmarshallParcel(unmarshallParcel),
     m_marshallRpc(marshallRpc),
     m_unmarshallRpc(unmarshallRpc)
{
}

void
CBasicType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    addTo->Add(new CMethodCall(parcel, m_marshallParcel, 1, v));
}

void
CBasicType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    addTo->Add(new CAssignment(v, new CMethodCall(parcel, m_unmarshallParcel)));
}

void
CBasicType::WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
        CVariable* data, int flags)
{
    addTo->Add(new CMethodCall(data, m_marshallRpc, 2, k, v));
}

void
CBasicType::CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v, CVariable* data,
        CVariable** cl)
{
    addTo->Add(new CAssignment(v, new CMethodCall(data, m_unmarshallRpc, 1, k)));
}

// ================================================================

CBooleanType::CBooleanType()
    :CType("bool", BUILT_IN, true, true, false)
{
}

void
CBooleanType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    addTo->Add(new CMethodCall(parcel, "writeInt32", 1, 
                new CTernary(v, new CLiteralExpression("1"),
                    new CLiteralExpression("0"))));
}

void
CBooleanType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    addTo->Add(new CAssignment(v, new CComparison(new CLiteralExpression("0"),
                    "!=", new CMethodCall(parcel, "readInt32"))));
}

void
CBooleanType::WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
        CVariable* data, int flags)
{
    addTo->Add(new CMethodCall(data, "putBoolean", 2, k, v));
}

void
CBooleanType::CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v, CVariable* data,
        CVariable** cl)
{
    addTo->Add(new CAssignment(v, new CMethodCall(data, "getBoolean", 1, k)));
}

// ================================================================

CCharType::CCharType()
    :CType("char", BUILT_IN, true, true, false)
{
}

void
CCharType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    addTo->Add(new CMethodCall(parcel, "writeInt32", 1, 
                    new CCast(CINT_TYPE, v)));
}

void
CCharType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    addTo->Add(new CAssignment(v, new CMethodCall(parcel, "readInt32"), this));
}

void
CCharType::WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
        CVariable* data, int flags)
{
    addTo->Add(new CMethodCall(data, "putChar", 2, k, v));
}

void
CCharType::CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v, CVariable* data,
        CVariable** cl)
{
    addTo->Add(new CAssignment(v, new CMethodCall(data, "getChar", 1, k)));
}

// ================================================================

CStringType::CStringType()
    :CType("android", "String8", BUILT_IN, true, true, false)
{
    //SetQualifiedName("android::String8");
}

void
CStringType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    addTo->Add(new CMethodCall(parcel, "writeString8", 1, v));
}

void
CStringType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    addTo->Add(new CAssignment(v, new CMethodCall(parcel, "readString8")));
}

void
CStringType::WriteToRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
        CVariable* data, int flags)
{
    addTo->Add(new CMethodCall(data, "putString", 2, k, v));
}

void
CStringType::CreateFromRpcData(CStatementBlock* addTo, CExpression* k, CVariable* v,
        CVariable* data, CVariable**)
{
    addTo->Add(new CAssignment(v, new CMethodCall(data, "getString", 1, k)));
}

// ================================================================

//RemoteExceptionType::RemoteExceptionType()
    //:Type("android.nonexit", "RemoteException", BUILT_IN, false, false, false)
//{
//}

//void
//RemoteExceptionType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
//{
    //fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
//}

//void
//RemoteExceptionType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
//{
    //fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
//}

// ================================================================

CIBinderType::CIBinderType()
    :CType("android", "IBinder", BUILT_IN, true, false, false)
{
    //SetQualifiedName("android::IBinder");
}

void
CIBinderType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    addTo->Add(new CMethodCall(parcel, "writeStrongBinder", 1, v));
}

void
CIBinderType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    addTo->Add(new CAssignment(v, new CMethodCall(parcel, "readStrongBinder")));
}

// ================================================================

CIInterfaceType::CIInterfaceType()
    :CType("android", "IInterface", BUILT_IN, false, false, false)
{
    //SetQualifiedName("android::IInterface");
}

void
CIInterfaceType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void
CIInterfaceType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}
// ================================================================

CInterfaceType::CInterfaceType(const string& _namespace, const string& name,
                        bool builtIn, bool oneway,
                        const string& declFile, int declLine)
    :CType(_namespace, name, INTERFACE, true, false, false,
                        declFile, declLine)
    ,m_oneway(oneway)
{
}

bool
CInterfaceType::OneWay() const
{
    return m_oneway;
}

void
CInterfaceType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    // parcel.writeStrongBinder(v != null ? v.asBinder() : null);
    addTo->Add(new CMethodCall(parcel, "writeStrongBinder", 1, 
                new CTernary(
                    new CComparison(v, "!=", CNULL_VALUE),
                    new CMethodCall(v, "asBinder"),
                    CNULL_VALUE)));
}

void
CInterfaceType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    // v = Interface.asInterface(parcel.readStrongBinder());
    string type = v->type->QualifiedName();
    type += ".Stub";
    addTo->Add(new CAssignment(v,
                new CMethodCall( CNAMES.Find(type), "asInterface", 1,
                    new CMethodCall(parcel, "readStrongBinder"))));
}



// ================================================================

CUserDataType::CUserDataType(const string& _namespace, const string& name,
                        bool builtIn, bool canWriteToParcel, bool canWriteToRpcData,
                        const string& declFile, int declLine)
    :CType(_namespace, name, builtIn ? BUILT_IN : USERDATA, canWriteToParcel, canWriteToRpcData,
            true, declFile, declLine)

{
}

void
CUserDataType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    // if (v != null) {
    //     parcel.writeInt(1);
    //     v.writeToParcel(parcel);
    // } else {
    //     parcel.writeInt(0);
    // }
    CIfStatement* elsepart = new CIfStatement();
    elsepart->statements->Add(new CMethodCall(parcel, "writeInt32", 1,
                                new CLiteralExpression("0")));
    CIfStatement* ifpart = new CIfStatement;
    ifpart->expression = new CComparison(v, "!=", CNULL_VALUE);
    ifpart->elseif = elsepart;
    ifpart->statements->Add(new CMethodCall(parcel, "writeInt32", 1,
                                new CLiteralExpression("1")));
    ifpart->statements->Add(new CMethodCall(v, "writeToParcel", 2,
                                parcel, BuildWriteToParcelFlags(flags)));

    addTo->Add(ifpart);
}

void
CUserDataType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    // if (0 != parcel.readInt()) {
    //     v = CLASS.CREATOR.createFromParcel(parcel)
    // } else {
    //     v = null;
    // }
    CIfStatement* elsepart = new CIfStatement();
    elsepart->statements->Add(new CAssignment(v, CNULL_VALUE));

    CIfStatement* ifpart = new CIfStatement();
    ifpart->expression = new CComparison(new CLiteralExpression("0"), "!=",
                new CMethodCall(parcel, "readInt32"));
    ifpart->elseif = elsepart;
    ifpart->statements->Add(new CAssignment(v,
                new CMethodCall(v->type, "createFromParcel", 1, parcel)));

    addTo->Add(ifpart);
}

void
CUserDataType::ReadFromParcel(CStatementBlock* addTo, CVariable* v,
                    CVariable* parcel, CVariable**)
{
    // TODO: really, we don't need to have this extra check, but we
    // don't have two separate marshalling code paths
    // if (0 != parcel.readInt()) {
    //     v.readFromParcel(parcel)
    // }
    CIfStatement* ifpart = new CIfStatement();
    ifpart->expression = new CComparison(new CLiteralExpression("0"), "!=",
                new CMethodCall(parcel, "readInt"));
    ifpart->statements->Add(new CMethodCall(v, "readFromParcel", 1, parcel));
    addTo->Add(ifpart);
}


// ================================================================

CParcelableInterfaceType::CParcelableInterfaceType()
    :CType("android", "Parcelable", BUILT_IN, false, false, false)
{
}

void
CParcelableInterfaceType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void
CParcelableInterfaceType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

CTemplateType::CTemplateType(const string& _namespace, const string& name,
                         const vector<CType*>& args)
    :CType(_namespace, name, BUILT_IN, true, true, true)
{
    m_args = args;


    string gen = "<";
    int N = args.size();
    for (int i=0; i<N; i++) {
        CType* t = args[i];
        gen += t->QualifiedName();
        if (i != N-1) {
            gen += ',';
        }
    }
    gen += '>';
    m_genericArguments = gen;
    SetQualifiedName(_namespace + "::" + name + gen);
}

const vector<CType*>&
CTemplateType::GenericArgumentTypes() const
{
    return m_args;
}

string
CTemplateType::GenericArguments() const
{
    return m_genericArguments;
}

string
CTemplateType::ImportType() const
{
    return m_importName;
}

void
CTemplateType::WriteToParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, int flags)
{
    fprintf(stderr, "implement GenericType::WriteToParcel\n");
}

void
CTemplateType::CreateFromParcel(CStatementBlock* addTo, CVariable* v, CVariable* parcel, CVariable**)
{
    fprintf(stderr, "implement GenericType::CreateFromParcel\n");
}

void
CTemplateType::ReadFromParcel(CStatementBlock* addTo, CVariable* v,
                            CVariable* parcel, CVariable**)
{
    fprintf(stderr, "implement GenericType::ReadFromParcel\n");
}

// ================================================================

CNamespace::CNamespace()
{
}

CNamespace::~CNamespace()
{
    int N = m_types.size();
    for (int i=0; i<N; i++) {
        delete m_types[i];
    }
}

void
CNamespace::Add(CType* type)
{
    CType* t = Find(type->QualifiedName());
    if (t == NULL) {
        m_types.push_back(type);
    }
}

void
CNamespace::AddTemplateType(const string& _namespace, const string& name, int args)
{
    _CTemplate g;
        g._namespace = _namespace;
        g.name = name;
        g.qualified = _namespace + "::" + name;
        g.args = args;
    m_templates.push_back(g);
}

CType*
CNamespace::Find(const string& name) const
{
    int N = m_types.size();
    for (int i=0; i<N; i++) {
        if (m_types[i]->QualifiedName() == name) {
            return m_types[i];
        }
    }
    return NULL;
}

CType*
CNamespace::Find(const char* _namespace, const char* name) const
{
    string s;
    if (_namespace != NULL) {
        s += _namespace;
        s += "::";
    }
    s += name;
    return Find(s);
}

static string
normalize_generic(const string& s)
{
    string r;
    int N = s.size();
    for (int i=0; i<N; i++) {
        char c = s[i];
        if (!isspace(c)) {
            r += c;
        }
    }
    return r;
}

CType*
CNamespace::Search(const string& name)
{
    // an exact match wins
    CType* result = Find(name);
    if (result != NULL) {
        return result;
    }

    // try the class names
    // our language doesn't allow you to not specify outer classes
    // when referencing an inner class.  that could be changed, and this
    // would be the place to do it, but I don't think the complexity in
    // scoping rules is worth it.
    int N = m_types.size();
    for (int i=0; i<N; i++) {
        if (m_types[i]->Name() == name) {
            return m_types[i];
        }
    }

    // we got to here and it's not a generic, give up
    if (name.find('<') == name.npos) {
        return NULL;
    }

    // remove any whitespace
    string normalized = normalize_generic(name);

    // find the part before the '<', find a generic for it
    ssize_t baseIndex = normalized.find('<');
    string base(normalized.c_str(), baseIndex);
    const _CTemplate* g = search_template(base);
    if (g == NULL) {
        return NULL;
    }

    // For each of the args, do a recursive search on it.  We don't allow
    // generics within generics like Java does, because we're really limiting
    // them to just built-in container classes, at least for now.  Our syntax
    // ensures this right now as well.
    vector<CType*> args;
    size_t start = baseIndex + 1;
    size_t end = start;
    while (normalized[start] != '\0') {
        end = normalized.find(',', start);
        if (end == normalized.npos) {
            end = normalized.find('>', start);
        }
        string s(normalized.c_str()+start, end-start);
        CType* t = this->Search(s);
        if (t == NULL) {
            // maybe we should print a warning here?
            return NULL;
        }
        args.push_back(t);
        start = end+1;
    }

    // construct a GenericType, add it to our name set so they always get
    // the same object, and return it.
    result = make_template_type(g->_namespace, g->name, args);
    if (result == NULL) {
        return NULL;
    }

    this->Add(result);
    return this->Find(result->QualifiedName());
}

const CNamespace::_CTemplate*
CNamespace::search_template(const string& name) const
{
    int N = m_templates.size();

    // first exact match
    for (int i=0; i<N; i++) {
        const _CTemplate& g = m_templates[i];
        if (g.qualified == name) {
            return &g;
        }
    }

    // then name match
    for (int i=0; i<N; i++) {
        const _CTemplate& g = m_templates[i];
        if (g.name == name) {
            return &g;
        }
    }

    return NULL;
}

void
CNamespace::Dump() const
{
    int n = m_types.size();
    for (int i=0; i<n; i++) {
        CType* t = m_types[i];
        printf("type: package=%s name=%s qualifiedName=%s\n",
                t->Namespace().c_str(), t->Name().c_str(),
                t->QualifiedName().c_str());
    }
}
