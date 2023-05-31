//
// Created by wy on 22.5.23.
//

#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

#include "lox/token.h"
#include "lox/value.h"

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

class ReturnException : public std::exception {
 public:
    explicit ReturnException(Value value) : value(std::move(value)) {}

    Value value;
};

class BreakException : public std::exception {};
