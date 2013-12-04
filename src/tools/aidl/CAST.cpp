#include "CType.h"
#include "CAST.h"
#include "aidl_common.h"

void
CWriteModifiers(FILE* to, int mod, int mask)
{
    int m = mod & mask;

    if ((m & CSCOPE_MASK) == CPUBLIC) {
        fprintf(to, "public:\n");
    }
    else if ((m & CSCOPE_MASK) == CPRIVATE) {
        fprintf(to, "private:\n");
    }
    else if ((m & CSCOPE_MASK) == CPROTECTED) {
        fprintf(to, "protected:\n");
    }

    if (m & CSTATIC) {
        fprintf(to, "static ");
    }

    if (m & CVIRTUAL) {
        fprintf(to, "virtual ");
    }
}

void
CWriteArgumentList(FILE* to, const vector<CExpression*>& arguments)
{
    size_t N = arguments.size();
    for (size_t i=0; i<N; i++) {
        arguments[i]->Write(to);
        if (i != N-1) {
            fprintf(to, ", ");
        }
    }
}

CClassElement::CClassElement(int m)
    : modifiers(m)
{
}

CClassElement::~CClassElement()
{
}

CField::CField()
    :CClassElement(0),
     variable(NULL),
     parent(NULL)
{
}

CField::CField(int m, CVariable* v)
    :CClassElement(m),
     variable(v),
     parent(NULL)
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
CField::WriteToHeader(FILE* to)
{
    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }
    CWriteModifiers(to, this->modifiers, CSCOPE_MASK | CSTATIC );
    fprintf(to, "%s %s", this->variable->type->QualifiedName().c_str(),
            this->variable->name.c_str());
    if (this->value.length() != 0) {
        fprintf(to, " = %s", this->value.c_str());
    }
    fprintf(to, ";\n");
}

void
CField::WriteToSource(FILE* to)
{
    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }
    CWriteModifiers(to, this->modifiers, CSCOPE_MASK | CSTATIC );
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
     dimension(0),
     isConst(false),
     val_type(VAR_VALUE)
{
}

CVariable::CVariable(CType* t, const string& n, enum CVARIABLE_TYPE _type, bool is_const)
    :type(t),
     name(n),
     dimension(0),
     isConst(is_const),
     val_type(_type)
{
}

CVariable::CVariable(CType* t, const string& n, int d, enum CVARIABLE_TYPE _type, bool is_const)
    :type(t),
     name(n),
     dimension(d),
     isConst(is_const),
     val_type(_type)
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
    //TODO:Translate to vector when dimension > 1
    if(this->type){
        switch(val_type) {
            case VAR_VALUE:
                fprintf(to, "%s%s %s", isConst ? "const ":"", this->type->QualifiedName().c_str(),
                        this->name.c_str());
                break;
            case VAR_REF:
                fprintf(to, "%s%s& %s", isConst ? "const ":"", this->type->QualifiedName().c_str(),
                        this->name.c_str());
                break;
            case VAR_POINTER:
                fprintf(to, "%s%s* %s", isConst ? "const " : "", this->type->QualifiedName().c_str(),
                        this->name.c_str());
                break;
        }
    }
    else {
        fprintf(to, "%s", this->name.c_str());
    }
}

void
CVariable::Write(FILE* to)
{
    fprintf(to, "%s", name.c_str());
}
//===========================================
CMethodVariable::CMethodVariable(CVariable *v, enum MethodVarType t)
    :var(v),
     mvar(NULL),
     mv_type(t)
{
}
CMethodVariable::CMethodVariable(CMethodCall *v, enum MethodVarType t)
    :var(NULL),
     mvar(v),
     mv_type(t)
{
}
CMethodVariable::~CMethodVariable()
{
}

void
CMethodVariable::GatherTypes(set<CType*>* types) const
{
    types->insert(this->var->type);
}

void
CMethodVariable::Write(FILE* to)
{
    switch(mv_type) {
        case MVAR_VALUE:
        case MVAR_REF:
            //fprintf(to, "%s", this->var->name.c_str());
            break;
        case MVAR_POINTER:
            //fprintf(to, "&%s", this->var->name.c_str());
            fprintf(to, "&");
            break;
        case MVAR_OBJECT_POINTER:
            //fprintf(to, "%s", this->var->name.c_str());
            break;
    }
    if(var)
        var->Write(to);
    else if(mvar)
        mvar->Write(to);
}

//===========================================
CEnum::CEnum()
{
}

CEnum::~CEnum()
{
}

void
CEnum::GatherTypes(set<CType*>* types) const
{
    types->insert(this->type);
}

void
CEnum::WriteToHeader(FILE* to)
{
    if(name.size()) {
        fprintf(to, "enum %s {\n", name.c_str());
    }
    else {
        fprintf(to, "enum {\n");
    }
    size_t N = elements.size();
    for (size_t i=0; i<N; i++) {
        elements[i]->Write(to);
        fprintf(to, ",\n");
    }
    fprintf(to, "};\n");
}

