/**
 * @file main.cpp
 * @brief The entry point of the program.
 * @details Checks the validity of the arguments and starts the program.
 */

#include "lib/Graph.h"
#include <iostream>

void printError() {
    // TODO
    std::cerr
            << "USAGE: TODO\n\n"
            << "DISCLAIMER: This program is tested for Linux only. As such, it "
               "only takes Linux-compatible CSV files on Linux. It might work on "
               "Windows, but that is untested.\n"
            << std::endl;
    std::exit(1);
}

bool isFlag(const std::string &arg) {
    // TODO
    return false;
}

int main(int argc, char **argv) {
    // TODO
    std::cout << "ERROR: main.cpp not implemented yet";
    return 1;
}
