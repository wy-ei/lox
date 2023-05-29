//
// Created by wy on 22.5.23.
//

#include "lox/parser.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "lox/exception.h"
#include "lox/function.h"
#include "lox/lox.h"

expr::Expr::ptr Parser::expression() {
    return assignment();
}

expr::Expr::ptr Parser::assignment() {
    expr::Expr::ptr expr = logical_or();

    if (match(Token::EQUAL)) {
        Token::ptr equals = previous();
        expr::Expr::ptr value = assignment();

        auto var = std::dynamic_pointer_cast<expr::Variable>(expr);
        if (var) {
            Token::ptr name = var->name;
            return std::make_shared<expr::Assign>(name, value);
        } else {
            throw RuntimeError(equals, "invalid variable assignment");
        }
    }

    return expr;
}

expr::Expr::ptr Parser::logical_or() {
    expr::Expr::ptr expr = logical_and();

    while (match(Token::OR)) {
        Token::ptr op = previous();
        expr::Expr::ptr right = logical_and();
        expr = std::make_shared<expr::Logical>(expr, op, right);
    }

    return expr;
}

expr::Expr::ptr Parser::logical_and() {
    expr::Expr::ptr expr = equality();

    while (match(Token::AND)) {
        Token::ptr op = previous();
        expr::Expr::ptr right = equality();
        expr = std::make_shared<expr::Logical>(expr, op, right);
    }

    return expr;
}

expr::Expr::ptr Parser::call() {
    expr::Expr::ptr expr = primary();
    while (true) {
        if (match(Token::LEFT_PAREN)) {
            expr = finish_call(expr);
        } else {
            break;
        }
    }
    return expr;
}

