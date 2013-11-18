#include "generate_cpp.h"
#include "Type.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =================================================
static void
generate_write_to_parcel(Type* t, StatementBlock* addTo, Variable* v,
                            Variable* parcel, int flags)
{
    if (v->dimension == 0) {
        t->WriteToParcel(addTo, v, parcel, flags);
    }
    if (v->dimension == 1) {
        t->WriteArrayToParcel(addTo, v, parcel, flags);
    }
}

static void
generate_create_from_parcel(Type* t, StatementBlock* addTo, Variable* v,
                            Variable* parcel, Variable** cl)
{
    if (v->dimension == 0) {
        t->CreateFromParcel(addTo, v, parcel, cl);
    }
    if (v->dimension == 1) {
        t->CreateArrayFromParcel(addTo, v, parcel, cl);
    }
}

static void
generate_read_from_parcel(Type* t, StatementBlock* addTo, Variable* v,
                            Variable* parcel, Variable** cl)
{
    if (v->dimension == 0) {
        t->ReadFromParcel(addTo, v, parcel, cl);
    }
    if (v->dimension == 1) {
        t->ReadArrayFromParcel(addTo, v, parcel, cl);
    }
}


static void
cgenerate_method(const method_type* method, Class* interface,
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
    sprintf(transactCodeValue, "(IBinder.FIRST_CALL_TRANSACTION + %d)", index);

    CEnumElement e
    eu.element.push_back(new CEnumElement(transactionCodeName,
                            string(transactionCodeValue)));

    // == the declaration in the interface ===================================
    Method* decl = new Method;
        decl->comment = gather_comments(method->comments_token->extra);
        decl->modifiers = PUBLIC;
        decl->returnType = NAMES.Search(method->type.type.data);
        decl->returnTypeDimension = method->type.dimension;
        decl->name = method->name.data;

    arg = method->args;
    while (arg != NULL) {
        decl->parameters.push_back(new Variable(
                            CNAMES.Search(arg->type.type.data), arg->name.data,
                            arg->type.dimension));
        arg = arg->next;
    }

    interface->elements.push_back(decl);

    // == the BN method ====================================================
    {
        CVariable *transact_code = new Variable(INT_TYPE, "code");
        CVariable *transact_data = new Variable(PARCEL_TYPE, "data");
        CVariable *transact_reply = new Variable(PARCEL_TYPE, "reply");
        CVariable *transact_flags = new Variable(INT_TYPE, "flags");
        CVariable *dummy = new Variable(INT_TYPE, interface->type->Name());

        Case* c = new Case(transactCodeName);

        MethodCall* realCall = new MethodCall(THIS_VALUE, method->name.data);

        // interface token validation is the very first thing we do
        c->statements->Add(new CMethodCall("CHECK_INTERFACE",
                        3, dummy, transact_data, transact_reply));

        // args
        Variable* cl = NULL;
        VariableFactory stubArgs("_arg");
        arg = method->args;
        while (arg != NULL) {
            Type* t = NAMES.Search(arg->type.type.data);
            Variable* v = stubArgs.Get(t);
            v->dimension = arg->type.dimension;

            c->statements->Add(new VariableDeclaration(v));

            if (convert_direction(arg->direction.data) & IN_PARAMETER) {
                generate_create_from_parcel(t, c->statements, v,
                        stubClass->transact_data, &cl);
            } else {
                if (arg->type.dimension == 0) {
                    c->statements->Add(new Assignment(v, new NewExpression(v->type)));
                }
                else if (arg->type.dimension == 1) {
                    generate_new_array(v->type, c->statements, v,
                            stubClass->transact_data);
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
        Variable* _result = NULL;
        if (0 == strcmp(method->type.type.data, "void")) {
            c->statements->Add(realCall);

            if (!oneway) {
                // report that there were no exceptions
                MethodCall* ex = new MethodCall(stubClass->transact_reply,
                        "writeNoException", 0);
                c->statements->Add(ex);
            }
        } else {
            _result = new Variable(decl->returnType, "_result",
                                    decl->returnTypeDimension);
            c->statements->Add(new VariableDeclaration(_result, realCall));

            if (!oneway) {
                // report that there were no exceptions
                MethodCall* ex = new MethodCall(stubClass->transact_reply,
                        "writeNoException", 0);
                c->statements->Add(ex);
            }

            // marshall the return value
            generate_write_to_parcel(decl->returnType, c->statements, _result,
                                        stubClass->transact_reply,
                                        Type::PARCELABLE_WRITE_RETURN_VALUE);
        }

        // out parameters
        i = 0;
        arg = method->args;
        while (arg != NULL) {
            Type* t = NAMES.Search(arg->type.type.data);
            Variable* v = stubArgs.Get(i++);

            if (convert_direction(arg->direction.data) & OUT_PARAMETER) {
                generate_write_to_parcel(t, c->statements, v,
                                    stubClass->transact_reply,
                                    Type::PARCELABLE_WRITE_RETURN_VALUE);
                hasOutParams = true;
            }

            arg = arg->next;
        }

        // return true
        c->statements->Add(new ReturnStatement(TRUE_VALUE));
        stubClass->transact_switch->cases.push_back(c);
    }

    // == the proxy method ===================================================
    Method* proxy = new Method;
        proxy->comment = gather_comments(method->comments_token->extra);
        proxy->modifiers = PUBLIC | OVERRIDE;
        proxy->returnType = NAMES.Search(method->type.type.data);
        proxy->returnTypeDimension = method->type.dimension;
        proxy->name = method->name.data;
        proxy->statements = new StatementBlock;
        arg = method->args;
        while (arg != NULL) {
            proxy->parameters.push_back(new Variable(
                            NAMES.Search(arg->type.type.data), arg->name.data,
                            arg->type.dimension));
            arg = arg->next;
        }
        proxy->exceptions.push_back(REMOTE_EXCEPTION_TYPE);
    proxyClass->elements.push_back(proxy);

    // the parcels
    Variable* _data = new Variable(PARCEL_TYPE, "_data");
    proxy->statements->Add(new VariableDeclaration(_data,
                                new MethodCall(PARCEL_TYPE, "obtain")));
    Variable* _reply = NULL;
    if (!oneway) {
        _reply = new Variable(PARCEL_TYPE, "_reply");
        proxy->statements->Add(new VariableDeclaration(_reply,
                                    new MethodCall(PARCEL_TYPE, "obtain")));
    }

    // the return value
    _result = NULL;
    if (0 != strcmp(method->type.type.data, "void")) {
        _result = new Variable(proxy->returnType, "_result",
                method->type.dimension);
        proxy->statements->Add(new VariableDeclaration(_result));
    }

    // try and finally
    TryStatement* tryStatement = new TryStatement();
    proxy->statements->Add(tryStatement);
    FinallyStatement* finallyStatement = new FinallyStatement();
    proxy->statements->Add(finallyStatement);

    // the interface identifier token: the DESCRIPTOR constant, marshalled as a string
    tryStatement->statements->Add(new MethodCall(_data, "writeInterfaceToken",
            1, new LiteralExpression("DESCRIPTOR")));

    // the parameters
    arg = method->args;
    while (arg != NULL) {
        Type* t = NAMES.Search(arg->type.type.data);
        Variable* v = new Variable(t, arg->name.data, arg->type.dimension);
        int dir = convert_direction(arg->direction.data);
        if (dir == OUT_PARAMETER && arg->type.dimension != 0) {
            IfStatement* checklen = new IfStatement();
            checklen->expression = new Comparison(v, "==", NULL_VALUE);
            checklen->statements->Add(new MethodCall(_data, "writeInt", 1,
                        new LiteralExpression("-1")));
            checklen->elseif = new IfStatement();
            checklen->elseif->statements->Add(new MethodCall(_data, "writeInt",
                        1, new FieldVariable(v, "length")));
            tryStatement->statements->Add(checklen);
        }
        else if (dir & IN_PARAMETER) {
            generate_write_to_parcel(t, tryStatement->statements, v, _data, 0);
        }
        arg = arg->next;
    }

    // the transact call
    MethodCall* call = new MethodCall(proxyClass->mRemote, "transact", 4,
                            new LiteralExpression("Stub." + transactCodeName),
                            _data, _reply ? _reply : NULL_VALUE,
                            new LiteralExpression(
                                oneway ? "android.os.IBinder.FLAG_ONEWAY" : "0"));
    tryStatement->statements->Add(call);

    // throw back exceptions.
    if (_reply) {
        MethodCall* ex = new MethodCall(_reply, "readException", 0);
        tryStatement->statements->Add(ex);
    }

    // returning and cleanup
    if (_reply != NULL) {
        if (_result != NULL) {
            generate_create_from_parcel(proxy->returnType,
                    tryStatement->statements, _result, _reply, &cl);
        }

        // the out/inout parameters
        arg = method->args;
        while (arg != NULL) {
            Type* t = NAMES.Search(arg->type.type.data);
            Variable* v = new Variable(t, arg->name.data, arg->type.dimension);
            if (convert_direction(arg->direction.data) & OUT_PARAMETER) {
                generate_read_from_parcel(t, tryStatement->statements,
                                            v, _reply, &cl);
            }
            arg = arg->next;
        }

        finallyStatement->statements->Add(new MethodCall(_reply, "recycle"));
    }
    finallyStatement->statements->Add(new MethodCall(_data, "recycle"));

    if (_result != NULL) {
        proxy->statements->Add(new ReturnStatement(_result));
    }
}

CNamespace*
generate_binder_interface_class(const interface_type* iface)
{
    CInterfaceType* interfaceType = static_cast<InterfaceType*>(
        CNAMES.Find(package2namespace(iface->package), iface->name.data));

    // the namespace class
    CNamespace* _namespace = new CNamespace;
        interface->comment = gather_comments(iface->comments_token->extra);
    // the code enum
    CEnum * eu = new CEnum;
    eu->type = CENUM_TYPE;

    // the interface class
    CClass* interface = new CClass;
    //interface->comment = gather_comments(iface->comments_token->extra);
        interface->type = interfaceType;

    CClass *BnXXX = new CClass;
        BnXXX.inherit.push_back(CNAME.Search("BnInterface"));
        BnXXX.type = CNAME.Find(append("Bn" + c->name.data));

    CClass *BpXXX = new CClass;
        BpXXX.inherit.push_back(CNAME.Search("BpInterface"));
        BpXXX.type = CNAME.Find(append("Bp" + c->name.data));

    _namespace.enums.push_back(eu);
    _namespace.classes.push_back(interface);
    _namespace.classes.push_back(BnXXX);
    _namespace.classes.push_back(BpXXX);

    CSwitchStatement* bn_transact_switch;
    // BnXXX init
    {
        CVariable *transact_code = new Variable(INT_TYPE, "code");
        CVariable *transact_data = new Variable(PARCEL_TYPE, "data");
        CVariable *transact_reply = new Variable(PARCEL_TYPE, "reply");
        CVariable *transact_flags = new Variable(INT_TYPE, "flags");
        CMethod* onTransact = new Method;
            onTransact->modifiers = PUBLIC;
            onTransact->returnType = CBOOLEAN_TYPE;
            onTransact->name = "onTransact";
            onTransact->parameters.push_back(transact_code);
            onTransact->parameters.push_back(transact_data);
            onTransact->parameters.push_back(transact_reply);
            onTransact->parameters.push_back(transact_flags);
            onTransact->statements = new StatementBlock;
        BnXXX->elements.push_back(onTransact);
        bn_transact_switch = new CSwitchStatement(transact_code);

        onTransact->statements->Add(bn_transact_switch);
        CMethodCall* superCall = new CMethodCall("BBinder::onTransact", 4,
                                        transact_code, transact_data,
                                        transact_reply, transact_flags);
        onTransact->statements->Add(new ReturnStatement(superCall));
    }
    // BpXXX init
    {
        Method* ctor = new Method;
        Variable* impl = new Variable(CSP_TEMPLATE_BINDER_TYPE, "impl", CVariable::VAR_REF);
            ctor->name = BpXXX.type->Name();
            ctor->statements = new StatementBlock;
            ctor->parameters.push_back(impl);
        CMethodCall* superCtor = new MethodCall(BpXXX->inherit[0]->QualifiedName().c_str(), 1,
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
            cgenerate_method(method_item, interface, eu, BnXXX, bn_transaction_switch, BpXXX, method_item->assigned_id);
        }
        item = item->next;
        index++;
    }

    return interface;
}

