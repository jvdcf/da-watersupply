#ifndef DA2324_PRJ1_G163_RUNTIME_H
#define DA2324_PRJ1_G163_RUNTIME_H

#include <string>
#include <vector>
#include "data/Data.h"


class Runtime {
private:
  Data* data;
  void processArgs(const std::vector<std::string>& arg);
  /* Add here the commands */

public:
  explicit Runtime(Data* d);
  [[noreturn]] void run();
};


#endif //DA2324_PRJ1_G163_RUNTIME_H
