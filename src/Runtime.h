#ifndef DA2324_PRJ1_G163_RUNTIME_H
#define DA2324_PRJ1_G163_RUNTIME_H

#include <string>
#include <vector>
#include "data/Data.h"

/**
 * @brief Interface and input handling.
 * @details This class is responsible for reading the user input and calling the appropriate functions.
 */

class Runtime {
private:
  /// Pointer to a Data object. Used to call their methods.
  Data* data;

  /**
   * @brief From a list of arguments, process them and call the appropriate function.
   * @param arg: A vector of strings with the arguments. The first string is the command.
   */
  void processArgs(const std::vector<std::string>& arg);

public:
  /**
   * @brief Constructor
   * @param d: Pointer to a Data object.
   */
  explicit Runtime(Data* d);

  /**
   * @brief Main loop.
   * @details Reads the user input, filters it and calls Runtime::processArgs().
   */
  [[noreturn]] void run();
};


#endif //DA2324_PRJ1_G163_RUNTIME_H
