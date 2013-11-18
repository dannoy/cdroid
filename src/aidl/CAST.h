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
    PUBLIC          = 0x00000001,
    PRIVATE         = 0x00000002,
    PROTECTED       = 0x00000003,
    SCOPE_MASK      = 0x00000003,

    STATIC          = 0x00000010,
    VIRTUAL         = 0x00000020,

    ALL_MODIFIERS   = 0xffffffff
};

// Write the modifiers that are set in both mod and mask
void WriteModifiers(FILE* to, int mod, int mask);

struct CClassElement
{
    CClassElement();
    virtual ~CClassElement();

    virtual void GatherTypes(set<CType*>* types) const = 0;
    virtual void Write(FILE* to) = 0;
};

struct CExpression
{
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
    CType* type;
    string name;
    int dimension;

    CVariable();
    CVariable(CType* type, const string& name);
    CVariable(CType* type, const string& name, int dimension);
    virtual ~CVariable();

    virtual void GatherTypes(set<CType*>* types) const;
    void WriteDeclaration(FILE* to);
    void Write(FILE* to);
};

struct CFieldVariable : public CExpression
{
    CExpression* object;
    CType* clazz;
    string name;

    CFieldVariable(CExpression* object, const string& name);
    CFieldVariable(CType* clazz, const string& name);
    virtual ~CFieldVariable();

    void Write(FILE* to);
};

struct CField : public CClassElement
{
    string comment;
    int modifiers;
    CVariable *variable;
    string value;

    CField();
    CField(int modifiers, CVariable* variable);
    virtual ~CField();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void Write(FILE* to);
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

struct CMethod : public CClassElement
{
    string comment;
    int modifiers;
    CType* returnType;
    size_t returnTypeDimension;
    string name;
    vector<CVariable*> parameters;
    vector<CType*> exceptions;
    CStatementBlock* statements;
    bool _virtual; //lijin

    CMethod();
    virtual ~CMethod();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void Write(FILE* to);
};


struct CEnum
{
    vector<CAssignment *> elements;
    CType *type;

    CEnum();
    virtual ~CEnum();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void Write(FILE* to);
}

struct CClass : public CClassElement
{
    string comment;
    int modifiers;
    CType* type;
    vector<CType*> inherit;
    vector<CClassElement*> elements;

    CClass();
    virtual ~CClass();

    virtual void GatherTypes(set<CType*>* types) const;
    virtual void Write(FILE* to);
};

struct CNamespace
{
    vector<CClass *> classes;
    vector<CFunction *> functions;
    vector<CEnum *> enums;
    string comment;
}

struct CDocument
{
    string comment;
    string _namespace;
    string originalSrc;
    set<string> includes;
    vector<CClass*> classes;

    CDocument();
    virtual ~CDocument();

    virtual void Write(FILE* to);
};

#endif // AIDL_CAST_H
