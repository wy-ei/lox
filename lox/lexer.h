//
// Created by wy on 17.5.23.
//

#pragma once

#include <string>
#include <vector>

#include "lox/token.h"

class Lexer {
 public:
    explicit Lexer(std::string source);

    std::vector<Token::ptr> scan();

    Token next();

 private:
    bool end();

    Token string();
    Token number();
    Token identifier();

    char lookahead(unsigned i);
    void consume();

    std::string source_;
    int start_{0};
    int current_{0};
    int line_{1};
};
