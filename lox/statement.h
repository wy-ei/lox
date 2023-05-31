//
// Created by wy on 22.5.23.
//

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "lox/expr.h"
#include "lox/value.h"

/*
 * program  ->  declaration* EOF
 *
 * declaration  ->  var_declaration
 *                  | funDecl
 *                  | statement;
 *
 * statement -> expresion_stmt
 *              | classDecl
 *              | returnStmt
 *              | ifStmt
 *              | forStmt
 *              | whileStmt
 *              | print_stmt;
 *              | block
 *
 * returnStmt -> "return" expression ? ";"
 * var_declaration -> "var" IDENTIFIER ("=" expression)? ";" ;
 *
 * classDecl  -> "class" IDENTIFIER ("<" IDENTIFIER)? "{" function* "}" ;
 *
 * block      -> "{" declaration* "}" ;
 *
 * funDec   ->  "fun" function;
 * function  -> IDENTIFIER "(" parameters? ")" block ;
 * parameters  -> IDENTIFIER ("," IDENTIFIER)* ;
 */

namespace stmt {


class Expression;
class Print;
class Var;
class Block;
class If;
class While;
class For;
class Function;
class Return;
class Class;

class Visitor {
 public:
    virtual Value visit_expression_stmt(Expression* stmt) = 0;

    virtual Value visit_print_stmt(Print* stmt) = 0;
    virtual Value visit_block_stmt(stmt::Block* stmt) = 0;
    virtual Value visit_var_stmt(Var* stmt) = 0;
    virtual Value visit_if_stmt(If* stmt) = 0;
    virtual Value visit_while_stmt(While* stmt) = 0;
    virtual Value visit_for_stmt(For* stmt) = 0;
    virtual Value visit_function_stmt(Function* stmt) = 0;
    virtual Value visit_return_stmt(Return* stmt) = 0;
    virtual Value visit_class_stmt(Class *stmt) = 0;
};

class Statement {
 public:
    using ptr = std::shared_ptr<Statement>;

    virtual Value accept(Visitor *visitor) = 0;
};

class Expression : public Statement {
 public:
    using ptr = std::shared_ptr<Expression>;

    explicit Expression(expr::Expr::ptr expression) : expression(std::move(expression)) {}
    Value accept(Visitor *visitor) override {
        return visitor->visit_expression_stmt(this);
    }

    expr::Expr::ptr expression;
};

class Print : public Statement {
 public:
    using ptr = std::shared_ptr<Print>;
    explicit Print(expr::Expr::ptr expression) : expression(std::move(expression)) {}

    Value accept(Visitor *visitor) override {
        return visitor->visit_print_stmt(this);
    }

    expr::Expr::ptr expression;
};

class Var : public Statement {
 public:
    using ptr = std::shared_ptr<Var>;
    Var(Token::ptr name, expr::Expr::ptr value) : name(std::move(name)), value(std::move(value)) {}

    Value accept(Visitor *visitor) override {
        return visitor->visit_var_stmt(this);
    }

    Token::ptr name;
    expr::Expr::ptr value;
};

class Block : public Statement {
 public:
    using ptr = std::shared_ptr<Block>;

    explicit Block(std::vector<Statement::ptr> statements) : statements(std::move(statements)) {}

    Value accept(Visitor *visitor) override {
        return visitor->visit_block_stmt(this);
    }

    std::vector<Statement::ptr> statements;
};

class If : public Statement {
 public:
    using ptr = std::shared_ptr<If>;

    If(expr::Expr::ptr condition, Statement::ptr then_branch, Statement::ptr else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    Value accept(Visitor *visitor) override {
        return visitor->visit_if_stmt(this);
    }

    expr::Expr::ptr condition;
    Statement::ptr then_branch;
    Statement::ptr else_branch;
};

class While : public Statement {
 public:
    using ptr = std::shared_ptr<While>;

    While(expr::Expr::ptr condition, Statement::ptr body) : condition(std::move(condition)), body(std::move(body)) {}

    Value accept(Visitor *visitor) override {
        return visitor->visit_while_stmt(this);
    }

    expr::Expr::ptr condition;
    Statement::ptr body;
};

class For : public Statement {
 public:
    using ptr = std::shared_ptr<For>;

    For(Statement::ptr initializer, expr::Expr::ptr condition, Statement::ptr increment, Statement::ptr body)
        : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)),
          body(std::move(body)) {}

    Value accept(Visitor *visitor) override {
        return visitor->visit_for_stmt(this);
    }

    Statement::ptr initializer;
    expr::Expr::ptr condition;
    Statement::ptr increment;
    Statement::ptr body;
};

class Function : public Statement, public std::enable_shared_from_this<Function> {
 public:
    using ptr = std::shared_ptr<Function>;

    Function(Token::ptr name, std::vector<Token::ptr> params, Statement::ptr body) {
        this->name = std::move(name);
        this->params = std::move(params);
        this->body = std::move(body);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_function_stmt(this);
    }

    Token::ptr name;
    std::vector<Token::ptr> params;
    Statement::ptr body;
};

class Return : public Statement {
 public:
    using ptr = std::shared_ptr<Return>;

    Return(Token::ptr keyword, expr::Expr::ptr value) {
        this->keyword = std::move(keyword);
        this->value = std::move(value);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_return_stmt(this);
    }

    Token::ptr keyword;
    expr::Expr::ptr value;
};

class Class : public Statement {
 public:
    using ptr = std::shared_ptr<Class>;

    Class(Token::ptr name, expr::Variable::ptr super,  std::vector<stmt::Function::ptr> methods) {
        this->name = std::move(name);
        this->methods = std::move(methods);
        this->super = std::move(super);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_class_stmt(this);
    }

    Token::ptr name;
    expr::Variable::ptr super;
    std::vector<stmt::Function::ptr> methods;
};

}  // namespace stmt
