//
// Created by wy on 27.5.23.
//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "lox/callable.h"
#include "lox/environment.h"
#include "lox/statement.h"

class Interpreter;
class LoxInstance;

class LoxFunction : public Callable {
 public:
    using ptr = std::shared_ptr<LoxFunction>;
    explicit LoxFunction(std::shared_ptr<stmt::Function> func, Environment::ptr closure)
        : func_(std::move(func)), closure_(std::move(closure)) {}

    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override;

    int arity() const override;

    std::string name() const override;

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance);

    bool is_initializer{false};

    std::string str() const {
        return "function<" + name() + ">";
    }

 private:
    std::shared_ptr<stmt::Function> func_;
    Environment::ptr closure_;
};
