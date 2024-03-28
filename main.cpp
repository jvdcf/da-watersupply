/**
 * @file main.cpp
 * @brief The entry point of the program.
 * @details Checks the validity of the arguments and starts the program.
 */

#include <iostream>
#include <fstream>
#include <filesystem>

#include "lib/Graph.h"
#include "lib/MutablePriorityQueue.h"
#include "lib/UFDS.h"
#include "src/data/Data.h"
#include "src/data/Info.h"
#include "src/CSV.h"
#include "src/Parser.h"
#include "src/Runtime.h"
#include "src/Utils.h"


void printError() {
  // TODO
  std::cerr
      << "USAGE: DA2324_PRJ1_G163 <path>\n"
      << "       being <path> the folder in which the following csv files are located:\n"
      << "        - Cities.csv\n"
      << "        - Pipes.csv\n"
      << "        - Reservoir.csv\n"
      << "        - Stations.csv\n"
      << "See the Doxygen documentation for more information."
      << std::endl;
  std::exit(1);
}

std::vector<std::string> getCSVPaths(std::string path) {
  std::vector<std::string> expectedFiles = {"Cities", "Pipes", "Reservoir", "Stations"};
  std::vector<std::string> paths = {"", "", "", ""};  // {cityPath, pipePath, reservoirPath, StationPath}

  for (const auto& file : std::filesystem::directory_iterator(path)) {
    std::string filePath = file.path().string();
    for (int i = 0; i < 4; ++i) {
      if (filePath.find(expectedFiles[i]) != std::string::npos && filePath.find(".csv") != std::string::npos) {
        if (paths[i] != "") {
          std::cerr << "ERROR: Found multiple " << expectedFiles[i] << " files\n";
          printError();
        }
        paths[i] = filePath;
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    if (paths[i].empty()) {
      std::cerr << "ERROR: Missing " << expectedFiles[i] << " file\n";
      printError();
    }
  }
  return paths;
}

std::vector<Csv> parseCSVs(std::vector<std::string> paths) {
  std::vector<Csv> csv;
  for (const std::string& path: paths) {
    std::ifstream file(path);
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto p = parse_csv().next(fileContent);
    if (!p.has_value()) {
      std::cerr << "ERROR: Failed to parse the csv file " << path << '\n';
      printError();
    }
    auto [rest, v] = p.value();
    csv.push_back(v);
  }
  return csv;
}

int main(int argc, char **argv) {
  if (argc != 2) printError();
  if (!std::filesystem::is_directory(argv[1])) {
    std::cerr << "ERROR: The path provided is not a directory\n";
    printError();
  }

  std::vector<std::string> paths = getCSVPaths(argv[1]);
  std::vector<Csv> csv = parseCSVs(paths);

  Data d(csv[0], csv[1], csv[2], csv[3]);
  Runtime rt(&d);
  rt.run();
}
