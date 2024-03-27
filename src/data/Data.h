#ifndef DA2324_PRJ1_G163_DATA_H
#define DA2324_PRJ1_G163_DATA_H

#include <cstdint>
#include <string>
#include <variant>
#include <optional>
#include "../../lib/Graph.h"
#include "../CSV.h"
#include "Info.h"


class Data {
private:
    Graph<Info> g;
public:
    Data(Csv cities, Csv pipes, Csv reservoirs, Csv stations);

    // TODO
};


#endif //DA2324_PRJ1_G163_DATA_H