void
CEnum::WriteToSource(FILE* to)
{
    WriteToHeader(to);
}

//========================================

CEnumElement::CEnumElement()
{
}
CEnumElement::CEnumElement(string n, string v)
        : name(n), value(v)
{
}

CEnumElement::~CEnumElement()
{
}

void
CEnumElement::GatherTypes(set<CType*>* types) const
{
}

void
CEnumElement::Write(FILE* to)
{
    fprintf(to, "%s = (%s)", name.c_str(), value.c_str());
}

//========================================

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
        fprintf(to, ".%s", name.c_str());
    }
    else if (this->clazz != NULL) {
        fprintf(to, "%s", this->clazz->QualifiedName().c_str());
        fprintf(to, "::%s", name.c_str());
    }
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
    if(comment.size()!=0) {
        fprintf(to, "%s", comment.c_str());
    }
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
        CMethodVariable *p = dynamic_cast<CMethodVariable *>(this->obj);
        if(0 != p && 0 != (CMethodVariable::MVAR_POINTER & p->mv_type)) {
            fprintf(to, "->");
        }
        else {
            fprintf(to, ".");
        }
    }
    else if (this->clazz != NULL) {
        fprintf(to, "%s.", this->clazz->QualifiedName().c_str());
    }
    fprintf(to, "%s(", this->name.c_str());
    CWriteArgumentList(to, this->arguments);
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
    CWriteArgumentList(to, this->arguments);
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

CFunction::CFunction()
    : returnType(NULL), // (NULL means constructor)
     returnTypeDimension(0),
     statements(NULL)
{
}

CFunction::~CFunction()
{
}

void
CFunction::GatherTypes(set<CType*>* types) const
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
CFunction::Write(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }


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

CMethod::CMethod()
    :CClassElement(0),
     returnType(NULL), // (NULL means constructor)
     statements(NULL),
     parent(0)
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
CMethod::WriteDeclaration(FILE* to, bool definition)
{
    size_t N, i;

    if(!definition) {
        CWriteModifiers(to, this->modifiers, CSTATIC | CVIRTUAL);
    }

    if (this->returnType != NULL) {
            fprintf(to, "%s ", this->returnType->QualifiedName().c_str());
    }
   
    if(definition) {
        string prefix;
        CClass *p = parent;
        while(p) {
            prefix = p->type->Name() + "::" + prefix;
            p = p->parent;
        }
        fprintf(to, "%s%s(", prefix.c_str(), this->name.c_str());
    } else {
        fprintf(to, "%s(", this->name.c_str());
    }

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

}

void
CMethod::WriteToHeader(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }

    WriteDeclaration(to, false);

    if(CPUREVIRTUAL == (this->modifiers & CPUREVIRTUAL))
        fprintf(to, " = 0");

    fprintf(to, ";\n");

}

void
CMethod::WriteToSource(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }
    WriteDeclaration(to, true);

    N = initial_list.size();
    if(N > 0) {
        fprintf(to, "\n:");
        for(i = 0; i < N-1; ++i) {
            initial_list[i]->first->Write(to);
            fprintf(to, "(");
            initial_list[i]->second->Write(to);
            fprintf(to, "),\n");
        }
        initial_list[i]->first->Write(to);
            fprintf(to, "(");
            initial_list[i]->second->Write(to);
            fprintf(to, ")\n");
    }


    if (this->statements == NULL) {
        CVariable* impl = new CVariable(CSP_TEMPLATE_IBINDER_TYPE, "impl", CVariable::VAR_REF);
        fprintf(to, ";\n");
    } else {
        fprintf(to, "\n");
        this->statements->Write(to);
    }
}


CClass::CClass()
    :CClassElement(0),
     type(NULL),
     parent(NULL)
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
CClass::WriteClassBegin(FILE* to)
{
    size_t N, i;


    fprintf(to, "class ");

    string name = this->type->Name();
    size_t pos = name.rfind(':');
    if (pos != string::npos) {
        name = name.c_str() + pos + 2;
    }

    fprintf(to, "%s", name.c_str());


    N = this->inherit.size();

    if (N != 0) {
        fprintf(to, " : ");

        for (i=0; i < N -1; i++) {
            fprintf(to, " public %s, ", this->inherit[i]->QualifiedName().c_str());
        }
        for (; i < N; i++) {
            fprintf(to, " public %s", this->inherit[i]->QualifiedName().c_str());
        }
    }

    fprintf(to, "\n");
    fprintf(to, "{\n");
}

void
CClass::WriteClassEnd(FILE* to)
{
    fprintf(to, "};\n");
}

