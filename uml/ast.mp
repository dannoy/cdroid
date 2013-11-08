input metauml;
prologues:=3;
filenametemplate "%j-%c.eps"; 

beginfig(1);
Class.Expression("Expression")
        ()
        (
        "+{virtual}~Expression():",
        "+{virtual}Write(File* to):void");

Class.LiteralExpression("LiteralExpression")
        ()
        ();
Class.StringLiteralExpression("StringLiteralExpression")
        (
        "+value : string"
        )
        ();
Class.Variable("Variable")
        (
        "+type : Type *",
        "+name : string",
        "+dimension : int"
        )
        ();
Class.FieldVariable("FieldVariable")
        (
        "+clazz : Type *",
        "+name : string",
        "+object : Expression *"
        )
        ();

Class.Assignment("Assignment")
        (
        "+lvalue : Variable *",
        "+rvalue : Expression",
        "+cast : Type *"
        )
        ();
Class.MethodCall("MethodCall")
        (
        "+obj : Expression *",
        "+clazz : Type *",
        "+name : string",
        "+argument : vector<Expression*>",
        "+exception : vector<string>"
        )
        ();

Class.Comparison("Comparison")
        (
        "+lvalue : Expression *",
        "+rvalue : Expression *",
        "+op : string"
        )
        ();
Class.NewExpression("NewExpression")
        (
        "+type : Type *",
        "+arguments : vector<Expression *>"
        )
        ();
Class.NewArrayExpression("NewArrayExpression")
        (
        "+type : Type *",
        "+size : Expression *"
        )
        ();
Class.Ternary("Ternary")
        (
        "+condition : Expression *",
        "+ifpart : Expression *",
        "+elsepart : Expression *"
        )
        ();
Class.Cast("Cast")
        (
        "+type : Type *",
        "+expression : Expression *"
        )
        ();

topToBottom(50)(Expression, Variable);
topToBottom(50)(NewExpression,Expression);
leftToRight.top(20)(LiteralExpression, StringLiteralExpression, Variable, FieldVariable, Assignment);
leftToRight.bottom(20)(MethodCall,Comparison,NewExpression, NewArrayExpression, Ternary, Cast);

drawObjects(Expression, LiteralExpression, StringLiteralExpression, Variable, FieldVariable, Assignment,
        MethodCall,Comparison,NewExpression, NewArrayExpression, Ternary, Cast
        );

link(inheritance)(pathStepY(LiteralExpression.n, Expression.s, 20));
link(inheritance)(pathStepY(StringLiteralExpression.n, Expression.s, 20));
link(inheritance)(Variable.n -- Expression.s);
link(inheritance)(pathStepY(FieldVariable.n, Expression.s, 20));
link(inheritance)(pathStepY(Assignment.n, Expression.s, 20));

link(inheritance)(pathStepY(MethodCall.s, Expression.n,-20));
link(inheritance)(pathStepY(Comparison.s, Expression.n,-20));
link(inheritance)(NewExpression.s -- Expression.n);
link(inheritance)(pathStepY(NewArrayExpression.s, Expression.n,-20));
link(inheritance)(pathStepY(Ternary.s, Expression.n,-20));
link(inheritance)(pathStepY(Cast.s, Expression.n,-20));


%endfig;

%beginfig(2);
Class.ClassElement("ClassElement")
        ()
        ("+ClassElement():",
        "+{virtual}~ClassElement():",
        "+{virtual}GatherTypes(set<Type*> *types):void",
        "+{virtual}Write(File* to):void");
ClassElement.n = below(Variable.s,50);
Class.Field("Field")
        (
        "+comment : string",
        "+modifiers : int",
        "+variable : Variable",
        "+value : string",
        )
        (
        );
Class.Method("Method")
        (
        "+comment : string",
        "+modifiers : int",
        "+returnType : Type *",
        "+returnTypeDimension : size_t",
        "+name : string",
        "+parameters : vector<Variable *>",
        "+exceptions : vector<Type *>",
        "+statements : StatementBlock *",
        )
        (
        );
