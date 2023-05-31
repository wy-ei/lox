//
// Created by wy on 31.5.23.
//

#include "lox/instance.h"

std::string LoxInstance::str() const {
    return "instance<" + klass_.str() + ">";
}

Value LoxInstance::get(const Token::ptr &name) {
    if (fields_.count(name->lexeme)) {
        return fields_[name->lexeme];
    }
    LoxFunction::ptr method = klass_.find_method(name->lexeme);
    if (method) {
        method = method->bind(this->shared_from_this());
        return method;
    }

    throw RuntimeError(name, "Undefined property '" + name->lexeme + "'.");
}

void LoxInstance::set(const Token::ptr &name, Value value) {
    fields_[name->lexeme] = std::move(value);
}
