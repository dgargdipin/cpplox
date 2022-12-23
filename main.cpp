#include <sysexits.h>
#include <iostream>
#include "lox.h"

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [script]\n";
        exit(EX_USAGE);
    } else if (argc == 2) {
        Lox::runFile(argv[1]);
    } else {
        Lox::runPrompt();
    }
}
