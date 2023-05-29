//
// Created by wy on 17.5.23.
//

#include "lexer.h"

#include <unordered_map>
#include <utility>

#include "lox/lox.h"

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

bool Lexer::end() {
    return current_ >= source_.size();
}

std::vector<Token::ptr> Lexer::scan() {
    std::vector<Token::ptr> tokens;
    while (true) {
        Token token = next();
        tokens.push_back(std::make_shared<Token>(token));
        if (token.kind == Token::Kind::END) {
            break;
        }
    }
    return tokens;
}

Token Lexer::next() {
    while (true) {
        char ch = lookahead(0);
        start_ = current_;
        switch (ch) {
        case EOF:
            return {Token::Kind::END, "<EOF>"};
        case '(':
            consume();
            return {Token::Kind::LEFT_PAREN, "("};
        case ')':
            consume();
            return {Token::Kind::RIGHT_PAREN, ")"};
        case '{':
            consume();
            return {Token::Kind::LEFT_BRACE, "{"};
        case '}':
            consume();
            return {Token::Kind::RIGHT_BRACE, "}"};
        case ',':
            consume();
            return {Token::Kind::COMMA, ","};
        case '.':
            consume();
            return {Token::Kind::DOT, "."};
        case '-':
            consume();
            return {Token::Kind::MINUS, "-"};
        case '+':
            consume();
            return {Token::Kind::PLUS, "+"};
        case ';':
            consume();
            return {Token::Kind::SEMICOLON, ";"};
        case '*':
            consume();
            return {Token::Kind::STAR, "*"};
        case '#':
            consume();
            return {Token::Kind::NUMBER_SIGN, "#"};
        case ':':
            consume();
            return {Token::Kind::COLON, ":"};
        case '!':
            consume();
            if (lookahead(0) == '=') {
                consume();
                return {Token::Kind::BANG_EQUAL, "!="};
            } else {
                return {Token::Kind::BANG, "!"};
            }
        case '=':
            consume();
            if (lookahead(0) == '=') {
                consume();
                return {Token::Kind::EQUAL_EQUAL, "=="};
            } else {
                return {Token::Kind::EQUAL, "="};
            }
        case '<':
            consume();
            if (lookahead(0) == '=') {
                consume();
                return {Token::Kind::LESS_EQUAL, "<="};
            } else {
                return {Token::Kind::LESS, "<"};
            }
        case '>':
            consume();
            if (lookahead(0) == '=') {
                consume();
                return {Token::Kind::GREATER_EQUAL, ">="};
            } else {
                return {Token::Kind::GREATER, ">"};
            }
        case '/':
            consume();
            if (lookahead(0) == '/') {
                consume();
                while (lookahead(0) != '\n' && !end()) {
                    consume();
                }
                if (lookahead(0) == '\n') {
                    consume();
                }
                break;
            } else {
                return {Token::Kind::SLASH, "/"};
            }
        case ' ':
        case '\r':
        case '\t':
            consume();
            break;
        case '\n':
            consume();
            line_++;
            break;
        case '"':
            return string();
        default:
            if (std::isdigit(ch)) {
                return number();
            } else if (std::isalnum(ch)) {
                return identifier();
            } else {
                consume();
                auto str = source_.substr(start_, current_ - start_);
                return {Token::Kind::UNEXPECTED, "<unexpected>(" + str + ")"};
            }
        }
    }
}

void Lexer::consume() {
    ++current_;
}

char Lexer::lookahead(unsigned int i) {
    if (current_ + i >= source_.size()) {
        return EOF;
    } else {
        return source_[current_ + i];
    }
}

Token Lexer::string() {
    consume();
    while (lookahead(0) != '"' && !end()) {
        if (lookahead(0) == '\n') {
            line_++;
        }
        consume();
    }
    if (end()) {
        return {Token::Kind::UNEXPECTED, "<unexpected>"};
    }
    consume(); // consume '"'

    auto str = source_.substr(start_ + 1, current_ - start_ - 2);
    return {Token::Kind::STRING, std::move(str)};
}

Token Lexer::number() {
    consume();
    while (std::isdigit(lookahead(0))) {
        consume();
    }
    if (lookahead(0) == '.' && isdigit(lookahead(1))) {
        consume();

        while (isdigit(lookahead(0))) {
            consume();
        }
    }
    return {Token::Kind::NUMBER, source_.substr(start_, current_ - start_)};
}

static std::unordered_map<std::string, Token::Kind> keywords = {
    {"and", Token::Kind::AND},
    {"class", Token::Kind::CLASS},
    {"else", Token::Kind::ELSE},
    {"false", Token::Kind::FALSE},
    {"for", Token::Kind::FOR},
    {"fun", Token::Kind::FUN},
    {"if", Token::Kind::IF},
    {"nil", Token::Kind::NIL},
    {"or", Token::Kind::OR},
    {"print", Token::Kind::PRINT},
    {"return", Token::Kind::RETURN},
    {"super", Token::Kind::SUPER},
    {"this", Token::Kind::THIS},
    {"true", Token::Kind::TRUE},
    {"var", Token::Kind::VAR},
    {"while", Token::Kind::WHILE},
    {"break", Token::Kind::BREAK},
};

Token Lexer::identifier() {
    char ch = lookahead(0);
    while (isalnum(ch) || ch == '_') {
        consume();
        ch = lookahead(0);
    }
    std::string text = source_.substr(start_, current_ - start_);
    if (keywords.count(text) != 0) {
        return {keywords[text], text};
    } else {
        return {Token::Kind::IDENTIFIER, text};
    }
}
