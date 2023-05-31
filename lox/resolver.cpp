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

void Resolver::resolve_function(stmt::Function *stmt) {
    begin_scope();
    for (auto &param : stmt->params) {
        declare(param);
        define(param);
    }
    resolve(stmt->body);
    end_scope();
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

    resolve_function(stmt);

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

Value Resolver::visit_class_stmt(stmt::Class *stmt) {
    declare(stmt->name);
    define(stmt->name);

    if (stmt->super && stmt->super->name->lexeme == stmt->name->lexeme) {
        throw RuntimeError(stmt->name, "A class can't inherit from itself.");
    }

    class_has_super_ = false;
    if (stmt->super) {
        class_has_super_ = true;
        resolve(stmt->super);
        begin_scope();
        scopes_.back()["super"] = true;
    }

    bool old_in_class = in_class_;
    in_class_ = true;
    begin_scope();
    scopes_.back()["this"] = true;
    for (const auto &item : stmt->methods) {
        resolve_function(item.get());
    }
    end_scope();
    if (stmt->super) {
        end_scope();
    }
    class_has_super_ = false;
    in_class_ = old_in_class;
    return nullptr;
}

Value Resolver::visit_super_expr(expr::Super *expr) {
    if (!in_class_) {
        throw RuntimeError(expr->keyword, "Can't use 'super' outside of a class.");
    }
    if (!class_has_super_) {
        throw RuntimeError(expr->keyword, "Can't use 'super' in a class which has no super class.");
    }
    return nullptr;
}

Value Resolver::visit_get_expr(expr::Get *expr) {
    resolve(expr->object);
    return nullptr;
}

Value Resolver::visit_set_expr(expr::Set *expr) {
    resolve(expr->value);
    resolve(expr->object);
    return nullptr;
}

Value Resolver::visit_this_expr(expr::This *expr) {
    if (!in_class_) {
        throw RuntimeError(expr->name, "Can't use 'this' outside of a class.");
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
