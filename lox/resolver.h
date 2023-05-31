//
// Created by wy on 29.5.23.
//

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "lox/statement.h"
#include "lox/expr.h"
#include "lox/exception.h"

class Resolver : public stmt::Visitor, public expr::Visitor {
 public:
    Resolver();

    void resolve(const std::vector<stmt::Statement::ptr> &statements);
    void resolve(const stmt::Statement::ptr &stmt);
    void resolve(const expr::Expr::ptr &expr);
    void resolve_function(stmt::Function *stmt);

    Value visit_block_stmt(stmt::Block* stmt) override;
    Value visit_var_stmt(stmt::Var* stmt) override;
    Value visit_function_stmt(stmt::Function* stmt) override;
    Value visit_expression_stmt(stmt::Expression* stmt) override;
    Value visit_for_stmt(stmt::For* stmt) override;
    Value visit_while_stmt(stmt::While* stmt) override;
    Value visit_print_stmt(stmt::Print* stmt) override;
    Value visit_if_stmt(stmt::If* stmt) override;
    Value visit_return_stmt(stmt::Return* stmt) override;
    Value visit_class_stmt(stmt::Class *stmt) override;
    Value visit_super_expr(expr::Super *expr) override;

    Value visit_get_expr(expr::Get *expr) override;
    Value visit_set_expr(expr::Set *expr) override;
    Value visit_this_expr(expr::This *expr) override;
    Value visit_variable_expr(expr::Variable *expr) override;
    Value visit_assign_expr(expr::Assign *expr) override;
    Value visit_literal_expr(expr::Literal *expr) override {
        return nullptr;
    }
    Value visit_grouping_expr(expr::Grouping *expr) override {
        resolve(expr->expression);
        return nullptr;
    }
    Value visit_unary_expr(expr::Unary *expr) override {
        resolve(expr->right);
        return nullptr;
    }
    Value visit_binary_expr(expr::Binary *expr) override {
        resolve(expr->left);
        resolve(expr->right);
        return nullptr;
    }
    Value visit_break_expr(expr::Break *expr) override {
        if (scopes_.size() == 1) {
            throw RuntimeError(expr->keyword, "Can't break from top-level code.");
        }
        return nullptr;
    }
    Value visit_logical_expr(expr::Logical *expr) override {
        resolve(expr->left);
        resolve(expr->right);
        return nullptr;
    }
    Value visit_call_expr(expr::Call *expr) override {
        resolve(expr->callee);
        for (const auto &item : expr->arguments) {
            resolve(item);
        }
        return nullptr;
    }

 private:
    void begin_scope();
    void end_scope();

    void declare(const Token::ptr &name);
    void define(const Token::ptr &name);

    std::vector<std::unordered_map<std::string, bool>> scopes_;
    bool in_class_ {false};
    bool class_has_super_ {false};
};
