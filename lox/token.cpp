//
// Created by wy on 17.5.23.
//

#include "lox/token.h"
#include <sstream>
#include <string>
#include <utility>

Token::Token(Token::Kind kind) : kind(kind) {}

Token::Token(Token::Kind kind, std::string lexeme) : kind(kind), lexeme(std::move(lexeme)) {}

std::string Token::str() const {
    std::ostringstream os;
    os << static_cast<int>(kind) << ":" << lexeme;
    return os.str();
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << token.str();
    return os;
}
