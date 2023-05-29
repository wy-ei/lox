//
// Created by wy on 22.5.23.
//

#pragma once

#include <exception>
#include <utility>
#include <stdexcept>
#include <string>

#include "lox/value.h"
#include "lox/token.h"

class RuntimeError : public std::runtime_error {
 public:
    RuntimeError(Token::ptr token, const std::string &message) : std::runtime_error(message), token(std::move(token)) {}

    Token::ptr token;
};

class TypeError : public std::exception {
 public:
    explicit TypeError(const std::string &message) : message_(message) {}
    const char *what() const noexcept override {
        return message_.c_str();
    }

 private:
    std::string message_;
};


class Return : public std::exception {
 public:
    explicit Return(Value value) : value(value) {}

    Value value;
};

class BreakException : public std::exception {};
