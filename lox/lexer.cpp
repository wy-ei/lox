//
// Created by wy on 17.5.23.
//

#include "lox/lexer.h"

#include <unordered_map>
#include <memory>
#include <utility>

#include "lox/lox.h"

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

bool Lexer::is_at_end() {
    return current_ >= source_.size();
}

std::vector<Token::ptr> Lexer::scan() {
    start_ = 0;
    current_ = 0;
    line_ = 1;
    col_ = 0;

    while (!is_at_end()) {
        start_ = current_;
        scan_next();
    }
    add_token(Token::END, "<EOF>");
    return std::move(tokens_);
}

void Lexer::scan_next() {
    char ch = consume();
    switch (ch) {
    case '(':
        add_token(Token::LEFT_PAREN);
        break;
    case ')':
        add_token(Token::RIGHT_PAREN);
        break;
    case '{':
        add_token(Token::LEFT_BRACE);
            break;
    case '}':
        add_token(Token::RIGHT_BRACE);
            break;
    case ',':
        add_token(Token::COMMA);
            break;
    case '.':
        add_token(Token::DOT);
            break;
    case '-':
        add_token(Token::MINUS);
            break;
    case '+':
        add_token(Token::PLUS);
            break;
    case ';':
        add_token(Token::SEMICOLON);
            break;
    case '*':
        add_token(Token::STAR);
            break;
    case '#':
        add_token(Token::NUMBER_SIGN);
            break;
    case ':':
        add_token(Token::COLON);
            break;
    case '!':
        if (check('=')) {
            add_token(Token::BANG_EQUAL);
        } else {
            add_token(Token::BANG);
        }
            break;
    case '=':
        if (check('=')) {
            add_token(Token::EQUAL_EQUAL);
        } else {
            add_token(Token::EQUAL);
        }
        break;
    case '<':
        if (check('=')) {
            add_token(Token::LESS_EQUAL);
        } else {
            add_token(Token::LESS);
        }
            break;
    case '>':
        if (check('=')) {
            add_token(Token::GREATER_EQUAL);
        } else {
            add_token(Token::GREATER);
        }
            break;
    case '/':
        if (check('/')) {
            while (consume() != '\n') {
            }
        } else {
            add_token(Token::SLASH);
        }
        break;
    case ' ':
    case '\r':
    case '\t':
    case '\n':
        break;
    case '"':
        string();
            break;
    default:
        if (std::isdigit(ch)) {
            number();
        } else if (std::isalnum(ch) || ch == '_') {
            identifier();
        } else {
            std::string text {ch};
            add_token(Token::UNEXPECTED, "<UNEXPECTED>");
            throw RuntimeError(tokens_.back(), "unknown character: `" + text + "`");
        }
    }
}

char Lexer::consume() {
    char ch = source_[current_++];
    if (ch == '\n') {
        line_++;
    }
    return ch;
}

bool Lexer::check(char ch) {
    if (source_[current_] == ch) {
        consume();
        return true;
    } else {
        return false;
    }
}

char Lexer::lookahead(int i) {
    return source_[current_ + i];
}

void Lexer::string() {
    while (!is_at_end() && consume() != '"') {
    }
    if (is_at_end()) {
        add_token(Token::UNEXPECTED, "<UNEXPECTED>");
        throw RuntimeError(tokens_.back(), "unexpected end of file");
    }

    auto str = source_.substr(start_ + 1, current_ - start_ - 2);
    add_token(Token::STRING, std::move(str));
}

void Lexer::number() {
    while (!is_at_end() && std::isdigit(lookahead(0))) {
        consume();
    }
    if (lookahead(0) == '.' && std::isdigit(lookahead(1))) {
        consume();

        while (std::isdigit(lookahead(0))) {
            consume();
        }
    }
    add_token(Token::NUMBER, source_.substr(start_, current_ - start_));
}

static std::unordered_map<std::string, Token::Kind> keywords = {
    {"and", Token::AND},
    {"class", Token::CLASS},
    {"else", Token::ELSE},
    {"false", Token::FALSE},
    {"for", Token::FOR},
    {"fun", Token::FUN},
    {"if", Token::IF},
    {"nil", Token::NIL},
    {"or", Token::OR},
    {"print", Token::PRINT},
    {"return", Token::RETURN},
    {"super", Token::SUPER},
    {"this", Token::THIS},
    {"true", Token::TRUE},
    {"var", Token::VAR},
    {"while", Token::WHILE},
    {"break", Token::BREAK},
};

void Lexer::identifier() {
    char ch = lookahead(0);
    while (isalnum(ch) || ch == '_') {
        consume();
        ch = lookahead(0);
    }
    std::string text = source_.substr(start_, current_ - start_);
    if (keywords.count(text) != 0) {
        add_token(keywords[text], text);
    } else {
        add_token(Token::IDENTIFIER, text);
    }
}

void Lexer::add_token(Token::Kind kind) {
    std::string text = source_.substr(start_, current_ - start_);
    add_token(kind, text);
}

void Lexer::add_token(Token::Kind kind, std::string lexeme) {
    auto token = std::make_shared<Token>(kind, std::move(lexeme), line_);
    tokens_.push_back(token);
}
