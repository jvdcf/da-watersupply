/**
 * @file main.cpp
 * @brief The entry point of the program.
 * @details Checks the validity of the arguments and starts the program.
 */

#include "lib/Graph.h"
#include "src/Runtime.h"
#include <iostream>

void printError() {
    // TODO
    std::cerr
            << "USAGE: TODO\n\n"
            << std::endl;
    std::exit(1);
}

bool isFlag(const std::string &arg) {
    // TODO
    return false;
}

int main(int argc, char **argv) {
    // TODO
    std::cerr << "ERROR: main.cpp not implemented yet\n";

    Data d; // TODO: Connect parser to data
    Runtime rt(&d);
    rt.run();
}
