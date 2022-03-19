#include <iostream>
#include "includes.h"
#include "vec_operations.h"



int main() {
    vector<double>  set_start{6800e3, 0, 0, 0, 7771, 0};
    satellite telc_sat(1111, 10, set_start);
    vector<double> start = telc_sat.get_Coord();
    cout<< "Satellite start position is: " "[ " << start << " ]" << std::endl;

    integrator INTEGRATE;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while(true){

        telc_sat.change_genCoord( INTEGRATE.make_iter_RK(telc_sat.get_Mass(), telc_sat.get_genCoord(), 1));
        vector<double> r = telc_sat.get_Coord();
        cout<< "Current satellite coordinates are:" << "[ "<<r<< "] " << std::endl;
        sleep_until(system_clock::now() + seconds(1));

    }
#pragma clang diagnostic pop

    return 0;
}
