#include "CType.h"
#include "CAST.h"

void
WriteModifiers(FILE* to, int mod, int mask)
{
    int m = mod & mask;

    if ((m & SCOPE_MASK) == PUBLIC) {
        fprintf(to, "public ");
    }
    else if ((m & SCOPE_MASK) == PRIVATE) {
        fprintf(to, "private ");
    }
    else if ((m & SCOPE_MASK) == PROTECTED) {
        fprintf(to, "protected ");
    }

    if (m & STATIC) {
        fprintf(to, "static ");
    }

    if (m & VIRTUAL) {
        fprintf(to, "virtual ");
    }
}

void
WriteArgumentList(FILE* to, const vector<CExpression*>& arguments)
{
    size_t N = arguments.size();
    for (size_t i=0; i<N; i++) {
        arguments[i]->Write(to);
        if (i != N-1) {
            fprintf(to, ", ");
        }
    }
}

CClassElement::CClassElement()
{
}

CClassElement::~CClassElement()
{
}

CField::CField()
    :CClassElement(),
     modifiers(0),
     variable(NULL)
{
}

CField::CField(int m, CVariable* v)
    :CClassElement(),
     modifiers(m),
     variable(v)
{
}

CField::~CField()
{
}

void
CField::GatherTypes(set<CType*>* types) const
{
    types->insert(this->variable->type);
}

void
CField::Write(FILE* to)
{
    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }
    WriteModifiers(to, this->modifiers, SCOPE_MASK | STATIC );
    fprintf(to, "%s %s", this->variable->type->QualifiedName().c_str(),
            this->variable->name.c_str());
    if (this->value.length() != 0) {
        fprintf(to, " = %s", this->value.c_str());
    }
    fprintf(to, ";\n");
}

CExpression::~CExpression()
{
}

CLiteralExpression::CLiteralExpression(const string& v)
    :value(v)
{
}

CLiteralExpression::~CLiteralExpression()
{
}

void
CLiteralExpression::Write(FILE* to)
{
    fprintf(to, "%s", this->value.c_str());
}

CStringLiteralExpression::CStringLiteralExpression(const string& v)
    :value(v)
{
}

CStringLiteralExpression::~CStringLiteralExpression()
{
}

void
CStringLiteralExpression::Write(FILE* to)
{
    fprintf(to, "\"%s\"", this->value.c_str());
}

CVariable::CVariable()
    :type(NULL),
     name(),
     dimension(0)
{
}

CVariable::CVariable(CType* t, const string& n)
    :type(t),
     name(n),
     dimension(0)
{
}

CVariable::CVariable(CType* t, const string& n, int d)
    :type(t),
     name(n),
     dimension(d)
{
}

CVariable::~CVariable()
{
}

void
CVariable::GatherTypes(set<CType*>* types) const
{
    types->insert(this->type);
}

void
CVariable::WriteDeclaration(FILE* to)
{
    string dim;
    for (int i=0; i<this->dimension; i++) {
        dim += "[]";
    }
    fprintf(to, "%s%s %s", this->type->QualifiedName().c_str(), dim.c_str(),
            this->name.c_str());
}

void
CVariable::Write(FILE* to)
{
    fprintf(to, "%s", name.c_str());
}

CFieldVariable::CFieldVariable(CExpression* o, const string& n)
    :object(o),
     clazz(NULL),
     name(n)
{
}

CFieldVariable::CFieldVariable(CType* c, const string& n)
    :object(NULL),
     clazz(c),
     name(n)
{
}

CFieldVariable::~CFieldVariable()
{
}

void
CFieldVariable::Write(FILE* to)
{
    if (this->object != NULL) {
        this->object->Write(to);
    }
    else if (this->clazz != NULL) {
        fprintf(to, "%s", this->clazz->QualifiedName().c_str());
    }
    fprintf(to, ".%s", name.c_str());
}


CStatement::~CStatement()
{
}

CStatementBlock::CStatementBlock()
{
}

CStatementBlock::~CStatementBlock()
{
}

void
CStatementBlock::Write(FILE* to)
{
    fprintf(to, "{\n");
    int N = this->statements.size();
    for (int i=0; i<N; i++) {
        this->statements[i]->Write(to);
    }
    fprintf(to, "}\n");
}

void
CStatementBlock::Add(CStatement* statement)
{
    this->statements.push_back(statement);
}

