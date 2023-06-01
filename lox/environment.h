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

class Environment {
 public:
    using ptr = std::shared_ptr<Environment>;

    Environment() = default;

    explicit Environment(ptr enclosing) : enclosing_(std::move(enclosing)) {}

    void define(const std::string &name, const Value &value) {
        values_[name] = value;
    }


    Value get(size_t depth, const std::string &name) {
        Environment *env = this;
        while (depth > 0) {
            env = env->enclosing_.get();
            depth--;
        }
        return env->values_.count(name) ? env->values_[name] : nullptr;
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

    Value get(const std::string &name) {
        if (values_.count(name)) {
            return values_[name];
        }

        if (enclosing_ != nullptr) {
            return enclosing_->get(name);
        }

        throw std::runtime_error("Undefined variable '" + name + "'.");
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

    ptr enclosing() const {
        return enclosing_;
    }

    void print() {
        Environment *env = this;
        int i = 0;
        while (env) {
            std::string space(i * 4, ' ');
            for (const auto &item : env->values_) {
                std::cout << space << item.first << ": " << item.second.str() << std::endl;
            }
            i++;
            env = env->enclosing_.get();
        }
    }

 private:
    std::unordered_map<std::string, Value> values_;
    std::unordered_map<std::string, Value> internal_values_;

    ptr enclosing_{nullptr};
};
