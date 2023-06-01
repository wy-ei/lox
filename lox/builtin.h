//
// Created by wy on 29.5.23.
//

#include <ctime>
#include <string>
#include <sys/time.h>
#include <vector>

#include "lox/callable.h"
#include "lox/interpreter.h"
#include "lox/value.h"

class Clock : public Callable {
 public:
    std::string name() const override {
        return "now";
    }
    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override {
        timeval tv;
        gettimeofday(&tv, nullptr);
        uint64_t us = tv.tv_sec * 1000000 + tv.tv_usec;
        return static_cast<double>(us);
    }
    int arity() const override {
        return 0;
    }
};

class Assert : public Callable {
 public:
    std::string name() const override {
        return "assert";
    }
    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override {
        if (!arguments[0]) {
            throw std::runtime_error("assert failed");
        }
        return nullptr;
    }
    int arity() const override {
        return 1;
    }
};

class Str : public Callable {
 public:
    std::string name() const override {
        return "str";
    }
    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override {
        return arguments[0].str();
    }
    int arity() const override {
        return 1;
    }
};

class Getc : public Callable {
public:
    std::string name() const override {
        return "getc";
    }
    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override {
        return (double)getchar();
    }
    int arity() const override {
        return 0;
    }
};

class Chr : public Callable {
public:
    std::string name() const override {
        return "chr";
    }
    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override {
        char c = static_cast<char>(arguments[0].as<double>());
        std::string s {c};
        return s;
    }
    int arity() const override {
        return 1;
    }
};

class Exit : public Callable {
public:
    std::string name() const override {
        return "exit";
    }
    Value call(Interpreter *interpreter, const std::vector<Value> &arguments) override {
        int n = static_cast<int>(arguments[0].as<double>());
        exit(n);
        return nullptr;
    }
    int arity() const override {
        return 1;
    }
};
