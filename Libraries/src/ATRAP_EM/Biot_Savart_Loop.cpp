#include <random>
#include "Biot_Savart/Biot_Savart_Loop.h"

std::vector<global_nms::default_data_type> bsl_nms::gen_mirror_coil_params(global_nms::default_data_type min, global_nms::default_data_type max, size_t turns)
{
if(true)
{
    std::minstd_rand gen {};
    std::normal_distribution<> nd{0.0, 5.0e-6};

    min += nd(gen);
    max += nd(gen);
}

    std::vector<global_nms::default_data_type> ret_vec;
    
    if(min < max) {
        if(turns < 2) {throw bsl_exc{"Must have at least two points"};}
        global_nms::default_data_type stride {(max - min)/(turns - 1)};
        for(size_t idx = 0; idx < turns; ++idx) {
            ret_vec.push_back(min + (stride*idx));
        }
    }
    else if(min == max) {
        if(turns != 1) {throw bsl_exc{"Only one turn allowed"};}
        ret_vec.push_back(min);
    }
    else {
        throw bsl_exc{"Min cannot exceed max"};
    }
    
    return ret_vec;
}
