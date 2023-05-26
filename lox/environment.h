//
// Created by wy on 26.5.23.
//

#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include <unordered_map>
#include <bitset>
#include <iostream>
#include <string>
#include "lox/value.h"
#include "lox/token.h"
#include "lox/exception.h"

namespace lox {

const std::string LOOP_BREAK = "LOOP_BREAK";

class Environment {
public:
    using ptr = std::shared_ptr<Environment>;

    Environment() = default;

    explicit Environment(ptr enclosing): enclosing_(std::move(enclosing)) {}

    void define(const std::string &name, const Value &value) {
        values_[name] = value;
    }

    Value get(const Token::ptr &name) {
        if (values_.count(name->lexeme)) {
            return values_[name->lexeme];
        }

        if (enclosing_ != nullptr) {
            return enclosing_->get(name);
        }

        throw RuntimeError(name, "Undefined variable '" + name->lexeme + "'.");
    }

    void assign(const Token::ptr &name, const Value &value) {
        if (values_.count(name->lexeme)) {
            values_[name->lexeme] = value;
            return;
        }

        if (enclosing_ != nullptr) {
            enclosing_->assign(name, value);
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name->lexeme + "'.");
    }

private:
    std::unordered_map<std::string, Value> values_;
    std::unordered_map<std::string, Value> internal_values_;

    ptr enclosing_ {nullptr};
};

}

#endif //LOX_ENVIRONMENT_H
