//
// Created by wy on 29.5.23.
//

#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lox/callable.h"
#include "lox/function.h"
#include "lox/token.h"

class LoxClass : public Callable {
 public:
    using ptr = std::shared_ptr<LoxClass>;

    explicit LoxClass(
        std::string name, ptr super, std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods)
        : name_(std::move(name)), super_(std::move(super)), methods_(std::move(methods)) {}

    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override;

    std::string name() const override {
        return name_;
    }

    std::string str() const {
        return "class<" + name_ + ">";
    }

    int arity() const override;

    std::shared_ptr<LoxFunction> find_method(const std::string &name) const;

 private:
    std::string name_;
    ptr super_;
    std::unordered_map<std::string, LoxFunction::ptr> methods_;
};
