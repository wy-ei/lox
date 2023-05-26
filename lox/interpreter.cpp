//
// Created by wy on 22.5.23.
//

#include "interpreter.h"

#include <iostream>
#include "lox/token.h"

namespace lox {

Interpreter::Interpreter() {
    environment_ = std::make_shared<Environment>();
}

Value Interpreter::visitUnaryExpr(Unary *expr) {
    Value value = evaluate(expr->right.get());
    switch (expr->op->kind) {
        case Token::Kind::MINUS:
            return -value.as<double>();
        case Token::Kind::BANG:
            return !value;
        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

Value Interpreter::visitBinaryExpr(Binary *expr) {
    Value left = evaluate(expr->left.get());
    Value right = evaluate(expr->right.get());

    switch (expr->op->kind) {
        case Token::Kind::PLUS:
            return left + right;
        case Token::Kind::MINUS:
            return left - right;
        case Token::Kind::STAR:
            return left * right;
        case Token::Kind::SLASH:
            return left / right;
        case Token::Kind::GREATER:
            return left > right;
        case Token::Kind::GREATER_EQUAL:
            return left >= right;
        case Token::Kind::LESS:
            return left < right;
        case Token::Kind::LESS_EQUAL:
            return left <= right;
        case Token::Kind::BANG_EQUAL:
            return left != right;
        case Token::Kind::EQUAL_EQUAL:
            return left == right;
        default:
            throw std::runtime_error("Unknown binary operator");
    }
}

lox::Value Interpreter::visitVariableExpr(Variable *expr) {
    return environment_->get(expr->name);
}

Value Interpreter::visitAssignExpr(lox::Assign *expr) {
    Value value = evaluate(expr->value.get());
    environment_->assign(expr->name, value);
    return value;
}

Value Interpreter::visitBreakExpr(lox::Break *expr) {
    if (loop_ctx_.empty()) {
        throw std::runtime_error("'break' must in loop body.");
    }
    loop_ctx_.top().break_loop = true;
    return nullptr;
}

Value Interpreter::visitExpressionStmt(Expression *stmt) {
    Value value = evaluate(stmt->expression.get());
    return value;
}

Value Interpreter::visitPrintStmt(Print *stmt) {
    Value value = evaluate(stmt->expression.get());
    std::cout << value.str() << std::endl;
    return nullptr;
}

Value Interpreter::visitVarStmt(Var *stmt) {
    Value value;
    if (stmt->value != nullptr) {
        value = evaluate(stmt->value.get());
    }
    environment_->define(stmt->name->lexeme, value);
    return nullptr;
}

Value Interpreter::visitBlockStmt(Block *stmt) {
    execute_block(stmt->statements, std::make_shared<Environment>(environment_));
    return nullptr;
}

Value Interpreter::visitIfStmt(IfStmt *stmt) {
    Value value = evaluate(stmt->condition.get());
    if (value) {
        execute(stmt->then_branch.get());
    } else {
        if (stmt->else_branch) {
            execute(stmt->else_branch.get());
        }
    }
    return nullptr;
}

Value Interpreter::visitLogicalExpr(lox::Logical *expr) {
    Value left = evaluate(expr->left.get());
    if (expr->op->kind == Token::OR) {
        if (left) {
            return left;
        }
    } else {
        if (!left) {
            return left;
        }
    }
    return evaluate(expr->right.get());
}

Value Interpreter::visitWhileStmt(lox::WhileStmt *stmt) {
    std::shared_ptr<void> defer(nullptr, [this, previous=this->environment_](void*){
        this->environment_ = previous;
        this->loop_ctx_.pop();
    });
    loop_ctx_.push(LoopContext{});
    this->environment_ = std::make_shared<Environment>(this->environment_);;

    while (evaluate(stmt->condition.get())) {
        execute(stmt->body.get());
        if (loop_ctx_.top().break_loop) {
            break;
        }
    }
    return nullptr;
}

Value Interpreter::visitForStmt(lox::ForStmt *stmt) {
    std::shared_ptr<void> defer(nullptr, [this, previous=this->environment_](void*){
        this->environment_ = previous;
        this->loop_ctx_.pop();
    });
    loop_ctx_.push(LoopContext{});
    this->environment_ = std::make_shared<Environment>(this->environment_);;

    for (execute(stmt->initializer.get()); evaluate(stmt->condition.get()); execute(stmt->increment.get())) {
        execute(stmt->body.get());
        if (loop_ctx_.top().break_loop) {
            break;
        }
    }
    return nullptr;
}

Value Interpreter::execute(Statement *statement) {
    if (statement == nullptr) {
        return nullptr;
    }
    return statement->accept(this);
}

void Interpreter::execute_block(const std::vector<Statement::ptr> &statements, Environment::ptr env) {
    std::shared_ptr<void> defer(nullptr, [this, previous=this->environment_](void*){
        this->environment_ = previous;
    });
    this->environment_ = std::move(env);

    for (const auto &stmt: statements) {
        execute(stmt.get());
        if (!loop_ctx_.empty() && loop_ctx_.top().break_loop) {
            break;
        }
    }
}

Value Interpreter::evaluate(Expr *expr) {
    return expr->accept(this);
}

void Interpreter::interpret(const std::vector<Statement::ptr> &statements) {
    try {
        for (const auto &statement : statements) {
            Value v = execute(statement.get());
            if (repl_mode_) {
                Expression::ptr expression = std::dynamic_pointer_cast<Expression>(statement);
                if (expression) {
                    std::cout << v.str() << std::endl;
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}



} //namespace lox

