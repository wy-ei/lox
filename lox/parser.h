//
// Created by wy on 22.5.23.
//

#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "lox/token.h"
#include "lox/expr.h"
#include "lox/exception.h"
#include "lox/statement.h"

namespace lox {


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

    std::vector<Statement::ptr>  parse();

private:

    Statement::ptr declaration();
    Statement::ptr var_declaration();
    Statement::ptr statement();
    Statement::ptr print_statement();
    Statement::ptr expression_statement();
    Statement::ptr block_statement();
    Statement::ptr if_statement();
    Statement::ptr while_statement();
    Statement::ptr for_statement();

    Expr::ptr assignment();
    Expr::ptr expression();
    Expr::ptr equality();
    Expr::ptr comparison();
    Expr::ptr term();
    Expr::ptr factor();
    Expr::ptr unary();
    Expr::ptr primary();
    Expr::ptr logical_or();
    Expr::ptr logical_and();



    bool match(Token::Kind kind) {
        if (check(kind)) {
            advance();
            return true;
        }
        return false;
    }

    bool match(const std::vector<Token::Kind> &kinds) {
        for (auto kind: kinds) {
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
        return peek()->kind == Token::Kind::END;
    }

    bool check(Token::Kind kind) {
        if (is_at_end()) return false;
        return peek()->kind == kind;
    }

    const Token::ptr &previous() {
        return tokens_[current_ - 1];
    }

    const Token::ptr &consume(Token::Kind kind, const std::string &message) {
        if (check(kind)) return advance();
        throw lox::RuntimeError(peek(), message);
    }

    std::vector<Token::ptr> tokens_;
    int current_{0};
};

}


#endif //LOX_PARSER_H
