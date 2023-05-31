//
// Created by wy on 17.5.23.
//

#include "lox/token.h"
#include <sstream>
#include <string>
#include <utility>

Token::Token(Token::Kind kind, std::string lexeme, int line) : kind(kind), lexeme(std::move(lexeme)), line(line) {}