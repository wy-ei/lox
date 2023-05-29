//
// Created by wy on 27.5.23.
//

#pragma once

#include <string>
#include <vector>

#include "lox/value.h"

class Interpreter;

class Callable {
 public:
    virtual std::string name() = 0;
    virtual int arity() = 0;
    virtual Value call(Interpreter *interpreter, const std::vector<Value> &arguments) = 0;

    virtual ~Callable() = default;
};
