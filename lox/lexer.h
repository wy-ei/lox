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

 private:
    void scan_next();
    void add_token(Token::Kind kind);
    void add_token(Token::Kind kind, std::string lexeme);

    bool is_at_end();

    void string();
    void number();
    void identifier();

    bool check(char ch);
    char consume();
    char lookahead(int i);

    std::vector<Token::ptr> tokens_;
    std::string source_;
    int start_{0};
    int current_{0};
    int line_{1};
    int col_ {0};
};