void
CClass::WriteToHeader(FILE* to)
{
    size_t N, i;


    WriteClassBegin(to);
    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }

    N = this->elements.size();
    fprintf(to, "public:\n");
    for (i=0; i<N; i++) {
        if(CPUBLIC == (this->elements[i]->modifiers & CSCOPE_MASK) &&
                CSTICK_TO_SOURCE != (this->elements[i]->modifiers & CSTICK_MASK))
            this->elements[i]->WriteToHeader(to);
    }
    fprintf(to, "private:\n");
    for (i=0; i<N; i++) {
        if(CPRIVATE == (this->elements[i]->modifiers & CSCOPE_MASK) &&
                CSTICK_TO_SOURCE != (this->elements[i]->modifiers & CSTICK_MASK))
            this->elements[i]->WriteToHeader(to);
    }

    WriteClassEnd(to);

}

void
CClass::WriteToSource(FILE* to)
{
    size_t N, i;

    if(CPRIVATE == (modifiers & CSCOPE_MASK)) {
        WriteToHeader(to);
    }

    N = this->elements.size();
    for (i=0; i<N; i++) {
        if(CSTICK_TO_HEADER != (this->elements[i]->modifiers & CSTICK_MASK)) {
            this->elements[i]->WriteToSource(to);
        }
    }


}
// ==========================================
//
void
CNamespace::WriteToHeader(FILE* to)
{
    size_t N, i;
    N = this->enums.size();
    for (i=0; i<N; i++) {
        CEnum* c = this->enums[i];
        if(CPUBLIC == (c->modifiers & CSCOPE_MASK))
            c->WriteToHeader(to);
    }

    N = this->classes.size();
    for (i=0; i<N; i++) {
        CClass* c = this->classes[i];
        if(CPUBLIC == (c->modifiers & CSCOPE_MASK))
            c->WriteToHeader(to);
    }
}

void
CNamespace::WriteToSource(FILE* to)
{
    size_t N, i;
    N = this->enums.size();
    //fprintf(stderr, "%d enum in namespace\n",N);
    for (i=0; i<N; i++) {
        CEnum* c = this->enums[i];
        if(CPRIVATE == (c->modifiers & CSCOPE_MASK))
            c->WriteToSource(to);
    }

    N = this->classes.size();
    //fprintf(stderr, "%d classes in namespace\n",N);
    for (i=0; i<N; i++) {
        CClass* c = this->classes[i];
        c->WriteToSource(to);
    }
}
// =========================================

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

string namespace2hierarchyBegin(string ns)
{
    int i;
    string prefix("namespace ");
    while((i = ns.find(':')) != ns.npos) {
        ns.replace(i,2," {\nnamespace ");
    }
    return prefix + ns + "{\n";
}

string namespace2hierarchyEnd(string ns)
{
    int i;
    string res("};\n");
    while((i = ns.find(':')) != ns.npos) {
        ns.replace(i,2,"");
        res += "};\n";
    }
    return res;
}


void
CDocument::WriteToHeader(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }
    fprintf(to, "/*\n"
                " * This file is auto-generated C++ header file.  DO NOT MODIFY.\n"
                " * Original file: %s\n"
                " */\n", escape_backslashes(this->originalSrc).c_str());
    set<string>::iterator sit = includes.begin();
    for (; sit != includes.end(); ++sit) {
        fprintf(to, "#include <%s>\n",(*sit).c_str());
    }
    if (this->_namespace.length() != 0) {
        fprintf(to, "%s\n", namespace2hierarchyBegin(this->_namespace.c_str()).c_str());
    }

    N = this->classes.size();
    for (i=0; i<N; i++) {
        CClass* c = this->classes[i];
        if(CPUBLIC == (c->modifiers & CSCOPE_MASK))
            c->WriteToHeader(to);
    }
    N = this->nss.size();
    for (i=0; i<N; i++) {
        nss[i]->WriteToHeader(to);
    }
    if (this->_namespace.length() != 0) {
        fprintf(to, "%s\n", namespace2hierarchyEnd(this->_namespace.c_str()).c_str());
    }
}

void
CDocument::WriteToSource(FILE* to)
{
    size_t N, i;

    if (this->comment.length() != 0) {
        fprintf(to, "%s\n", this->comment.c_str());
    }

    char* pos = rfind(const_cast<char *>(header_file.c_str()),'/');

    if(pos != NULL) {
        fprintf(to, "#include <%s>\n", pos+1);
    }
    fprintf(to, "/*\n"
                " * This file is auto-generated C++ source file.  DO NOT MODIFY.\n"
                " * Original file: %s\n"
                " */\n", escape_backslashes(this->originalSrc).c_str());
    if (this->_namespace.length() != 0) {
        fprintf(to, "%s\n", namespace2hierarchyBegin(this->_namespace.c_str()).c_str());
    }
    N = this->classes.size();
    //fprintf(stderr, "%d classes in total\n",N);
    for (i=0; i<N; i++) {
        CClass* c = this->classes[i];
        c->WriteToSource(to);
    }
    N = this->nss.size();
    //fprintf(stderr,"%d namespaces in total\n",N);
    for (i=0; i<N; i++) {
        nss[i]->WriteToSource(to);
    }
    if (this->_namespace.length() != 0) {
        fprintf(to, "%s\n", namespace2hierarchyEnd(this->_namespace.c_str()).c_str());
    }
}
