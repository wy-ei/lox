//
// Created by wy on 22.5.23.
//

#include "lox/interpreter.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "lox/builtin.h"
#include "lox/function.h"
#include "lox/instance.h"
#include "lox/klass.h"
#include "lox/return.h"
#include "lox/token.h"

Interpreter::Interpreter() {
    globals_environment_ = std::make_shared<Environment>();
    globals_environment_->define("clock", std::shared_ptr<Callable>(new Clock()));
    globals_environment_->define("assert", std::shared_ptr<Callable>(new Assert()));
    globals_environment_->define("str", std::shared_ptr<Callable>(new Str()));
    globals_environment_->define("getc", std::shared_ptr<Callable>(new Getc()));
    globals_environment_->define("chr", std::shared_ptr<Callable>(new Chr()));
    globals_environment_->define("exit", std::shared_ptr<Callable>(new Exit()));
    environment_ = std::make_shared<Environment>(globals_environment_);
}

Value Interpreter::visit_literal_expr(expr::Literal *expr) {
    return expr->value;
}

Value Interpreter::visit_grouping_expr(expr::Grouping *expr) {
    return evaluate(expr->expression.get());
}

Value Interpreter::visit_unary_expr(expr::Unary *expr) {
    Value value = evaluate(expr->right.get());
    switch (expr->op->kind) {
    case Token::Kind::MINUS:
        return -value.as<double>();
    case Token::Kind::BANG:
        return !value;
    default:
        throw RuntimeError(expr->op, "Unknown unary operator");
    }
}

Value Interpreter::visit_binary_expr(expr::Binary *expr) {
    Value left = evaluate(expr->left.get());
    Value right = evaluate(expr->right.get());

    try {
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
            throw std::runtime_error("unknown binary operator");
        }
    } catch (const std::exception &e) {
        throw RuntimeError(expr->op, e.what());
    }
}

Value Interpreter::visit_variable_expr(expr::Variable *expr) {
    return environment_->get(expr->name);
}

Value Interpreter::visit_assign_expr(expr::Assign *expr) {
    Value value = evaluate(expr->value.get());
    environment_->assign(expr->name, value);
    return value;
}

Value Interpreter::visit_break_expr(expr::Break *expr) {
    throw BreakException(expr->keyword);
}

Value Interpreter::visit_call_expr(expr::Call *expr) {
    Value callee = evaluate(expr->callee.get());
    std::vector<Value> arguments;
    for (const auto &arg : expr->arguments) {
        arguments.push_back(evaluate(arg.get()));
    }

    std::shared_ptr<Callable> callable;
    if (callee.is<LoxFunction::ptr>()) {
        callable = std::dynamic_pointer_cast<Callable>(callee.as<LoxFunction::ptr>());
    } else if (callee.is<LoxClass::ptr>()) {
        callable = std::dynamic_pointer_cast<Callable>(callee.as<LoxClass::ptr>());
    } else if (callee.is<Callable::ptr>()) {
        callable = callee.as<Callable::ptr>();
    } else {
        throw RuntimeError(expr->paren, "function or method is required");
    }

    if (arguments.size() != callable->arity()) {
        std::ostringstream os;
        os << "function " << callable->name() << " require " << callable->arity() << " argument(s) but "
           << arguments.size() << " given.";
        throw RuntimeError(expr->paren, os.str());
    }

    return callable->call(this, arguments);
}

Value Interpreter::visit_get_expr(expr::Get *expr) {
    Value object = evaluate(expr->object.get());
    if (object.is<LoxInstance::ptr>()) {
        return object.as<LoxInstance::ptr>()->get(expr->name);
    }
    throw RuntimeError(expr->name, "Only instances have properties.");
}

Value Interpreter::visit_set_expr(expr::Set *expr) {
    Value object = evaluate(expr->object.get());

    if (object.is<std::shared_ptr<LoxInstance>>()) {
        auto ins = object.as<std::shared_ptr<LoxInstance>>();
        Value value = evaluate(expr->value.get());
        ins->set(expr->name, value);
        return value;
    }

    throw RuntimeError(expr->name, "Only instances have fields.");
}

Value Interpreter::visit_this_expr(expr::This *expr) {
    return environment_->get(expr->name);
}

Value Interpreter::visit_super_expr(expr::Super *expr) {
    auto super = environment_->get(expr->keyword).as<LoxClass::ptr>();
    auto object = environment_->get("this").as<LoxInstance::ptr>();

    LoxFunction::ptr method = super->find_method(expr->method->lexeme);
    if (method == nullptr) {
        throw RuntimeError(expr->method, "Undefined property '" + expr->method->lexeme + "'.");
    }

    method = method->bind(object);
    return method;
}

