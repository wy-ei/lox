//
// Created by wy on 22.5.23.
//

#pragma once

#include <utility>
#include <string>
#include <vector>

#include "lox/exception.h"
#include "lox/expr.h"
#include "lox/statement.h"
#include "lox/token.h"

/*
 * Current Parser expression handling:
 *  expression     → assignment ;
 *  assignment     → IDENTIFIER "=" assignment
 *                  | equality ;
 *  equality       → comparison ( ( "!=" | "==" ) comparison )* ;
 *  comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 *  term           → factor ( ( "-" | "+" ) factor )* ;
 *  factor         → unary ( ( "/" | "*" ) unary )* ;
 *  unary          → ( "!" | "-" ) unary
 *                  | function
 *                  | call ;
 *  call           → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
 *  arguments      → expression ( ",", expression )* ;
 *  primary        → NUMBER | STRING | "true" | "false" | "nil"
 *                 | "(" expression ")"
 *                 | "super" "." IDENTIFIER;
 */

class Parser {
 public:
    explicit Parser(std::vector<Token::ptr> tokens) : tokens_(std::move(tokens)) {}

    std::vector<stmt::Statement::ptr> parse();

 private:

    expr::Expr::ptr assignment();
    expr::Expr::ptr expression();
    expr::Expr::ptr equality();
    expr::Expr::ptr comparison();
    expr::Expr::ptr term();
    expr::Expr::ptr factor();
    expr::Expr::ptr unary();
    expr::Expr::ptr primary();
    expr::Expr::ptr logical_or();
    expr::Expr::ptr logical_and();
    expr::Expr::ptr call();
    expr::Expr::ptr finish_call(expr::Expr::ptr callee);

    stmt::Statement::ptr declaration();
    stmt::Statement::ptr var_declaration();
    stmt::Function::ptr func_declaration(const std::string &kind);
    stmt::Statement::ptr statement();
    stmt::Statement::ptr print_statement();
    stmt::Statement::ptr expression_statement();
    stmt::Statement::ptr block_statement();
    stmt::Statement::ptr if_statement();
    stmt::Statement::ptr while_statement();
    stmt::Statement::ptr for_statement();
    stmt::Statement::ptr return_statement();
    stmt::Statement::ptr class_declaration();

    bool match(Token::Kind kind) {
        if (check(kind)) {
            advance();
            return true;
        }
        return false;
    }

    bool match(const std::vector<Token::Kind> &kinds) {
        for (auto kind : kinds) {
            if (check(kind)) {
                advance();
                return true;
            }
        }
        return false;
    }

    const Token::ptr &peek() {
        return tokens_[current_];
    }

    const Token::ptr &advance() {
        if (!is_at_end()) {
            current_++;
        }
        return previous();
    }

    bool is_at_end() {
        return peek()->kind == Token::END;
    }

    bool check(Token::Kind kind) {
        if (is_at_end())
            return false;
        return peek()->kind == kind;
    }

    const Token::ptr &previous() {
        return tokens_[current_ - 1];
    }

    const Token::ptr &consume(Token::Kind kind, const std::string &message) {
        if (check(kind))
            return advance();
        throw RuntimeError(peek(), message);
    }

    std::vector<Token::ptr> tokens_;
    int current_{0};
};
