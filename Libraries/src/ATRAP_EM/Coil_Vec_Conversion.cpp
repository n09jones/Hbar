#include "Coil_Vec_Conversion.h"

mem_arr_nms::mem_arr<> cvc_nms::vec_to_coil(const mem_arr_vec_nms::mem_arr_vec<>& vec)
{
    if(vec.arr_len() < 2) {throw cvc_exc{"Malformed coil vec"};}
    mem_arr_nms::mem_arr<> coil {3*vec.arr_len()};
    
    for(unsigned comp = 0; comp < 3; ++comp) {
        for(size_t idx = 0; idx < vec.arr_len(); ++idx) {
            coil[(3*idx) + comp] = vec[comp][idx];
        }
    }
    
    return coil;
}

mem_arr_vec_nms::mem_arr_vec<> cvc_nms::coil_to_vec(const mem_arr_nms::mem_arr<>& coil)
{
    if((coil.size() % 3) || (coil.size() < 6)) {throw cvc_exc{"Malformed coil"};}
    mem_arr_vec_nms::mem_arr_vec<> vec {coil.size()/3};
    
    for(unsigned comp = 0; comp < 3; ++comp) {
        for(size_t idx = 0; idx < vec.arr_len(); ++idx) {
            vec[comp][idx] = coil[(3*idx) + comp];
        }
    }
    
    return vec;
}