Value Interpreter::visit_expression_stmt(stmt::Expression *stmt) {
    Value value = evaluate(stmt->expression.get());
    return value;
}

Value Interpreter::visit_print_stmt(stmt::Print *stmt) {
    Value value = evaluate(stmt->expression.get());
    std::cout << value.str() << std::endl;
    return nullptr;
}

Value Interpreter::visit_var_stmt(stmt::Var *stmt) {
    Value value;
    if (stmt->value != nullptr) {
        value = evaluate(stmt->value.get());
    }
    environment_->define(stmt->name->lexeme, value);

    return nullptr;
}

Value Interpreter::visit_block_stmt(stmt::Block *stmt) {
    execute_block(stmt->statements, std::make_shared<Environment>(environment_));
    return nullptr;
}

Value Interpreter::visit_if_stmt(stmt::If *stmt) {
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

Value Interpreter::visit_logical_expr(expr::Logical *expr) {
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

Value Interpreter::visit_while_stmt(stmt::While *stmt) {
    std::shared_ptr<void> defer(nullptr, [this, previous = this->environment_](void *) {
        this->environment_ = previous;
    });
    this->environment_ = std::make_shared<Environment>(this->environment_);

    while (evaluate(stmt->condition.get())) {
        try {
            execute(stmt->body.get());
        } catch (const BreakException &e) {
            break;
        }
    }
    return nullptr;
}

Value Interpreter::visit_for_stmt(stmt::For *stmt) {
    std::shared_ptr<void> defer(nullptr, [this, previous = this->environment_](void *) {
        this->environment_ = previous;
    });
    this->environment_ = std::make_shared<Environment>(this->environment_);

    for (execute(stmt->initializer.get()); evaluate(stmt->condition.get()); execute(stmt->increment.get())) {
        try {
            execute(stmt->body.get());
        } catch (const BreakException &e) {
            break;
        }
    }
    return nullptr;
}

Value Interpreter::visit_function_stmt(stmt::Function *stmt) {
    auto func = std::make_shared<LoxFunction>(stmt->shared_from_this(), environment_);
    auto callable = std::dynamic_pointer_cast<Callable>(func);
    this->environment_->define(stmt->name->lexeme, callable);
    return callable;
}

Value Interpreter::visit_return_stmt(stmt::Return *stmt) {
    Value value = nullptr;
    if (stmt->value) {
        value = evaluate(stmt->value.get());
    }
    throw ReturnException(value);
}

Value Interpreter::visit_class_stmt(stmt::Class *stmt) {
    Value superclass = nullptr;
    if (stmt->super) {
        superclass = evaluate(stmt->super.get());
        if (!superclass.is<LoxClass::ptr>()) {
            throw RuntimeError(stmt->super->name, "Superclass must be a class.");
        }
    }
    environment_->define(stmt->name->lexeme, nullptr);
    if (stmt->super) {
        environment_ = std::make_shared<Environment>(environment_);
        environment_->define("super", superclass);
    }

    std::unordered_map<std::string, LoxFunction::ptr> methods;
    for (const auto &item : stmt->methods) {
        auto fn = std::make_shared<LoxFunction>(item, environment_);
        methods[item->name->lexeme] = fn;
    }

    LoxClass::ptr super;
    if (stmt->super) {
        environment_ = environment_->enclosing();
        super = superclass.as<LoxClass::ptr>();
    }
    auto klass = std::make_shared<LoxClass>(stmt->name->lexeme, super, methods);
    environment_->assign(stmt->name, klass);
    return nullptr;
}

Value Interpreter::execute(stmt::Statement *statement) {
    if (statement == nullptr) {
        return nullptr;
    }
    return statement->accept(this);
}

void Interpreter::execute_block(const std::vector<stmt::Statement::ptr> &statements, Environment::ptr env) {
    std::shared_ptr<void> defer(nullptr, [this, previous = this->environment_](void *) {
        this->environment_ = previous;
    });
    this->environment_ = std::move(env);

    for (const auto &stmt : statements) {
        execute(stmt.get());
    }
}

Value Interpreter::evaluate(expr::Expr *expr) {
    return expr->accept(this);
}

void Interpreter::interpret(const std::vector<stmt::Statement::ptr> &statements) {
    try {
        for (const auto &statement : statements) {
            Value v = execute(statement.get());
            if (repl_mode_) {
                stmt::Expression::ptr expression = std::dynamic_pointer_cast<stmt::Expression>(statement);
                if (expression) {
                    std::cout << v.str() << std::endl;
                }
            }
        }
    } catch (const BreakException &e) {
        throw RuntimeError(e.token, "break must in the body of 'for' or 'while'");
    }
}
