//
// Created by wy on 29.5.23.
//

#include "lox/resolver.h"

Resolver::Resolver() {
    scopes_.emplace_back();
}

void Resolver::resolve(const std::vector<stmt::Statement::ptr> &statements) {
    for (const auto &stmt : statements) {
        resolve(stmt);
    }
}

void Resolver::resolve(const stmt::Statement::ptr &stmt) {
    stmt->accept(this);
}

void Resolver::resolve(const expr::Expr::ptr &stmt) {
    stmt->accept(this);
}

void Resolver::begin_scope() {
    scopes_.emplace_back();
}

void Resolver::end_scope() {
    scopes_.pop_back();
}

void Resolver::declare(const Token::ptr &name) {
    if (scopes_.empty()) {
        return;
    }
    if (scopes_.back().count(name->lexeme)) {
        throw RuntimeError(name, "Already a variable with this name in this scope: " + name->lexeme);
    }

    scopes_.back()[name->lexeme] = false;
}

void Resolver::define(const Token::ptr &name) {
    if (scopes_.empty()) {
        return;
    }
    scopes_.back()[name->lexeme] = true;
}

Value Resolver::visit_block_stmt(stmt::Block *stmt) {
    begin_scope();
    resolve(stmt->statements);
    end_scope();
    return nullptr;
}

Value Resolver::visit_var_stmt(stmt::Var *stmt) {
    declare(stmt->name);
    if (stmt->value) {
        resolve(stmt->value);
    }
    define(stmt->name);
    return nullptr;
}

Value Resolver::visit_function_stmt(stmt::Function *stmt) {
    declare(stmt->name);
    define(stmt->name); // lets a function recursively refer to itself inside its own body


    begin_scope();
    for (auto &param : stmt->params) {
        declare(param);
        define(param);
    }
    resolve(stmt->body);
    end_scope();

    return nullptr;
}

Value Resolver::visit_expression_stmt(stmt::Expression *stmt) {
    resolve(stmt->expression);
    return nullptr;
}

Value Resolver::visit_for_stmt(stmt::For *stmt) {
    resolve(stmt->initializer);
    resolve(stmt->condition);
    resolve(stmt->increment);
    resolve(stmt->body);
    return nullptr;
}

Value Resolver::visit_while_stmt(stmt::While *stmt) {
    resolve(stmt->condition);
    resolve(stmt->body);
    return nullptr;
}

Value Resolver::visit_print_stmt(stmt::Print *stmt) {
    resolve(stmt->expression);
    return nullptr;
}

Value Resolver::visit_if_stmt(stmt::If *stmt) {
    resolve(stmt->condition);
    resolve(stmt->then_branch);
    if (stmt->else_branch) {
        resolve(stmt->else_branch);
    }
    return nullptr;
}

Value Resolver::visit_return_stmt(stmt::Return *stmt) {
    if (stmt->value) {
        resolve(stmt->value);
    }
    if (scopes_.size() == 1) {
        throw RuntimeError(stmt->keyword, "Can't return from top-level code.");
    }
    return nullptr;
}

Value Resolver::visit_variable_expr(expr::Variable *expr) {
    if (!scopes_.empty() && scopes_.back().count(expr->name->lexeme) && !scopes_.back()[expr->name->lexeme]) {
        throw RuntimeError(expr->name, "Can't read local variable in its own initializer.");
    }
    return nullptr;
}

Value Resolver::visit_assign_expr(expr::Assign *expr) {
    resolve(expr->value);
    return nullptr;
}
