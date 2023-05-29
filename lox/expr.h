//
// Created by wy on 22.5.23.
//

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "lox/callable.h"
#include "lox/token.h"
#include "lox/value.h"

namespace stmt {

class Function;

}

namespace expr {

class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;
class Logical;
class Break;
class Call;

/*
 * unary ->  ( "!" | "-") unary | call ;
 * call  ->  primary ( "(" arguments? ")" )*
 * arguments -> expression ( "," expression )*
 */

class Visitor {
 public:
    virtual Value visit_binary_expr(Binary *expr) = 0;

    virtual Value visit_grouping_expr(Grouping *expr) = 0;

    virtual Value visit_literal_expr(Literal *expr) = 0;

    virtual Value visit_unary_expr(Unary *expr) = 0;

    virtual Value visit_variable_expr(Variable *expr) = 0;

    virtual Value visit_assign_expr(Assign *expr) = 0;

    virtual Value visit_logical_expr(Logical *expr) = 0;

    virtual Value visit_break_expr(Break *expr) = 0;

    virtual Value visit_call_expr(Call *expr) = 0;

    virtual ~Visitor() = default;
};

class Expr {
 public:
    using ptr = std::shared_ptr<Expr>;

    virtual Value accept(Visitor *visitor) = 0;
};

class Binary : public Expr {
 public:
    Binary(Expr::ptr left, Token::ptr op, Expr::ptr right) {
        this->left = std::move(left);
        this->op = std::move(op);
        this->right = std::move(right);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_binary_expr(this);
    }

    Expr::ptr left;
    Token::ptr op;
    Expr::ptr right;
};

class Grouping : public Expr {
 public:
    explicit Grouping(Expr::ptr expression) {
        this->expression = std::move(expression);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_grouping_expr(this);
    }

    Expr::ptr expression;
};

class Literal : public Expr {
 public:
    explicit Literal(Value value) {
        this->value = std::move(value);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_literal_expr(this);
    }

    Value value;
};

class Unary : public Expr {
 public:
    Unary(Token::ptr op, Expr::ptr right) {
        this->op = std::move(op);
        this->right = std::move(right);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_unary_expr(this);
    }

    Token::ptr op;
    Expr::ptr right;
};

class Variable : public Expr, public std::enable_shared_from_this<Variable> {
 public:
    explicit Variable(Token::ptr name) {
        this->name = std::move(name);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_variable_expr(this);
    }

    Token::ptr name;
};

class Assign : public Expr, public std::enable_shared_from_this<Assign> {
 public:
    Assign(Token::ptr name, Expr::ptr value) {
        this->name = std::move(name);
        this->value = std::move(value);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_assign_expr(this);
    }

    Token::ptr name;
    Expr::ptr value;
};

class Logical : public Expr {
 public:
    Logical(Expr::ptr left, Token::ptr op, Expr::ptr right) {
        this->left = std::move(left);
        this->op = std::move(op);
        this->right = std::move(right);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_logical_expr(this);
    }

    Expr::ptr left;
    Token::ptr op;
    Expr::ptr right;
};

class Break : public Expr {
 public:
    explicit Break(Token::ptr keyword) {
        this->keyword = std::move(keyword);
    }
    Value accept(Visitor *visitor) override {
        return visitor->visit_break_expr(this);
    }
    Token::ptr keyword;
};

class Call : public Expr {
 public:
    Call(Expr::ptr callee, Token::ptr paren, std::vector<Expr::ptr> arguments) {
        this->callee = std::move(callee);
        this->paren = std::move(paren);
        this->arguments = std::move(arguments);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_call_expr(this);
    }

    Expr::ptr callee;
    Token::ptr paren;
    std::vector<Expr::ptr> arguments;
};

}  // namespace expr
