#include <cmath>
#include "Interpolation_Data.h"

namespace
{
    /* Normalized grid generators */
    
    id_nms::xyz_x0y0z0_to_coeff_map_t unnorm_coeff_map_elem(id_nms::xyz_x0y0z0_to_coeff_map_t in_map, id_nms::power_t decomposed_key)
    {
        if(decomposed_key == id_nms::power_t{0, 0, 0}) {return in_map;}
        
        id_nms::power_t key_add_on {0, 0, 0};
        for(unsigned comp = 0; comp < 3; ++comp) {
            if(decomposed_key[comp]) {
                --decomposed_key[comp];
                key_add_on[comp] = 1;
                break;
            }
        }
        
        id_nms::xyz_x0y0z0_to_coeff_map_t out_map;
        for(auto key_val : in_map)
        {
            out_map[{{key_val.first[0] + key_add_on, key_val.first[1]}}] += key_val.second;
            out_map[{{key_val.first[0], key_val.first[1] + key_add_on}}] += -key_val.second;
        }
        
        return unnorm_coeff_map_elem(out_map, decomposed_key);
    }
    
    id_nms::xyz_x0y0z0_to_coeff_map_t unnorm_coeff_map(id_nms::xyz_to_coeff_map_t in_map)
    {
        id_nms::xyz_x0y0z0_to_coeff_map_t out_map;
        
        for(auto key_val : in_map) {
            for(auto key_val_out : unnorm_coeff_map_elem({{{{{0, 0, 0}, {0, 0, 0}}}, key_val.second}}, key_val.first)) {
                out_map[key_val_out.first] += key_val_out.second;
            }
        }
        
        return out_map;
    }
    
    id_nms::grid_offset_to_xyz_x0y0z0_to_coeff_map_t unnorm_grid_coeff_map(id_nms::grid_offset_to_xyz_to_coeff_map_t in_map)
    {
        id_nms::grid_offset_to_xyz_x0y0z0_to_coeff_map_t out_map;
        
        for(auto key_val : in_map)
        {
            auto tmp_map {unnorm_coeff_map(key_val.second)};
            for(auto tmp_key_val : tmp_map) {
                out_map[key_val.first][tmp_key_val.first] += tmp_key_val.second;
            }
        }
        
        return out_map;
    }
}

/*
 Polynomials are ordered by
 - Lowest order
 - Highest order x
 - Highest order y
 */

template <ig_nms::interp_type IT>
std::vector<id_nms::power_t> id_nms::index_to_xyz_powers()
{
    std::vector<power_t> ret_vec;
    bvec_nms::basic_vec<> prime_vec {2, 3, 5};
    
    monomial_handler<IT> test_monomials {};
    test_monomials.set_monomials(prime_vec);
    
    for(auto elem : test_monomials.monomials)
    {
        power_t powers {0, 0, 0};
        
        for(unsigned comp = 0; comp < 3; ++comp) {
            for(auto q = elem/prime_vec[comp]; std::abs(q - std::round(q)) < 0.001; ++(powers[comp]), q /= prime_vec[comp]);
        }
        
        ret_vec.push_back(powers);
    }
    
    return ret_vec;
}

template <>
id_nms::grid_offset_to_xyz_x0y0z0_to_coeff_map_t id_nms::get_coeff_map<ig_nms::interp_type::zeroth>()
{
    id_nms::grid_offset_to_xyz_to_coeff_map_t ret_map
    {
        {{0, 0, 0}, {{{0, 0, 0}, 1.0}}},
        {{1, 0, 0}, {{{0, 0, 0}, 1.0}}},
        {{0, 1, 0}, {{{0, 0, 0}, 1.0}}},
        {{0, 0, 1}, {{{0, 0, 0}, 1.0}}},
        {{1, 0, 1}, {{{0, 0, 0}, 1.0}}},
        {{0, 1, 1}, {{{0, 0, 0}, 1.0}}},
        {{1, 1, 0}, {{{0, 0, 0}, 1.0}}},
        {{1, 1, 1}, {{{0, 0, 0}, 1.0}}}
    };
    
    for(auto& x : ret_map) {
        for(auto& y : x.second) {
            y.second /= 8.0;
        }
    }
    
    return unnorm_grid_coeff_map(ret_map);
}

