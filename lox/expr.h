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
class Get;
class Set;
class This;
class Super;

/*
 * unary ->  ( "!" | "-") unary | call ;
 * call  ->  primary ( "(" arguments? ")" )*
 * arguments -> expression ( "," expression )*
 * assignment     → ( call "." )? IDENTIFIER "=" assignment
               | logic_or ;
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
    virtual Value visit_get_expr(Get *expr) = 0;
    virtual Value visit_set_expr(Set *expr) = 0;
    virtual Value visit_this_expr(This *expr) = 0;
    virtual Value visit_super_expr(Super *expr) = 0;
    virtual ~Visitor() = default;
};

class Expr {
 public:
    using ptr = std::shared_ptr<Expr>;

    virtual Value accept(Visitor *visitor) = 0;
};

class Binary : public Expr {
 public:
    using ptr = std::shared_ptr<Binary>;
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
    using ptr = std::shared_ptr<Grouping>;
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
    using ptr = std::shared_ptr<Literal>;
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
    using ptr = std::shared_ptr<Unary>;
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
    using ptr = std::shared_ptr<Variable>;
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
    using ptr = std::shared_ptr<Assign>;
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
    using ptr = std::shared_ptr<Logical>;
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
    using ptr = std::shared_ptr<Break>;
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
    using ptr = std::shared_ptr<Call>;
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

class Get : public Expr {
 public:
    using ptr = std::shared_ptr<Get>;
    Get(Expr::ptr object, Token::ptr name) {
        this->object = std::move(object);
        this->name = std::move(name);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_get_expr(this);
    }

    Expr::ptr object;
    Token::ptr name;
};

class Set : public Expr {
 public:
    using ptr = std::shared_ptr<Set>;
    Set(Expr::ptr object, Token::ptr name, Expr::ptr value) {
        this->object = std::move(object);
        this->name = std::move(name);
        this->value = std::move(value);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_set_expr(this);
    }

    Expr::ptr object;
    Token::ptr name;
    Expr::ptr value;
};

class This : public Expr {
 public:
    using ptr = std::shared_ptr<This>;
    explicit This(Token::ptr name) {
        this->name = std::move(name);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_this_expr(this);
    }

    Token::ptr name;
};

class Super : public Expr {
 public:
    using ptr = std::shared_ptr<Super>;
    explicit Super(Token::ptr keyword, Token::ptr method) {
        this->keyword = std::move(keyword);
        this->method = std::move(method);
    }

    Value accept(Visitor *visitor) override {
        return visitor->visit_super_expr(this);
    }

    Token::ptr keyword;
    Token::ptr method;
};

} // namespace expr
