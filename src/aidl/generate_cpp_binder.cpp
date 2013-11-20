#include "generate_cpp.h"
#include "CType.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =================================================
static void
cgenerate_write_to_parcel(CType* t, CStatementBlock* addTo, CVariable* v,
                            CVariable* parcel, int flags)
{
    if (v->dimension == 0) {
        t->WriteToParcel(addTo, v, parcel, flags);
    }
}

static void
cgenerate_create_from_parcel(CType* t, CStatementBlock* addTo, CVariable* v,
                            CVariable* parcel, CVariable** cl)
{
    if (v->dimension == 0) {
        t->CreateFromParcel(addTo, v, parcel, cl);
    }
}

static void
cgenerate_read_from_parcel(CType* t, CStatementBlock* addTo, CVariable* v,
                            CVariable* parcel, CVariable** cl)
{
    if (v->dimension == 0) {
        t->ReadFromParcel(addTo, v, parcel, cl);
    }
}


static void
cgenerate_method(const method_type* method, CClass* interface,
                    CEnum *eu, CClass *bn, CSwitchStatement *bnsw,
                    CClass* bp, int index)
{
    arg_type* arg;
    int i;
    bool hasOutParams = false;

    const bool oneway = method->oneway;

    // == the TRANSACT_ constant =============================================
    string transactCodeName = "TRANSACTION_";
    transactCodeName += method->name.data;

    char transactCodeValue[60];
    sprintf(transactCodeValue, "IBinder::FIRST_CALL_TRANSACTION + %d", index);

    eu->elements.push_back(new CEnumElement(transactCodeName,
                            string(transactCodeValue)));

    // == the declaration in the interface ===================================
    CMethod* decl = new CMethod;
        decl->comment = gather_comments(method->comments_token->extra);
        decl->modifiers = CPUBLIC;
        decl->returnType = CNAMES.Search(method->type.type.data);
        decl->returnTypeDimension = method->type.dimension;
        decl->name = method->name.data;
    fprintf(stderr, "%s %d %p %s\n",__func__, __LINE__, decl->returnType, method->type.type.data);

    arg = method->args;
    while (arg != NULL) {
        decl->parameters.push_back(new CVariable(
                            CNAMES.Search(arg->type.type.data), arg->name.data,
                            arg->type.dimension));
        arg = arg->next;
    }

    interface->elements.push_back(decl);

    // == the BN method ====================================================
    {
        CVariable *transact_code = new CVariable(CINT_TYPE, "code");
        CVariable *transact_data = new CVariable(CPARCEL_TYPE, "data");
        CVariable *transact_reply = new CVariable(CPARCEL_TYPE, "reply");
        CVariable *transact_flags = new CVariable(CINT_TYPE, "flags");
        CVariable *dummy = new CVariable(CINT_TYPE, interface->type->Name());

        CCase* c = new CCase(transactCodeName);

        CMethodCall* realCall = new CMethodCall(CTHIS_VALUE, method->name.data);

        // interface token validation is the very first thing we do
        c->statements->Add(new CMethodCall("CHECK_INTERFACE",
                        3, dummy, transact_data, transact_reply));

        // args
        CVariable* cl = NULL;
        CVariableFactory stubArgs("_arg");
        arg = method->args;
        while (arg != NULL) {
            CType* t = CNAMES.Search(arg->type.type.data);
            CVariable* v = stubArgs.Get(t);
            v->dimension = arg->type.dimension;

            c->statements->Add(new CVariableDeclaration(v));

            if (convert_direction(arg->direction.data) & IN_PARAMETER) {
                cgenerate_create_from_parcel(t, c->statements, v,
                        transact_data, &cl);
            } else {
                if (arg->type.dimension == 0) {
                    c->statements->Add(new CAssignment(v, new CNewExpression(v->type)));
                }
                else {
                    fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__,
                            __LINE__);
                }
            }

            realCall->arguments.push_back(v);

            arg = arg->next;
        }

        // the real call
        CVariable* _result = NULL;
        if (0 == strcmp(method->type.type.data, "void")) {
            c->statements->Add(realCall);

            if (!oneway) {
                // report that there were no exceptions
                CMethodCall* ex = new CMethodCall(transact_reply,
                        "writeNoException", 0);
                c->statements->Add(ex);
            }
        } else {
            _result = new CVariable(decl->returnType, "_result",
                                    decl->returnTypeDimension);
            c->statements->Add(new CVariableDeclaration(_result, realCall));

            if (!oneway) {
                // report that there were no exceptions
                CMethodCall* ex = new CMethodCall(transact_reply,
                        "writeNoException", 0);
                c->statements->Add(ex);
            }

    fprintf(stderr, "%s %d %p\n",__func__, __LINE__, decl->returnType);
            // marshall the return value
            cgenerate_write_to_parcel(decl->returnType, c->statements, _result,
                                        transact_reply,
                                        CType::PARCELABLE_WRITE_RETURN_VALUE);
        }
    fprintf(stderr, "%s %d\n",__func__, __LINE__);

        // out parameters
        i = 0;
        arg = method->args;
        while (arg != NULL) {
            CType* t = CNAMES.Search(arg->type.type.data);
            CVariable* v = stubArgs.Get(i++);

            if (convert_direction(arg->direction.data) & OUT_PARAMETER) {
                cgenerate_write_to_parcel(t, c->statements, v,
                                    transact_reply,
                                    CType::PARCELABLE_WRITE_RETURN_VALUE);
                hasOutParams = true;
            }

            arg = arg->next;
        }
    fprintf(stderr, "%s %d\n",__func__, __LINE__);

        // return true
        c->statements->Add(new CReturnStatement(CTRUE_VALUE));
        bnsw->cases.push_back(c);
    }
    fprintf(stderr, "%s %d\n",__func__, __LINE__);

    // == the bp method ===================================================
    CMethod* proxy = new CMethod;
        proxy->comment = gather_comments(method->comments_token->extra);
        proxy->modifiers = CPUBLIC;
        proxy->returnType = CNAMES.Search(method->type.type.data);
        proxy->returnTypeDimension = method->type.dimension;
        proxy->name = method->name.data;
        proxy->statements = new CStatementBlock;
        arg = method->args;
        while (arg != NULL) {
            proxy->parameters.push_back(new CVariable(
                            CNAMES.Search(arg->type.type.data), arg->name.data,
                            arg->type.dimension));
            arg = arg->next;
        }
    bp->elements.push_back(proxy);

    // the parcels
    CVariable* _data = new CVariable(CPARCEL_TYPE, "_data");
    proxy->statements->Add(new CVariableDeclaration(_data));
    CVariable* _reply = NULL;
    if (!oneway) {
        _reply = new CVariable(CPARCEL_TYPE, "_reply");
        proxy->statements->Add(new CVariableDeclaration(_reply));
    }

    // the return value
    CVariable* _result = NULL;
    if (0 != strcmp(method->type.type.data, "void")) {
        _result = new CVariable(proxy->returnType, "_result",
                method->type.dimension);
        proxy->statements->Add(new CVariableDeclaration(_result));
    }

    // the interface identifier token: the DESCRIPTOR constant, marshalled as a string
    proxy->statements->Add(new CMethodCall(_data, "writeInterfaceToken",
            1, new CMethodCall(CTHIS_VALUE,"getInterfaceDescriptor")));

    // the parameters
    arg = method->args;
    while (arg != NULL) {
        CType* t = CNAMES.Search(arg->type.type.data);
        CVariable* v = new CVariable(t, arg->name.data, arg->type.dimension);
        int dir = convert_direction(arg->direction.data);
        if (dir & IN_PARAMETER) {
            cgenerate_write_to_parcel(t, proxy->statements, v, _data, 0);
        }
        arg = arg->next;
    }

    // the transact call
    CMethodCall* call = new CMethodCall(new CMethodCall(CTHIS_VALUE, "remote"), "transact", 4,
                            new CLiteralExpression(transactCodeName),
                            _data, _reply ? _reply : CNULL_VALUE,
                            new CLiteralExpression(
                                oneway ? "IBinder::FLAG_ONEWAY" : "0"));
    proxy->statements->Add(call);

    // throw back exceptions.
    if (_reply) {
        CMethodCall* ex = new CMethodCall(_reply, "readExceptionCode", 0);
        proxy->statements->Add(ex);
    }

    CVariable* cl = NULL;
    // returning and cleanup
    if (_reply != NULL) {
        if (_result != NULL) {
            cgenerate_create_from_parcel(proxy->returnType,
                    proxy->statements, _result, _reply, &cl);
        }

        // the out/inout parameters
        arg = method->args;
        while (arg != NULL) {
            CType* t = CNAMES.Search(arg->type.type.data);
            CVariable* v = new CVariable(t, arg->name.data, arg->type.dimension);
            if (convert_direction(arg->direction.data) & OUT_PARAMETER) {
                cgenerate_read_from_parcel(t, proxy->statements,
                                            v, _reply, &cl);
            }
            arg = arg->next;
        }

    }

    if (_result != NULL) {
        proxy->statements->Add(new CReturnStatement(_result));
    }
}