void
CStatementBlock::Add(CExpression* expression)
{
    this->statements.push_back(new CExpressionStatement(expression));
}

CExpressionStatement::CExpressionStatement(CExpression* e)
    :expression(e)
{
}

CExpressionStatement::~CExpressionStatement()
{
}

void
CExpressionStatement::Write(FILE* to)
{
    this->expression->Write(to);
    fprintf(to, ";\n");
}

CAssignment::CAssignment(CVariable* l, CExpression* r)
    :lvalue(l),
     rvalue(r),
     cast(NULL)
{
}

CAssignment::CAssignment(CVariable* l, CExpression* r, CType* c)
    :lvalue(l),
     rvalue(r),
     cast(c)
{
}

CAssignment::~CAssignment()
{
}

void
CAssignment::Write(FILE* to)
{
    this->lvalue->Write(to);
    fprintf(to, " = ");
    if (this->cast != NULL) {
        fprintf(to, "(%s)", this->cast->QualifiedName().c_str());
    }
    this->rvalue->Write(to);
}

CMethodCall::CMethodCall(const string& n)
    :obj(NULL),
     clazz(NULL),
     name(n)
{
}

CMethodCall::CMethodCall(const string& n, int argc = 0, ...)
    :obj(NULL),
     clazz(NULL),
     name(n)
{
  va_list args;
  va_start(args, argc);
  init(argc, args);
  va_end(args);
}

CMethodCall::CMethodCall(CExpression* o, const string& n)
    :obj(o),
     clazz(NULL),
     name(n)
{
}

CMethodCall::CMethodCall(CType* t, const string& n)
    :obj(NULL),
     clazz(t),
     name(n)
{
}

CMethodCall::CMethodCall(CExpression* o, const string& n, int argc = 0, ...)
    :obj(o),
     clazz(NULL),
     name(n)
{
  va_list args;
  va_start(args, argc);
  init(argc, args);
  va_end(args);
}

CMethodCall::CMethodCall(CType* t, const string& n, int argc = 0, ...)
    :obj(NULL),
     clazz(t),
     name(n)
{
  va_list args;
  va_start(args, argc);
  init(argc, args);
  va_end(args);
}

CMethodCall::~CMethodCall()
{
}

void
CMethodCall::init(int n, va_list args)
{
    for (int i=0; i<n; i++) {
        CExpression* expression = (CExpression*)va_arg(args, void*);
        this->arguments.push_back(expression);
    }
}

void
CMethodCall::Write(FILE* to)
{
    if (this->obj != NULL) {
        this->obj->Write(to);
        fprintf(to, ".");
    }
    else if (this->clazz != NULL) {
        fprintf(to, "%s.", this->clazz->QualifiedName().c_str());
    }
    fprintf(to, "%s(", this->name.c_str());
    WriteArgumentList(to, this->arguments);
    fprintf(to, ")");
}

CComparison::CComparison(CExpression* l, const string& o, CExpression* r)
    :lvalue(l),
     op(o),
     rvalue(r)
{
}

CComparison::~CComparison()
{
}

void
CComparison::Write(FILE* to)
{
    fprintf(to, "(");
    this->lvalue->Write(to);
    fprintf(to, "%s", this->op.c_str());
    this->rvalue->Write(to);
    fprintf(to, ")");
}

CNewExpression::CNewExpression(CType* t)
    :type(t)
{
}

CNewExpression::CNewExpression(CType* t, int argc = 0, ...)
    :type(t)
{
  va_list args;
  va_start(args, argc);
  init(argc, args);
  va_end(args);
}

CNewExpression::~CNewExpression()
{
}

void
CNewExpression::init(int n, va_list args)
{
    for (int i=0; i<n; i++) {
        CExpression* expression = (CExpression*)va_arg(args, void*);
        this->arguments.push_back(expression);
    }
}

void
CNewExpression::Write(FILE* to)
{
    fprintf(to, "new %s(", this->type->InstantiableName().c_str());
    WriteArgumentList(to, this->arguments);
    fprintf(to, ")");
}

CNewArrayExpression::CNewArrayExpression(CType* t, CExpression* s)
    :type(t),
     size(s)
{
}

CNewArrayExpression::~CNewArrayExpression()
{
}

void
CNewArrayExpression::Write(FILE* to)
{
    fprintf(to, "new %s[", this->type->QualifiedName().c_str());
    size->Write(to);
    fprintf(to, "]");
}