expr::Expr::ptr Parser::finish_call(expr::Expr::ptr callee) {
    std::vector<expr::Expr::ptr> arguments;

    if (!check(Token::RIGHT_PAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(Token::COMMA));
    }

    Token::ptr paren = consume(Token::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<expr::Call>(std::move(callee), paren, arguments);
}

expr::Expr::ptr Parser::equality() {
    expr::Expr::ptr expr = comparison();

    while (match({Token::Kind::BANG_EQUAL, Token::Kind::EQUAL_EQUAL})) {
        Token::ptr op = previous();
        expr::Expr::ptr right = comparison();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

expr::Expr::ptr Parser::comparison() {
    expr::Expr::ptr expr = term();

    while (match({Token::Kind::GREATER, Token::Kind::GREATER_EQUAL, Token::Kind::LESS, Token::Kind::LESS_EQUAL})) {
        Token::ptr op = previous();
        expr::Expr::ptr right = term();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

expr::Expr::ptr Parser::term() {
    expr::Expr::ptr expr = factor();

    while (match({Token::Kind::MINUS, Token::Kind::PLUS})) {
        Token::ptr op = previous();
        expr::Expr::ptr right = factor();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

expr::Expr::ptr Parser::factor() {
    expr::Expr::ptr expr = unary();

    while (match({Token::Kind::SLASH, Token::Kind::STAR})) {
        Token::ptr op = previous();
        expr::Expr::ptr right = unary();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

expr::Expr::ptr Parser::unary() {
    if (match({Token::Kind::PLUS, Token::Kind::MINUS})) {
        Token::ptr op = previous();
        expr::Expr::ptr right = unary();
        return std::make_shared<expr::Unary>(op, right);
    } else if (match(Token::BREAK)) {
        return std::make_shared<expr::Break>(previous());
    } else {
        return call();
    }
}

expr::Expr::ptr Parser::primary() {
    if (match(Token::Kind::TRUE)) {
        return std::make_shared<expr::Literal>(true);
    }
    if (match(Token::Kind::FALSE)) {
        return std::make_shared<expr::Literal>(false);
    }
    if (match(Token::Kind::NIL)) {
        return std::make_shared<expr::Literal>(nullptr);
    }
    if (match(Token::Kind::STRING)) {
        return std::make_shared<expr::Literal>(previous()->lexeme);
    }
    if (match(Token::Kind::NUMBER)) {
        double d = std::stod(previous()->lexeme, nullptr);
        return std::make_shared<expr::Literal>(d);
    }
    if (match(Token::Kind::LEFT_PAREN)) {
        expr::Expr::ptr expr = expression();
        consume(Token::Kind::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<expr::Grouping>(expr);
    }
    if (match(Token::IDENTIFIER)) {
        return std::make_shared<expr::Variable>(previous());
    }

    throw RuntimeError(peek(), "unexpected token " + peek()->str());
}

std::vector<stmt::Statement::ptr> Parser::parse() {
    std::vector<stmt::Statement::ptr> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

stmt::Statement::ptr Parser::declaration() {
    try {
        if (match(Token::FUN)) {
            return func_declaration("function");
        }
        if (match(Token::VAR)) {
            return var_declaration();
        }
        return statement();
    } catch (const RuntimeError &e) {
        //        synchronize();
        //        return null;
        throw e;
    }
}

stmt::Statement::ptr Parser::var_declaration() {
    Token::ptr name = consume(Token::IDENTIFIER, "Expect variable name.");
    expr::Expr::ptr initializer = nullptr;
    if (match(Token::EQUAL)) {
        initializer = expression();
    }
    consume(Token::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<stmt::Var>(name, initializer);
}

stmt::Statement::ptr Parser::func_declaration(const std::string &kind) {
    Token::ptr name = consume(Token::IDENTIFIER, "Expect " + kind + " name.");
    consume(Token::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token::ptr> parameters;
    if (!check(Token::RIGHT_PAREN)) {
        do {
            if (parameters.size() > 255) {
                throw RuntimeError(peek(), "Too many parameters.");
            }

            parameters.push_back(consume(Token::IDENTIFIER, "Expect parameter name."));
        } while (match(Token::COMMA));
    }

    consume(Token::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(Token::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    stmt::Statement::ptr body = block_statement();

    return std::make_shared<stmt::Function>(name, parameters, body);
}

stmt::Statement::ptr Parser::statement() {
    if (match(Token::PRINT)) {
        return print_statement();
    }
    if (match(Token::LEFT_BRACE)) {
        return block_statement();
    }
    if (match(Token::IF)) {
        return if_statement();
    }
    if (match(Token::WHILE)) {
        return while_statement();
    }
    if (match(Token::FOR)) {
        return for_statement();
    }
    if (match(Token::RETURN)) {
        return return_statement();
    }
    return expression_statement();
}

stmt::Statement::ptr Parser::print_statement() {
    expr::Expr::ptr value = expression();
    consume(Token::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<stmt::Print>(value);
}

stmt::Statement::ptr Parser::expression_statement() {
    expr::Expr::ptr expr = expression();
    consume(Token::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<stmt::Expression>(expr);
}

stmt::Statement::ptr Parser::block_statement() {
    std::vector<stmt::Statement::ptr> statements;
    while (!check(Token::RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(declaration());
    }
    consume(Token::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_shared<stmt::Block>(statements);
}

stmt::Statement::ptr Parser::if_statement() {
    consume(Token::LEFT_PAREN, "Expect '(' after 'if'.");
    expr::Expr::ptr condition = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after if condition.");

    stmt::Statement::ptr thenBranch = statement();
    stmt::Statement::ptr elseBranch = nullptr;
    if (match(Token::ELSE)) {
        elseBranch = statement();
    }
    return std::make_shared<stmt::If>(condition, thenBranch, elseBranch);
}

stmt::Statement::ptr Parser::while_statement() {
    consume(Token::LEFT_PAREN, "Expect '(' after 'if'.");
    expr::Expr::ptr condition = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after while condition.");

    stmt::Statement::ptr body = statement();
    return std::make_shared<stmt::While>(condition, body);
}

stmt::Statement::ptr Parser::for_statement() {
    consume(Token::LEFT_PAREN, "Expect '(' after 'for'.");
    stmt::Statement::ptr initializer = nullptr;
    if (match(Token::SEMICOLON)) {
        initializer = nullptr;
    } else if (match(Token::VAR)) {
        initializer = var_declaration();
    } else {
        initializer = expression_statement();
    }

    expr::Expr::ptr condition;
    if (!check(Token::SEMICOLON)) {
        condition = expression();
    } else {
        condition = std::make_shared<expr::Literal>(true);
    }
    consume(Token::SEMICOLON, "Expect ';' after loop condition.");

    stmt::Statement::ptr increment;
    if (!check(Token::RIGHT_PAREN)) {
        increment = std::make_shared<stmt::Expression>(expression());
    }
    consume(Token::RIGHT_PAREN, "Expect ')' after loop clauses.");

    stmt::Statement::ptr body = statement();

    return std::make_shared<stmt::For>(initializer, condition, increment, body);
}

stmt::Statement::ptr Parser::return_statement() {
    Token::ptr token = previous();
    expr::Expr::ptr value = nullptr;
    if (!check(Token::SEMICOLON)) {
        value = expression();
    }
    consume(Token::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<stmt::Return>(token, value);
}
