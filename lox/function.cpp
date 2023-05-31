//
// Created by wy on 27.5.23.
//

#include "lox/function.h"

#include "lox/environment.h"
#include "lox/interpreter.h"
#include "lox/return.h"
#include "lox/value.h"
#include <iostream>
#include <utility>

Value LoxFunction::call(Interpreter *interpreter, const std::vector<Value> &arguments) {
    Environment::ptr env = std::make_shared<Environment>(closure_);

    for (size_t i = 0; i < func_->params.size(); i++) {
        env->define(func_->params[i]->lexeme, arguments[i]);
    }
    auto bd = std::dynamic_pointer_cast<stmt::Block>(func_->body);
    try {
        interpreter->execute_block(bd->statements, env);
    } catch (const ReturnException &ret) {
        return ret.value;
    }

    if (is_initializer) {
        return env->get(0, "this");
    }
    return nullptr;
}

int LoxFunction::arity() const {
    return static_cast<int>(func_->params.size());
}

std::string LoxFunction::name() const {
    return func_->name->lexeme;
}

std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
    auto env = std::make_shared<Environment>(this->closure_);
    env->define("this", std::move(instance));
    return std::make_shared<LoxFunction>(func_, env);
}
