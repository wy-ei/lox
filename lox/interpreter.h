//
// Created by wy on 22.5.23.
//

#pragma once

#include <stack>
#include <unordered_map>
#include <vector>

#include "lox/environment.h"
#include "lox/expr.h"
#include "lox/statement.h"

class Interpreter : public expr::Visitor, public stmt::Visitor {
 public:
    Interpreter();

    void interpret(const std::vector<stmt::Statement::ptr> &statements);

    void enable_repl_mode() {
        repl_mode_ = true;
    }

    Environment::ptr globals() {
        return globals_environment_;
    }

    Value execute(stmt::Statement *statement);

    void execute_block(const std::vector<stmt::Statement::ptr> &statements, Environment::ptr env);

    // expr
    Value visit_literal_expr(expr::Literal *expr) override;
    Value visit_grouping_expr(expr::Grouping *expr) override;
    Value visit_unary_expr(expr::Unary *expr) override;
    Value visit_binary_expr(expr::Binary *expr) override;
    Value visit_variable_expr(expr::Variable *expr) override;
    Value visit_logical_expr(expr::Logical *expr) override;
    Value visit_assign_expr(expr::Assign *expr) override;
    Value visit_break_expr(expr::Break *expr) override;
    Value visit_call_expr(expr::Call *expr) override;
    Value visit_get_expr(expr::Get *expr) override;
    Value visit_set_expr(expr::Set *expr) override;
    Value visit_this_expr(expr::This *expr) override;
    Value visit_super_expr(expr::Super *expr) override;

    // statements
    Value visit_expression_stmt(stmt::Expression *stmt) override;
    Value visit_print_stmt(stmt::Print *stmt) override;
    Value visit_var_stmt(stmt::Var *stmt) override;
    Value visit_block_stmt(stmt::Block *stmt) override;
    Value visit_if_stmt(stmt::If *stmt) override;
    Value visit_while_stmt(stmt::While *stmt) override;
    Value visit_for_stmt(stmt::For *stmt) override;
    Value visit_function_stmt(stmt::Function *stmt) override;
    Value visit_return_stmt(stmt::Return *stmt) override;
    Value visit_class_stmt(stmt::Class *stmt) override;

 private:
    Value evaluate(expr::Expr *expr);

    Environment::ptr globals_environment_;
    Environment::ptr environment_;
    bool repl_mode_{false};
};
