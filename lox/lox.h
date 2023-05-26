//
// Created by wy on 17.5.23.
//
#pragma once

#include <string>
#include "lox/token.h"
#include "lox/interpreter.h"

namespace lox {

class Lox {
public:
    void execute_script(const std::string &filepath);

    void prompt();

private:
    void execute(const std::string &content);

    Interpreter interpreter_;
};

}

