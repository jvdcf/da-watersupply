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

    void setCities(Csv cities);
    void setPipes(Csv pipes);
    void setReservoirs(Csv reservoirs);
    void setStations(Csv stations);

public:
    Data(Csv cities, Csv pipes, Csv reservoirs, Csv stations);
    std::array<int, 3> countVertexes();
};


#endif //DA2324_PRJ1_G163_DATA_H