template <>
id_nms::grid_offset_to_xyz_x0y0z0_to_coeff_map_t id_nms::get_coeff_map<ig_nms::interp_type::lin>()
{
    id_nms::grid_offset_to_xyz_to_coeff_map_t ret_map
    {
        {{0, 0, 0},
            {{{0, 0, 0}, 4.0}, {{1, 0, 0}, -2.0}, {{0, 1, 0}, -2.0}, {{0, 0, 1}, -2.0}}},
        {{1, 0, 0},
            {{{0, 0, 0}, 2.0}, {{1, 0, 0}, 2.0}, {{0, 1, 0}, -2.0}, {{0, 0, 1}, -2.0}}},
        {{0, 1, 0},
            {{{0, 0, 0}, 2.0}, {{1, 0, 0}, -2.0}, {{0, 1, 0}, 2.0}, {{0, 0, 1}, -2.0}}},
        {{0, 0, 1},
            {{{0, 0, 0}, 2.0}, {{1, 0, 0}, -2.0}, {{0, 1, 0}, -2.0}, {{0, 0, 1}, 2.0}}},
        {{1, 0, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 2.0}, {{0, 1, 0}, -2.0}, {{0, 0, 1}, 2.0}}},
        {{0, 1, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -2.0}, {{0, 1, 0}, 2.0}, {{0, 0, 1}, 2.0}}},
        {{1, 1, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 2.0}, {{0, 1, 0}, 2.0}, {{0, 0, 1}, -2.0}}},
        {{1, 1, 1},
            {{{0, 0, 0}, -2.0}, {{1, 0, 0}, 2.0}, {{0, 1, 0}, 2.0}, {{0, 0, 1}, 2.0}}}
    };
    
    for(auto& x : ret_map) {
        for(auto& y : x.second) {
            y.second /= 8.0;
        }
    }
    
    return unnorm_grid_coeff_map(ret_map);
}

template <>
id_nms::grid_offset_to_xyz_x0y0z0_to_coeff_map_t id_nms::get_coeff_map<ig_nms::interp_type::quad>()
{
    id_nms::grid_offset_to_xyz_to_coeff_map_t ret_map
    {
        {{0, 0, 0},
            {{{0, 0, 0}, 14.0}, {{1, 0, 0}, -11.0}, {{0, 1, 0}, -11.0}, {{0, 0, 1}, -11.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, 8.0}, {{1, 0, 1}, 8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, 8.0}, {{0, 0, 2}, -1.0}}},
        {{1, 0, 0},
            {{{0, 0, 0}, 2.0}, {{1, 0, 0}, 13.0}, {{0, 1, 0}, -3.0}, {{0, 0, 1}, -3.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, -8.0}, {{1, 0, 1}, -8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, 8.0}, {{0, 0, 2}, -1.0}}},
        {{2, 0, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{0, 1, 0},
            {{{0, 0, 0}, 2.0}, {{1, 0, 0}, -3.0}, {{0, 1, 0}, 13.0}, {{0, 0, 1}, -3.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, -8.0}, {{1, 0, 1}, 8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, -8.0}, {{0, 0, 2}, -1.0}}},
        {{1, 1, 0},
            {{{0, 0, 0}, -2.0}, {{1, 0, 0}, 5.0}, {{0, 1, 0}, 5.0}, {{0, 0, 1}, 5.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, 8.0}, {{1, 0, 1}, -8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, -8.0}, {{0, 0, 2}, -1.0}}},
        {{0, 2, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{0, 0, 1},
            {{{0, 0, 0}, 2.0}, {{1, 0, 0}, -3.0}, {{0, 1, 0}, -3.0}, {{0, 0, 1}, 13.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, 8.0}, {{1, 0, 1}, -8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, -8.0}, {{0, 0, 2}, -1.0}}},
        {{1, 0, 1},
            {{{0, 0, 0}, -2.0}, {{1, 0, 0}, 5.0}, {{0, 1, 0}, 5.0}, {{0, 0, 1}, 5.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, -8.0}, {{1, 0, 1}, 8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, -8.0}, {{0, 0, 2}, -1.0}}},
        {{0, 1, 1},
            {{{0, 0, 0}, -2.0}, {{1, 0, 0}, 5.0}, {{0, 1, 0}, 5.0}, {{0, 0, 1}, 5.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, -8.0}, {{1, 0, 1}, -8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, 8.0}, {{0, 0, 2}, -1.0}}},
        {{0, 0, 2},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}},
        {{2, 0, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, 1, 1},
            {{{0, 0, 0}, 2.0}, {{1, 0, 0}, -3.0}, {{0, 1, 0}, -3.0}, {{0, 0, 1}, -3.0}, {{2, 0, 0}, -1.0}, {{1, 1, 0}, 8.0}, {{1, 0, 1}, 8.0}, {{0, 2, 0}, -1.0}, {{0, 1, 1}, 8.0}, {{0, 0, 2}, -1.0}}},
        {{0, 2, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{0, 0, -1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}},
        {{2, 1, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{0, -1, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{0, 1, 2},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}},
        {{2, 1, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{0, -1, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{0, 1, -1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}},
        {{-1, 0, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, 2, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, 0, 2},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}},
        {{-1, 0, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, 2, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, 0, -1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}},
        {{-1, 1, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, -1, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, 1, 2},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}},
        {{-1, 1, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, -1, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, 0.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 1.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 0.0}}},
        {{1, 1, -1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, -1.0}, {{2, 0, 0}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 2, 0}, 0.0}, {{0, 1, 1}, 0.0}, {{0, 0, 2}, 1.0}}}
    };
    
    for(auto& x : ret_map) {
        for(auto& y : x.second) {
            y.second /= 16.0;
        }
    }
    
    return unnorm_grid_coeff_map(ret_map);
}

