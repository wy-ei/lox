//
// Created by wy on 17.5.23.
//
#pragma once

#include <memory>
#include <ostream>
#include <string>

class Token {
 public:
    using ptr = std::shared_ptr<Token>;

    enum Kind {
        // Single-character tokens.
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,
        NUMBER_SIGN,
        COLON,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // logical operators.
        BIT_AND,
        BIT_OR,
        BIT_NOT,

        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,
        BREAK,

        END,
        UNEXPECTED,
    };

    Token(Kind kind, std::string lexeme, int line);

    Kind kind;
    std::string lexeme;
    int line;
};
