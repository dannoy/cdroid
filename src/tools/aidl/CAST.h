#ifndef AIDL_CAST_H
#define AIDL_CAST_H

#include <string>
#include <vector>
#include <set>
#include <stdarg.h>
#include <stdio.h>


using namespace std;

class CType;

enum {
    CPUBLIC          = 0x00000001,
    CPRIVATE         = 0x00000002,
    CPROTECTED       = 0x00000003,
    CSCOPE_MASK      = 0x00000003,

    CSTATIC          = 0x00000010,
    CVIRTUAL         = 0x00000020,
    CPUREVIRTUAL     = 0x00000060,

    CSTICK_TO_HEADER = 0x00000100,
    CSTICK_TO_SOURCE = 0x00000200,
    CSTICK_MASK      = 0x00000f00,

    CALL_MODIFIERS   = 0xffffffff
};

// Write the modifiers that are set in both mod and mask
void WriteModifiers(FILE* to, int mod, int mask);

struct CClassElement
{
    int modifiers;
    CClassElement(int m);
    virtual ~CClassElement();

    virtual void GatherTypes(set<CType*>* types) const = 0;
    virtual void WriteToHeader(FILE* to) = 0;
    virtual void WriteToSource(FILE* to) = 0;
};

struct CExpression
{
    string comment;
    virtual ~CExpression();
    virtual void Write(FILE* to) = 0;
};

struct CLiteralExpression : public CExpression
{
    string value;

    CLiteralExpression(const string& value);
    virtual ~CLiteralExpression();
    virtual void Write(FILE* to);
};

// TODO: also escape the contents.  not needed for now
struct CStringLiteralExpression : public CExpression
{
    string value;

    CStringLiteralExpression(const string& value);
    virtual ~CStringLiteralExpression();
    virtual void Write(FILE* to);
};


struct CVariable : public CExpression
{
    enum CVARIABLE_TYPE{
        VAR_POINTER,
        VAR_REF,
        VAR_VALUE
    };
    CType* type;
    string name;
    int dimension;
    int val_type;
    bool isConst;

    CVariable();
    CVariable(CType* type, const string& name, enum CVARIABLE_TYPE _type = VAR_VALUE, bool is_const = false);
    CVariable(CType* type, const string& name, int dimension, enum CVARIABLE_TYPE _type = VAR_VALUE, bool is_const = false);
    virtual ~CVariable();

    virtual void GatherTypes(set<CType*>* types) const;
    void WriteDeclaration(FILE* to);
    void Write(FILE* to);
};

struct CMethodCall;
struct CMethodVariable: public CExpression
{
    enum MethodVarType{
        MVAR_POINTER            = 0x1,
        MVAR_OBJECT_POINTER     = 0x3,
        MVAR_REF                = 0x4,
        MVAR_VALUE              = 0x8
    };
    CVariable *var;
    CMethodCall *mvar;
    MethodVarType mv_type;

    CMethodVariable(CVariable *v, MethodVarType t = MVAR_VALUE);
    CMethodVariable(CMethodCall *v, MethodVarType t = MVAR_VALUE);
    virtual ~CMethodVariable();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void Write(FILE* to);
};


struct CFieldVariable : public CExpression
{
    CExpression* object;
    CType* clazz;
    string name;
    bool isStatic;

    CFieldVariable(CExpression* object, const string& name);
    CFieldVariable(CType* clazz, const string& name);
    virtual ~CFieldVariable();

    void Write(FILE* to);
};

struct CStatement
{
    virtual ~CStatement();
    virtual void Write(FILE* to) = 0;
};

struct CStatementBlock : public CStatement
{
    vector<CStatement*> statements;

    CStatementBlock();
    virtual ~CStatementBlock();
    virtual void Write(FILE* to);

    void Add(CStatement* cstatement);
    void Add(CExpression* cexpression);
};

struct CExpressionStatement : public CStatement
{
    CExpression* expression;

    CExpressionStatement(CExpression* cexpression);
    virtual ~CExpressionStatement();
    virtual void Write(FILE* to);
};

struct CAssignment : public CExpression
{
    CVariable* lvalue;
    CExpression* rvalue;
    CType* cast;

    CAssignment(CVariable* lvalue, CExpression* rvalue);
    CAssignment(CVariable* lvalue, CExpression* rvalue, CType* cast);
    virtual ~CAssignment();
    virtual void Write(FILE* to);
};

struct CMethodCall : public CExpression
{
    CExpression* obj;
    CType* clazz;
    string name;
    vector<CExpression*> arguments;
    vector<string> exceptions;

    CMethodCall(const string& name);
    CMethodCall(const string& name, int argc, ...);
    CMethodCall(CExpression* obj, const string& name);
    CMethodCall(CType* clazz, const string& name);
    CMethodCall(CExpression* obj, const string& name, int argc, ...);
    CMethodCall(CType* clazz, const string& name, int argc, ...);
    virtual ~CMethodCall();
    virtual void Write(FILE* to);

private:
    void init(int n, va_list args);
};

struct CComparison : public CExpression
{
    CExpression* lvalue;
    string op;
    CExpression* rvalue;

    CComparison(CExpression* lvalue, const string& op, CExpression* rvalue);
    virtual ~CComparison();
    virtual void Write(FILE* to);
};

struct CNewExpression : public CExpression
{
    CType* type;
    vector<CExpression*> arguments;

