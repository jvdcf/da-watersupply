/**
 * @file main.cpp
 * @brief The entry point of the program.
 * @details Checks the validity of the arguments and starts the program.
 */

#include "lib/Graph.h"
#include "src/CSV.h"
#include "src/Parser.h"
#include <fstream>
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
  std::cout << "ERROR: main.cpp not implemented yet\n";
  
  std::cout << "DEBUG: TEST CSV\n";
  std::cout << "Test 1\n";
  // auto p1 = verifies(isdigit).take_while();
  // auto p2 = p1.next("9703927");
  // if (!p2.has_value()) std::cout << "ERROR\n";
  // auto [rest,v] = p2.value();
  // std::cout << "rest: " << rest << " v: " << std::string(v.begin(),v.end()) << std::endl; 
  //std::ifstream file("dataset/DataSetSmall/Pipes_Madeira.csv");
  std::ifstream file("test.csv");
  std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  auto p1 = parse_csv().next(fileContent);
  if (!p1.has_value()) return -1;
  auto [rest, v] = p1.value();
  std::cout << "rest: " << rest << " v: " << v.display() << std::endl;
  std::string te = "Bea,1,1.0\n";
  auto p2 = parse_line().next(te);
  if (!p2.has_value()) return -1;
  auto [rest1, v1] = p2.value();
  std::cout << "rest: " << rest1 << " v: " << v1.display() << std::endl;

  

  

  return 1;
}
