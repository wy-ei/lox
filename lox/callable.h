//
// Created by wy on 27.5.23.
//

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "lox/value.h"

class Interpreter;

class Callable {
 public:
    using ptr = std::shared_ptr<Callable>;

    virtual std::string name() const = 0;
    virtual int arity() const = 0;
    virtual Value call(Interpreter *interpreter, const std::vector<Value> &arguments) = 0;

    virtual ~Callable() = default;
};
