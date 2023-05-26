//
// Created by wy on 22.5.23.
//

#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include <vector>
#include <stack>

#include "lox/expr.h"
#include "lox/statement.h"
#include "lox/environment.h"

namespace lox {

class LoopContext {
public:
    bool break_loop {false};
};

class Interpreter : public ExprVisitor , public StmtVisitor {
public:
    Interpreter();

    void interpret(const std::vector<Statement::ptr> &statements);

    void enable_repl_mode() {
        repl_mode_ = true;
    }

    Value execute(Statement *statement);
    void execute_block(const std::vector<Statement::ptr> &statements, Environment::ptr env);

    // expr
    Value visitLiteralExpr(Literal *expr) override {
        return expr->value;
    }
    Value visitGroupingExpr(Grouping *expr) override {
        return evaluate(expr->expression.get());
    }
    Value visitUnaryExpr(Unary *expr) override;

    Value visitBinaryExpr(Binary *expr) override;
    Value visitVariableExpr(Variable *expr) override;
    Value visitAssignExpr(lox::Assign *expr) override;
    Value visitBreakExpr(Break *expr) override;


    // statements
    Value visitExpressionStmt(lox::Expression *stmt) override;
    Value visitPrintStmt(lox::Print *stmt) override;
    Value visitVarStmt(lox::Var *stmt) override;
    Value visitBlockStmt(lox::Block *stmt) override;
    Value visitIfStmt(lox::IfStmt *stmt) override;
    Value visitLogicalExpr(lox::Logical *expr) override;
    Value visitWhileStmt(lox::WhileStmt *stmt) override;
    Value visitForStmt(lox::ForStmt *stmt) override;

private:
    Value evaluate(Expr *expr);

    Environment::ptr environment_;
    std::stack<LoopContext> loop_ctx_;
    bool repl_mode_{false};
};

}

#endif //LOX_INTERPRETER_H
