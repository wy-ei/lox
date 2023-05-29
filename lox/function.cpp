//
// Created by wy on 27.5.23.
//

#include "lox/function.h"
#include "lox/environment.h"
#include "lox/interpreter.h"
#include "lox/return.h"
#include "lox/value.h"

Value Function::call(Interpreter *interpreter, const std::vector<Value> &arguments) {
    Environment::ptr env = std::make_shared<Environment>(closure_);

    for (size_t i = 0; i < func_->params.size(); i++) {
        env->define(func_->params[i]->lexeme, arguments[i]);
    }
    auto bd = std::dynamic_pointer_cast<stmt::Block>(func_->body);
    try {
        interpreter->execute_block(bd->statements, env);
    } catch (const Return &ret) {
        return ret.value;
    }
    return nullptr;
}

int Function::arity() {
    return static_cast<int>(func_->params.size());
}

std::string Function::name() {
    return func_->name->lexeme;
}
