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

class Function : public Callable {
 public:
    explicit Function(std::shared_ptr<stmt::Function> func, Environment::ptr closure)
        : func_(std::move(func)), closure_(std::move(closure)) {}

    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override;

    int arity() override;

    std::string name() override;

 private:
    std::shared_ptr<stmt::Function> func_;
    Environment::ptr closure_;
};
