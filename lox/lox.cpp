//
// Created by wy on 17.5.23.
//

#include "lox/lox.h"

#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include "lox/lexer.h"
#include "lox/token.h"
#include "lox/expr.h"
#include "lox/ast_printer.h"
#include "lox/parser.h"
#include "lox/interpreter.h"

namespace lox {

void Lox::execute_script(const std::string &filepath) {
    std::ifstream file(filepath.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "can't open file '" << filepath << "': No such file or directory" << std::endl;
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    execute(content);
}

void Lox::execute(const std::string &script) {
    Lexer lexer(script);
    std::vector<Token::ptr> tokens;

    try {
        tokens = lexer.scan();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    try {
        Parser parser(tokens);
        std::vector<Statement::ptr> statements  = parser.parse();
        interpreter_.interpret(statements);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}

void Lox::prompt() {
    std::string line;

    interpreter_.enable_repl_mode();

    std::cout << "> ";
    while (getline(std::cin, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        execute(line);
        std::cout << "> ";
    }
}

} //namespace lox