/*
 @ 0 -> 1 - x
 @ 1 -> x
 */
template <>
id_nms::grid_offset_to_xyz_x0y0z0_to_coeff_map_t id_nms::get_coeff_map<ig_nms::interp_type::trilin>()
{
    id_nms::grid_offset_to_xyz_to_coeff_map_t ret_map
    {
        {{0, 0, 0},
            {{{0, 0, 0}, 1.0}, {{1, 0, 0}, -1.0}, {{0, 1, 0}, -1.0}, {{0, 0, 1}, -1.0}, {{1, 1, 0}, 1.0}, {{1, 0, 1}, 1.0}, {{0, 1, 1}, 1.0}, {{1, 1, 1}, -1.0}}},
        {{1, 0, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 1.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{1, 1, 0}, -1.0}, {{1, 0, 1}, -1.0}, {{0, 1, 1}, 0.0}, {{1, 1, 1}, 1.0}}},
        {{0, 1, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 1.0}, {{0, 0, 1}, 0.0}, {{1, 1, 0}, -1.0}, {{1, 0, 1}, 0.0}, {{0, 1, 1}, -1.0}, {{1, 1, 1}, 1.0}}},
        {{0, 0, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 1.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, -1.0}, {{0, 1, 1}, -1.0}, {{1, 1, 1}, 1.0}}},
        {{1, 1, 0},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{1, 1, 0}, 1.0}, {{1, 0, 1}, 0.0}, {{0, 1, 1}, 0.0}, {{1, 1, 1}, -1.0}}},
        {{1, 0, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 1.0}, {{0, 1, 1}, 0.0}, {{1, 1, 1}, -1.0}}},
        {{0, 1, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 1, 1}, 1.0}, {{1, 1, 1}, -1.0}}},
        {{1, 1, 1},
            {{{0, 0, 0}, 0.0}, {{1, 0, 0}, 0.0}, {{0, 1, 0}, 0.0}, {{0, 0, 1}, 0.0}, {{1, 1, 0}, 0.0}, {{1, 0, 1}, 0.0}, {{0, 1, 1}, 0.0}, {{1, 1, 1}, 1.0}}}
    };
    
    return unnorm_grid_coeff_map(ret_map);
}

template std::vector<id_nms::power_t> id_nms::index_to_xyz_powers<ig_nms::interp_type::zeroth>();
template std::vector<id_nms::power_t> id_nms::index_to_xyz_powers<ig_nms::interp_type::lin>();
template std::vector<id_nms::power_t> id_nms::index_to_xyz_powers<ig_nms::interp_type::quad>();
template std::vector<id_nms::power_t> id_nms::index_to_xyz_powers<ig_nms::interp_type::trilin>();

