#include <iostream>
#include "lox/lox.h"

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "Usage: cx [script]" << std::endl;
        exit(64);
    }

    lox::Lox app;
    if (argc == 2) {
        app.execute_script(argv[1]);
    } else {
        app.prompt();
    }
    exit(0);
}