    CNewExpression(CType* type);
    CNewExpression(CType* type, int argc, ...);
    virtual ~CNewExpression();
    virtual void Write(FILE* to);

private:
    void init(int n, va_list args);
};

struct CNewArrayExpression : public CExpression
{
    CType* type;
    CExpression* size;

    CNewArrayExpression(CType* type, CExpression* size);
    virtual ~CNewArrayExpression();
    virtual void Write(FILE* to);
};

struct CTernary : public CExpression
{
    CExpression* condition;
    CExpression* ifpart;
    CExpression* elsepart;

    CTernary();
    CTernary(CExpression* condition, CExpression* ifpart, CExpression* elsepart);
    virtual ~CTernary();
    virtual void Write(FILE* to);
};

struct CCast : public CExpression
{
    CType* type;
    CExpression* expression;

    CCast();
    CCast(CType* type, CExpression* cexpression);
    virtual ~CCast();
    virtual void Write(FILE* to);
};

struct CVariableDeclaration : public CStatement
{
    CVariable* lvalue;
    CType* cast;
    CExpression* rvalue;

    CVariableDeclaration(CVariable* lvalue);
    CVariableDeclaration(CVariable* lvalue, CExpression* rvalue, CType* cast = NULL);
    virtual ~CVariableDeclaration();
    virtual void Write(FILE* to);
};

struct CIfStatement : public CStatement
{
    CExpression* expression;
    CStatementBlock* statements;
    CIfStatement* elseif;

    CIfStatement();
    virtual ~CIfStatement();
    virtual void Write(FILE* to);
};

struct CReturnStatement : public CStatement
{
    CExpression* expression;

    CReturnStatement(CExpression* expression);
    virtual ~CReturnStatement();
    virtual void Write(FILE* to);
};

struct CTryStatement : public CStatement
{
    CStatementBlock* statements;

    CTryStatement();
    virtual ~CTryStatement();
    virtual void Write(FILE* to);
};

struct CCatchStatement : public CStatement
{
    CStatementBlock* statements;
    CVariable* exception;

    CCatchStatement(CVariable* e);
    virtual ~CCatchStatement();
    virtual void Write(FILE* to);
};

struct CCase
{
    vector<string> cases;
    CStatementBlock* statements;

    CCase();
    CCase(const string& c);
    virtual ~CCase();
    virtual void Write(FILE* to);
};

struct CSwitchStatement : public CStatement
{
    CExpression* expression;
    vector<CCase*> cases;

    CSwitchStatement(CExpression* cexpression);
    virtual ~CSwitchStatement();
    virtual void Write(FILE* to);
};

struct CBreak : public CStatement
{
    CBreak();
    virtual ~CBreak();
    virtual void Write(FILE* to);
};

struct CFunction : public CStatement
{
    string comment;
    CType* returnType;
    size_t returnTypeDimension;
    string name;
    vector<CVariable*> parameters;
    vector<CType*> exceptions;
    CStatementBlock* statements;

    CFunction();
    virtual ~CFunction();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void Write(FILE* to);
};

struct CClass : public CClassElement
{
    string comment;
    CType* type;
    vector<CType*> inherit;
    vector<CClassElement*> elements;
    CClass *parent;

    CClass();
    virtual ~CClass();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void WriteToHeader(FILE* to);
    virtual void WriteToSource(FILE* to);
private:
    void WriteClassBegin(FILE* to);
    void WriteClassEnd(FILE* to);
};

struct CMethod : public CClassElement
{
    string comment;
    CType* returnType;
    string name;
    vector<CVariable*> parameters;
    vector<CType*> exceptions;
    CStatementBlock* statements;
    vector<pair<CVariable *,CExpression *>* > initial_list;
    CClass *parent;

    CMethod();
    virtual ~CMethod();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void WriteToHeader(FILE* to);
    virtual void WriteToSource(FILE* to);
private:
    void WriteDeclaration(FILE* to, bool definition);
};

struct CField : public CClassElement
{
    string comment;
    CVariable *variable;
    string value;
    CClass *parent;

    CField();
    CField(int modifiers, CVariable* variable);
    virtual ~CField();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void WriteToHeader(FILE* to);
    virtual void WriteToSource(FILE* to);
};


struct CEnumElement
{
    string name;
    string value;
    CEnumElement();
    CEnumElement(string name, string value);
    virtual ~CEnumElement();
    virtual void GatherTypes(set<CType*>* types) const;
    virtual void Write(FILE* to);
};


struct CEnum
{
    vector<CEnumElement *> elements;
    string name;
    CType *type;
    int modifiers;

    CEnum();
    virtual ~CEnum();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void WriteToHeader(FILE* to);
    virtual void WriteToSource(FILE* to);
};


struct CNamespace
{
    vector<CClass *> classes;
    vector<CFunction *> functions;
    vector<CEnum *> enums;
    string comment;
    virtual void WriteToHeader(FILE* to);
    virtual void WriteToSource(FILE* to);
};

struct CDocument
{
    string comment;
    string _namespace;
    string header_file;
    string source_file;
    string originalSrc;
    set<string> includes;
    vector<CClass*> classes;
    vector<CNamespace*> nss;

    CDocument();
    virtual ~CDocument();

    virtual void WriteToHeader(FILE* to);
    virtual void WriteToSource(FILE* to);
};

#endif // AIDL_CAST_H
