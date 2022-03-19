//
// Created by 79042 on 11.03.2022.
//

#ifndef MAIN_CPP_SATT_H
#define MAIN_CPP_SATT_H

#include "vector"
#include <iostream>
using std::vector;

class satellite {
protected:
    vector<double> gen_coord;
    double mass;
    unsigned int id;
public:
    satellite();
    satellite(unsigned int id_, double m, vector<double> set_q);
    ~satellite() = default;
    [[nodiscard]] vector<double> get_genCoord() const;
    [[nodiscard]] vector<double> get_Coord() const;
    [[nodiscard]] vector<double> get_Vel() const;
    [[nodiscard]] double get_Mass() const;
    [[nodiscard]] vector<double> change_genCoord(vector<double> new_genCoords);
};


#endif //MAIN_CPP_SATT_H
