//
// Created by wy on 22.5.23.
//

#ifndef LOX_STATEMENT_H
#define LOX_STATEMENT_H

#include <memory>
#include <utility>

#include "lox/value.h"
#include "lox/expr.h"

namespace lox {

/*
 * program  ->  declaration* EOF
 *
 * declaration  ->  var_declaration
 *                  | statement;
 *
 * statement -> expresion_stmt
 *              | ifStmt
 *              | forStmt
 *              | whileStmt
 *              | print_stmt;
 *              | block
 *
 * var_declaration -> "var" IDENTIFIER ("=" expression)? ";" ;
 *
 * block      -> "{" declaration* "}" ;
 */

class Expression;
class Print;
class Var;
class Block;
class IfStmt;
class WhileStmt;
class ForStmt;

class StmtVisitor {
public:
    virtual Value visitExpressionStmt(Expression* stmt) = 0;

    virtual Value visitPrintStmt(Print* stmt) = 0;
//    virtual Value visitInput(std::shared_ptr<Input> stmt) = 0;
    virtual Value visitBlockStmt(Block *stmt) = 0;
    virtual Value visitVarStmt(Var* stmt) = 0;
    virtual Value visitIfStmt(IfStmt *stmt) = 0;
    virtual Value visitWhileStmt(WhileStmt *stmt) = 0;
    virtual Value visitForStmt(ForStmt *stmt) = 0;

//    virtual Value visitFunction(std::shared_ptr<Function> stmt) = 0;
//
//    virtual Value visitReturn(std::shared_ptr<Return> stmt) = 0;
//
//    virtual Value visitClass(std::shared_ptr<Class> stmt) = 0;
};


class Statement {
public:
    using ptr = std::shared_ptr<Statement>;

    virtual Value accept(StmtVisitor *visitor) = 0;
};


class Expression : public Statement {
public:
    explicit Expression(Expr::ptr expression): expression(std::move(expression)) {}
    Value accept(StmtVisitor *visitor) override {
        return visitor->visitExpressionStmt(this);
    }

    Expr::ptr expression;
};


class Print : public Statement {
public:
    explicit Print(Expr::ptr expression): expression(std::move(expression)) {}

    Value accept(StmtVisitor *visitor) override {
        return visitor->visitPrintStmt(this);
    }

    Expr::ptr expression;
};


class Var : public Statement {
public:
    Var(Token::ptr name, Expr::ptr value): name(std::move(name)), value(std::move(value)) {}

    Value accept(lox::StmtVisitor *visitor) override {
        return visitor->visitVarStmt(this);
    }

    Token::ptr name;
    Expr::ptr value;

};

class Block : public Statement {
public:
    explicit Block(std::vector<Statement::ptr> statements): statements(std::move(statements)) {}

    Value accept(lox::StmtVisitor *visitor) override {
        return visitor->visitBlockStmt(this);
    }

    std::vector<Statement::ptr> statements;
};

class IfStmt : public Statement {
public:
    IfStmt(Expr::ptr condition, Statement::ptr then_branch, Statement::ptr else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    Value accept(StmtVisitor *visitor) override {
        return visitor->visitIfStmt(this);
    }

    Expr::ptr condition;
    Statement::ptr then_branch;
    Statement::ptr else_branch;
};

class WhileStmt : public Statement {
public:
    WhileStmt(Expr::ptr condition, Statement::ptr body)
        : condition(std::move(condition)), body(std::move(body)) {}

    Value accept(StmtVisitor *visitor) override {
        return visitor->visitWhileStmt(this);
    }

    Expr::ptr condition;
    Statement::ptr body;
};

class ForStmt : public Statement {
public:
    ForStmt(Statement::ptr initializer, Expr::ptr condition, Statement::ptr increment, Statement::ptr body)
        : initializer(std::move(initializer)), condition(std::move(condition)),
          increment(std::move(increment)), body(std::move(body)) {}

    Value accept(StmtVisitor *visitor) override {
        return visitor->visitForStmt(this);
    }

    Statement::ptr initializer;
    Expr::ptr condition;
    Statement::ptr increment;
    Statement::ptr body;
};


} // namespace lox



#endif //LOX_STATEMENT_H
