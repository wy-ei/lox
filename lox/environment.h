//
// Created by wy on 26.5.23.
//

#pragma once

#include "lox/exception.h"
#include "lox/token.h"
#include "lox/value.h"
#include <bitset>
#include <iostream>
#include <string>
#include <unordered_map>

const std::string LOOP_BREAK = "LOOP_BREAK";

class Environment {
 public:
    using ptr = std::shared_ptr<Environment>;

    Environment() = default;

    explicit Environment(ptr enclosing) : enclosing_(std::move(enclosing)) {}

    void define(const std::string &name, const Value &value) {
        values_[name] = value;
    }

    bool local_has(const std::string &name) const {
        auto it = values_.find(name);
        return it != values_.end();
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

    ptr enclosing_{nullptr};
};