CTernary::CTernary()
    :condition(NULL),
     ifpart(NULL),
     elsepart(NULL)
{
}

CTernary::CTernary(CExpression* a, CExpression* b, CExpression* c)
    :condition(a),
     ifpart(b),
     elsepart(c)
{
}

CTernary::~CTernary()
{
}

void
CTernary::Write(FILE* to)
{
    fprintf(to, "((");
    this->condition->Write(to);
    fprintf(to, ")?(");
    this->ifpart->Write(to);
    fprintf(to, "):(");
    this->elsepart->Write(to);
    fprintf(to, "))");
}

CCast::CCast()
    :type(NULL),
     expression(NULL)
{
}

CCast::CCast(CType* t, CExpression* e)
    :type(t),
     expression(e)
{
}

CCast::~CCast()
{
}

void
CCast::Write(FILE* to)
{
    fprintf(to, "((%s)", this->type->QualifiedName().c_str());
    expression->Write(to);
    fprintf(to, ")");
}

CVariableDeclaration::CVariableDeclaration(CVariable* l, CExpression* r, CType* c)
    :lvalue(l),
     cast(c),
     rvalue(r)
{
}

CVariableDeclaration::CVariableDeclaration(CVariable* l)
    :lvalue(l),
     cast(NULL),
     rvalue(NULL)
{
}

CVariableDeclaration::~CVariableDeclaration()
{
}

void
CVariableDeclaration::Write(FILE* to)
{
    this->lvalue->WriteDeclaration(to);
    if (this->rvalue != NULL) {
        fprintf(to, " = ");
        if (this->cast != NULL) {
            fprintf(to, "(%s)", this->cast->QualifiedName().c_str());
        }
        this->rvalue->Write(to);
    }
    fprintf(to, ";\n");
}

CIfStatement::CIfStatement()
    :expression(NULL),
     statements(new CStatementBlock),
     elseif(NULL)
{
}

CIfStatement::~CIfStatement()
{
}

void
CIfStatement::Write(FILE* to)
{
    if (this->expression != NULL) {
        fprintf(to, "if (");
        this->expression->Write(to);
        fprintf(to, ") ");
    }
    this->statements->Write(to);
    if (this->elseif != NULL) {
        fprintf(to, "else ");
        this->elseif->Write(to);
    }
}

CReturnStatement::CReturnStatement(CExpression* e)
    :expression(e)
{
}

CReturnStatement::~CReturnStatement()
{
}

void
CReturnStatement::Write(FILE* to)
{
    fprintf(to, "return ");
    this->expression->Write(to);
    fprintf(to, ";\n");
}

CTryStatement::CTryStatement()
    :statements(new CStatementBlock)
{
}

CTryStatement::~CTryStatement()
{
}

void
CTryStatement::Write(FILE* to)
{
    fprintf(to, "try ");
    this->statements->Write(to);
}

CCatchStatement::CCatchStatement(CVariable* e)
    :statements(new CStatementBlock),
     exception(e)
{
}

CCatchStatement::~CCatchStatement()
{
}

void
CCatchStatement::Write(FILE* to)
{
    fprintf(to, "catch ");
    if (this->exception != NULL) {
        fprintf(to, "(");
        this->exception->WriteDeclaration(to);
        fprintf(to, ") ");
    }
    this->statements->Write(to);
}

CCase::CCase()
    :statements(new CStatementBlock)
{
}

CCase::CCase(const string& c)
    :statements(new CStatementBlock)
{
    cases.push_back(c);
}

CCase::~CCase()
{
}

void
CCase::Write(FILE* to)
{
    int N = this->cases.size();
    if (N > 0) {
        for (int i=0; i<N; i++) {
            string s = this->cases[i];
            if (s.length() != 0) {
                fprintf(to, "case %s:\n", s.c_str());
            } else {
                fprintf(to, "default:\n");
            }
        }
    } else {
        fprintf(to, "default:\n");
    }
    statements->Write(to);
}

CSwitchStatement::CSwitchStatement(CExpression* e)
    :expression(e)
{
}

CSwitchStatement::~CSwitchStatement()
{
}

void
CSwitchStatement::Write(FILE* to)
{
    fprintf(to, "switch (");
    this->expression->Write(to);
    fprintf(to, ")\n{\n");
    int N = this->cases.size();
    for (int i=0; i<N; i++) {
        this->cases[i]->Write(to);
    }
    fprintf(to, "}\n");
}