CNamespace*
cgenerate_binder_interface_class(const interface_type* iface)
{
    fprintf(stderr, "%s %d\n",__func__, __LINE__);
    CInterfaceType* interfaceType = static_cast<CInterfaceType*>(
        CNAMES.Find(package2namespace(iface->package).c_str(), iface->name.data));

    // the namespace class
    CNamespace* _namespace = new CNamespace;
        _namespace->comment = gather_comments(iface->comments_token->extra);
    // the code enum
    CEnum * eu = new CEnum;
        eu->type = CENUM_TYPE;
        eu->modifiers = CPRIVATE;

    // the interface class
    CClass* interface = new CClass;
        interface->comment = gather_comments(iface->comments_token->extra);
        interface->type = interfaceType;
        interface->modifiers = CPUBLIC;

    CClass *BnXXX = new CClass;
        BnXXX->inherit.push_back(CNAMES.Search("BnInterface"));
        BnXXX->type = CNAMES.Search(append("Bn", iface->name.data));
        BnXXX->modifiers = CPUBLIC;
        BnXXX->comment = interface->comment;
    fprintf(stderr, "%s %d bn type %p %s\n",__func__, __LINE__, BnXXX->type, append("Bn", iface->name.data).c_str());

    CClass *BpXXX = new CClass;
        BpXXX->inherit.push_back(CNAMES.Search("BpInterface"));
        BpXXX->type = CNAMES.Search(append("Bp", iface->name.data));
        BpXXX->modifiers = CPRIVATE;
        BpXXX->comment = interface->comment;
    fprintf(stderr, "%s %d bn type %p\n",__func__, __LINE__, BpXXX->type);

    _namespace->enums.push_back(eu);
    _namespace->classes.push_back(interface);
    _namespace->classes.push_back(BnXXX);
    _namespace->classes.push_back(BpXXX);

    CSwitchStatement* bn_transact_switch;
    // BnXXX init
    {
        CVariable *transact_code = new CVariable(CINT_TYPE, "code");
        CVariable *transact_data = new CVariable(CPARCEL_TYPE, "data");
        CVariable *transact_reply = new CVariable(CPARCEL_TYPE, "reply");
        CVariable *transact_flags = new CVariable(CINT_TYPE, "flags");
        CMethod* onTransact = new CMethod;
            onTransact->modifiers = CPUBLIC;
            onTransact->returnType = CBOOLEAN_TYPE;
            onTransact->name = "onTransact";
            onTransact->parameters.push_back(transact_code);
            onTransact->parameters.push_back(transact_data);
            onTransact->parameters.push_back(transact_reply);
            onTransact->parameters.push_back(transact_flags);
            onTransact->statements = new CStatementBlock;
        BnXXX->elements.push_back(onTransact);
        bn_transact_switch = new CSwitchStatement(transact_code);

        onTransact->statements->Add(bn_transact_switch);
        CMethodCall* superCall = new CMethodCall("BBinder::onTransact", 4,
                                        transact_code, transact_data,
                                        transact_reply, transact_flags);
        onTransact->statements->Add(new CReturnStatement(superCall));
    }
    // BpXXX init
    {
        CMethod* ctor = new CMethod;
        CVariable* impl = new CVariable(CSP_TEMPLATE_IBINDER_TYPE, "impl", CVariable::VAR_REF);
            ctor->name = BpXXX->type->Name();
            ctor->statements = new CStatementBlock;
            ctor->parameters.push_back(impl);
        CMethodCall* superCtor = new CMethodCall(BpXXX->inherit[0]->QualifiedName().c_str(), 1,
                                        impl);
            ctor->statements->Add(superCtor);
        BpXXX->elements.push_back(ctor);
    }

    // all the declared methods of the interface
    int index = 0;
    interface_item_type* item = iface->interface_items;
    while (item != NULL) {
        if (item->item_type == METHOD_TYPE) {
            method_type * method_item = (method_type*) item;
            fprintf(stderr, "%s %d %p %s\n",__func__, __LINE__, item, method_item->type.type.data);
            cgenerate_method(method_item, interface, eu, BnXXX, bn_transact_switch, BpXXX, method_item->assigned_id);
        }
        item = item->next;
        index++;
    }

    return _namespace;
}