Class.Clazz("Class")
        (
        "+comment : string",
        "+modifiers : int",
        "+what : int *",
        "+type : Type *",
        "+extends : Type *",
        "+interfaces : vector<Type *>",
        "+elements : vector<ClassElement *>"
        )
        (
        );

topToBottom(50)(ClassElement, Method);
leftToRight.top(20)(Field, Method, Clazz);
drawObjects(ClassElement, Field, Method, Clazz);
link(inheritance)(pathStepY(Field.n, ClassElement.s, 20));
link(inheritance)(Method.n -- ClassElement.s);
link(inheritance)(pathStepY(Clazz.n, ClassElement.s,20));
%endfig;

%beginfig(3);
Class.Statement("Statement")
        ()
        (
        "+{virtual}~Statement():",
        "+{virtual}Write(File* to):void");
Class.StatementBlock("StatementBlock")
        (
        "+statements : vector<Statement*>",
        )
        (
        );
Class.ExpressionStatement("ExpressionStatement")
        (
        "+expression : Expression *"
        )
        (
        );

Class.VariableDeclaration("VariableDeclaration")
        (
        "+cast : Type *",
        "+lvalue : Variable *",
        "+rvalue : Expression *"
        )
        ();
Class.IfStatement("IfStatement")
        (
        "+expression : Expression *",
        "+statements : StatementBlock *",
        "+elseif : IfStatement *"
        )
        ();
Class.ReturnStatement("ReturnStatement")
        (
        "+expression : Expression *"
        )
        ();
Class.TryStatement("TryStatement")
        (
        "+expression : Expression *"
        )
        ();
Class.CatchStatement("CatchStatement")
        (
        "+statements : StatementBlock *",
        "+exception : Variable *"
        )
        ();
Class.FinallyStatement("FinallyStatement")
        (
        "+statements : StatementBlock *"
        )
        ();
Class.SwitchStatement("SwitchStatement")
        (
        "+expression : Expression *",
        "+cases : vector<Case *> *"
        )
        ();
Class.Break("Break")
        (
        )
        ();
FinallyStatement.n = below(Method.s,50);

topToBottom(50)(Statement, VariableDeclaration);
topToBottom(50)(FinallyStatement, Statement);
leftToRight.bottom(20)(TryStatement, CatchStatement, FinallyStatement, Break,SwitchStatement);
leftToRight.top(20)(StatementBlock, ExpressionStatement ,VariableDeclaration, IfStatement, ReturnStatement);
drawObjects(Statement, StatementBlock, ExpressionStatement ,VariableDeclaration,
            IfStatement, ReturnStatement,
            TryStatement, CatchStatement, FinallyStatement, SwitchStatement, Break);

Class.Case("Case")
        (
        "+cases : vector<string>",
        "+statements : StatementBlock *"
        )
        ();

Case.w = atright(SwitchStatement.e, 50);

drawObjects(Case);



link(inheritance)(pathStepY(StatementBlock.n, Statement.s, 20));
link(inheritance)(pathStepY(ExpressionStatement.n, Statement.s, 20));
link(inheritance)(VariableDeclaration.n -- Statement.s);
link(inheritance)(pathStepY(IfStatement.n, Statement.s, 20));
link(inheritance)(pathStepY(ReturnStatement.n, Statement.s, 20));

link(inheritance)(pathStepY(TryStatement.s, Statement.n,-20));
link(inheritance)(pathStepY(CatchStatement.s, Statement.n,-20));
link(inheritance)(FinallyStatement.s -- Statement.n);
link(inheritance)(pathStepY(SwitchStatement.s, Statement.n,-20));
link(inheritance)(pathStepY(Break.s, Statement.n,-20));

link(associationUni)( SwitchStatement.e -- Case.w);
%link(associationUni)( ExpressionStatement.s .. Expression.w);

endfig;

end;
