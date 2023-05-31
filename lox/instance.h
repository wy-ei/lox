//
// Created by wy on 31.5.23.
//

#pragma once

#include <memory>
#include <unordered_map>

#include "lox/klass.h"
#include "lox/value.h"
#include "lox/token.h"

class LoxInstance: public std::enable_shared_from_this<LoxInstance> {
public:
    using ptr = std::shared_ptr<LoxInstance>;
    explicit LoxInstance(LoxClass klass): klass_(std::move(klass)) {}

    std::string str() const;

    Value get(const Token::ptr &name);
    void set(const Token::ptr &name, Value value);

private:
    LoxClass klass_;
    std::unordered_map<std::string, Value> fields_;
};