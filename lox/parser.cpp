//
// Created by wy on 22.5.23.
//

#include "lox/parser.h"
#include "lox/lox.h"
#include "lox/exception.h"

namespace lox {

Expr::ptr Parser::expression() {
    return assignment();
}

Expr::ptr Parser::assignment() {
    Expr::ptr expr = logical_or();

    if (match(Token::EQUAL)) {
        Token::ptr equals = previous();
        Expr::ptr value = assignment();

        auto var = std::dynamic_pointer_cast<Variable>(expr);
        if (var) {
            Token::ptr name = var->name;
            return std::make_shared<Assign>(name, value);
        } else {
            throw RuntimeError(equals, "invalid variable assignment");
        }
    }

    return expr;
}

Expr::ptr Parser::logical_or() {
    Expr::ptr expr = logical_and();

    while (match(Token::OR)) {
        Token::ptr op = previous();
        Expr::ptr right = logical_and();
        expr = std::make_shared<Logical>(expr, op, right);
    }

    return expr;
}

Expr::ptr Parser::logical_and() {
    Expr::ptr expr = equality();

    while (match(Token::AND)) {
        Token::ptr op = previous();
        Expr::ptr right = equality();
        expr = std::make_shared<Logical>(expr, op, right);
    }

    return expr;
}

Expr::ptr Parser::equality() {
    Expr::ptr expr = comparison();

    while (match({Token::Kind::BANG_EQUAL, Token::Kind::EQUAL_EQUAL})) {
        Token::ptr op = previous();
        Expr::ptr right = comparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

Expr::ptr Parser::comparison() {
    Expr::ptr expr = term();

    while (match({Token::Kind::GREATER, Token::Kind::GREATER_EQUAL, Token::Kind::LESS, Token::Kind::LESS_EQUAL})) {
        Token::ptr op = previous();
        Expr::ptr right = term();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

Expr::ptr Parser::term() {
    Expr::ptr expr = factor();

    while (match({Token::Kind::MINUS, Token::Kind::PLUS})) {
        Token::ptr op = previous();
        Expr::ptr right = factor();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

Expr::ptr Parser::factor() {
    Expr::ptr expr = unary();

    while (match({Token::Kind::SLASH, Token::Kind::STAR})) {
        Token::ptr op = previous();
        Expr::ptr right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

Expr::ptr Parser::unary() {
    if (match({Token::Kind::PLUS, Token::Kind::MINUS})) {
        Token::ptr op = previous();
        Expr::ptr right = unary();
        return std::make_shared<Unary>(op, right);
    } else if (match(Token::BREAK)) {
        return std::make_shared<Break>();
    } else {
        return primary();
    }
}

Expr::ptr Parser::primary() {
    if (match(Token::Kind::TRUE)) {
        return std::make_shared<Literal>(true);
    }
    if (match(Token::Kind::FALSE)) {
        return std::make_shared<Literal>(false);
    }
    if (match(Token::Kind::NIL)) {
        return std::make_shared<Literal>(nullptr);
    }
    if (match(Token::Kind::STRING)) {
        return std::make_shared<Literal>(previous()->lexeme);
    }
    if (match(Token::Kind::NUMBER)) {
        double d = std::stod(previous()->lexeme, nullptr);
        return std::make_shared<Literal>(d);
    }
    if (match(Token::Kind::LEFT_PAREN)) {
        Expr::ptr expr = expression();
        consume(Token::Kind::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }
    if (match(Token::IDENTIFIER)) {
        return std::make_shared<Variable>(previous());
    }

    throw lox::RuntimeError(peek(), "unexpected token " + peek()->str());
}

std::vector<Statement::ptr> Parser::parse() {
    std::vector<Statement::ptr> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

Statement::ptr Parser::declaration() {
    try {
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

Statement::ptr Parser::var_declaration() {
    Token::ptr name = consume(Token::IDENTIFIER, "Expect variable name.");
    Expr::ptr initializer = nullptr;
    if (match(Token::EQUAL)) {
        initializer = expression();
    }
    consume(Token::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Var>(name, initializer);
}

Statement::ptr Parser::statement() {
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
    return expression_statement();
}

Statement::ptr Parser::print_statement() {
    Expr::ptr value = expression();
    consume(Token::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<Print>(value);
}

Statement::ptr Parser::expression_statement() {
    Expr::ptr expr = expression();
    consume(Token::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<Expression>(expr);
}

Statement::ptr Parser::block_statement() {
    std::vector<Statement::ptr> statements;
    while (!check(Token::RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(declaration());
    }
    consume(Token::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_shared<Block>(statements);
}

Statement::ptr Parser::if_statement() {
    consume(Token::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr::ptr condition = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after if condition.");

    Statement::ptr thenBranch = statement();
    Statement::ptr elseBranch = nullptr;
    if (match(Token::ELSE)) {
        elseBranch = statement();
    }
    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

Statement::ptr Parser::while_statement() {
    consume(Token::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr::ptr condition = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after while condition.");

    Statement::ptr body = statement();
    return std::make_shared<WhileStmt>(condition, body);
}

Statement::ptr Parser::for_statement() {
    consume(Token::LEFT_PAREN, "Expect '(' after 'for'.");
    Statement::ptr initializer = nullptr;
    if (match(Token::SEMICOLON)) {
        initializer = nullptr;
    } else if (match(Token::VAR)) {
        initializer = var_declaration();
    } else {
        initializer = expression_statement();
    }

    Expr::ptr condition;
    if (!check(Token::SEMICOLON)) {
        condition = expression();
    } else {
        condition = std::make_shared<Literal>(true);
    }
    consume(Token::SEMICOLON, "Expect ';' after loop condition.");

    Statement::ptr increment;
    if (!check(Token::RIGHT_PAREN)) {
        increment = std::make_shared<Expression>(expression());
    }
    consume(Token::RIGHT_PAREN, "Expect ')' after loop clauses.");

    Statement::ptr body = statement();

//    if (increment != nullptr) {
//        Statement::ptr inc = std::make_shared<Expression>(increment);
//        std::vector<Statement::ptr> statements {body, inc};
//        body = std::make_shared<Block>(statements);
//    }
//
//    if (condition == nullptr) {
//        condition = std::make_shared<Literal>(true);
//    }
//    body = std::make_shared<WhileStmt>(condition, body);
//
//    if (initializer != nullptr) {
//        std::vector<Statement::ptr> statements {initializer, body};
//        body = std::make_shared<Block>(statements);
//    }

    return std::make_shared<ForStmt>(initializer, condition, increment, body);
}

}