CBreak::CBreak()
{
}

CBreak::~CBreak()
{
}

void
CBreak::Write(FILE* to)
{
    fprintf(to, "break;\n");
}

CMethod::CMethod()
    :CClassElement(),
     modifiers(0),
     returnType(NULL), // (NULL means constructor)
     returnTypeDimension(0),
     statements(NULL),
     _virtual(false)
{
}

CMethod::~CMethod()
{
}

void
CMethod::GatherTypes(set<CType*>* types) const
{
    size_t N, i;

    if (this->returnType) {
        types->insert(this->returnType);
    }

    N = this->parameters.size();
    for (i=0; i<N; i++) {
        this->parameters[i]->GatherTypes(types);
    }

    N = this->exceptions.size();
    for (i=0; i<N; i++) {
        types->insert(this->exceptions[i]);
    }
}

void
CMethod::Write(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }

    WriteModifiers(to, this->modifiers, SCOPE_MASK | STATIC | VIRTUAL);

    if (this->returnType != NULL) {
        string dim;
        for (i=0; i<this->returnTypeDimension; i++) {
            dim += "[]";
        }
        fprintf(to, "%s%s ", this->returnType->QualifiedName().c_str(),
                dim.c_str());
    }
   
    fprintf(to, "%s(", this->name.c_str());

    N = this->parameters.size();
    for (i=0; i<N; i++) {
        this->parameters[i]->WriteDeclaration(to);
        if (i != N-1) {
            fprintf(to, ", ");
        }
    }

    fprintf(to, ")");

    N = this->exceptions.size();
    for (i=0; i<N; i++) {
        if (i == 0) {
            fprintf(to, " throws ");
        } else {
            fprintf(to, ", ");
        }
        fprintf(to, "%s", this->exceptions[i]->QualifiedName().c_str());
    }

    if (this->statements == NULL) {
        fprintf(to, ";\n");
    } else {
        fprintf(to, "\n");
        this->statements->Write(to);
    }
}

CClass::CClass()
    :modifiers(0),
     type(NULL)
{
}

CClass::~CClass()
{
}

void
CClass::GatherTypes(set<CType*>* types) const
{
    int N, i;

    types->insert(this->type);
    N = this->inherit.size();
    for (i=0; i<N; i++) {
        types->insert(this->inherit[i]);
    }

    N = this->elements.size();
    for (i=0; i<N; i++) {
        this->elements[i]->GatherTypes(types);
    }
}

void
CClass::Write(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }

    WriteModifiers(to, this->modifiers, ALL_MODIFIERS);

    fprintf(to, "class ");

    string name = this->type->Name();
    size_t pos = name.rfind('.');
    if (pos != string::npos) {
        name = name.c_str() + pos + 1;
    }

    fprintf(to, "%s", name.c_str());


    N = this->inherit.size();

    if (N != 0) {
        for (i=0; i < N -1; i++) {
            fprintf(to, " public %s,", this->inherit[i]->QualifiedName().c_str());
        }
        for (; i < N; i++) {
            fprintf(to, " public %s", this->inherit[i]->QualifiedName().c_str());
        }
    }

    fprintf(to, "\n");
    fprintf(to, "{\n");

    N = this->elements.size();
    for (i=0; i<N; i++) {
        this->elements[i]->Write(to);
    }

    fprintf(to, "}\n");

}

CDocument::CDocument()
{
}

CDocument::~CDocument()
{
}

static string
escape_backslashes(const string& str)
{
    string result;
    const size_t I=str.length();
    for (size_t i=0; i<I; i++) {
        char c = str[i];
        if (c == '\\') {
            result += "\\\\";
        } else {
            result += c;
        }
    }
    return result;
}

void
CDocument::Write(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }
    fprintf(to, "/*\n"
                " * This file is auto-generated.  DO NOT MODIFY.\n"
                " * Original file: %s\n"
                " */\n", escape_backslashes(this->originalSrc).c_str());
    if (this->_namespace.length() != 0) {
        fprintf(to, "namespace %s{\n", this->_namespace.c_str());
    }

    N = this->classes.size();
    for (i=0; i<N; i++) {
        CClass* c = this->classes[i];
        c->Write(to);
    }
    if (this->_namespace.length() != 0) {
        fprintf(to, "};\n");
    }
}

