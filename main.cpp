#include "lox/lox.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "Usage: cx [script]" << std::endl;
        exit(64);
    }

    Lox lox;
    if (argc == 2) {
        lox.execute_script(argv[1]);
    } else {
        lox.prompt();
    }
    exit(0);
}
