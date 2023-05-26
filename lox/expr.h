//
// Created by wy on 22.5.23.
//

#pragma once

#include <memory>
#include <utility>

#include "lox/token.h"
#include "lox/value.h"


namespace lox {

struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Logical;
struct Break;

class ExprVisitor {
public:

    virtual lox::Value visitBinaryExpr(Binary *expr) = 0;

    virtual lox::Value visitGroupingExpr(Grouping *expr) = 0;

    virtual lox::Value visitLiteralExpr(Literal *expr) = 0;

    virtual lox::Value visitUnaryExpr(Unary *expr) = 0;

    virtual lox::Value visitVariableExpr(Variable *expr) = 0;

    virtual lox::Value visitAssignExpr(Assign *expr) = 0;

    virtual lox::Value visitLogicalExpr(Logical *expr) = 0;

    virtual lox::Value visitBreakExpr(Break *expr) = 0;

    virtual ~ExprVisitor() = default;
};


class Expr {
public:
    using ptr = std::shared_ptr<Expr>;

    virtual lox::Value accept(ExprVisitor *visitor) = 0;
};

struct Binary : public Expr {
    Binary(Expr::ptr left, Token::ptr op, Expr::ptr right) {
        this->left = std::move(left);
        this->op = std::move(op);
        this->right = std::move(right);
    }

    lox::Value accept(ExprVisitor *visitor) override {
        return visitor->visitBinaryExpr(this);
    }

    Expr::ptr left;
    Token::ptr op;
    Expr::ptr right;
};


struct Grouping : public Expr {
    explicit Grouping(Expr::ptr expression) {
        this->expression = std::move(expression);
    }

    lox::Value accept(ExprVisitor *visitor) override {
        return visitor->visitGroupingExpr(this);
    }

    Expr::ptr expression;
};


struct Literal : public Expr {
    explicit Literal(lox::Value value) : value(std::move(value)) {}

    lox::Value accept(ExprVisitor *visitor) override {
        return visitor->visitLiteralExpr(this);
    }

    lox::Value value;
};


struct Unary : public Expr {
    Unary(Token::ptr op, Expr::ptr right) {
        this->op = std::move(op);
        this->right = std::move(right);
    }

    lox::Value accept(ExprVisitor *visitor) override {
        return visitor->visitUnaryExpr(this);
    }

    Token::ptr op;
    Expr::ptr right;
};

struct Variable : public Expr {
    explicit Variable(Token::ptr name): name(std::move(name)) {}

    lox::Value accept(lox::ExprVisitor *visitor) override {
        return visitor->visitVariableExpr(this);
    }

    Token::ptr name;
};

struct Assign : public Expr {
    explicit Assign(Token::ptr name, Expr::ptr value): name(std::move(name)), value(std::move(value)) {}

    lox::Value accept(lox::ExprVisitor *visitor) override {
        return visitor->visitAssignExpr(this);
    }

    Token::ptr name;
    Expr::ptr value;
};

struct Logical : public Expr {
    explicit Logical(Expr::ptr left, Token::ptr op, Expr::ptr right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    lox::Value accept(lox::ExprVisitor *visitor) override {
        return visitor->visitLogicalExpr(this);
    }

    Token::ptr op;
    Expr::ptr left;
    Expr::ptr right;
};

struct Break : public Expr {
    lox::Value accept(lox::ExprVisitor *visitor) override {
        return visitor->visitBreakExpr(this);
    }
};

} //namespace lox

