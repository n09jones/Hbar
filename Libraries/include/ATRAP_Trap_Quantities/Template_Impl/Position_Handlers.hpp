#ifndef POSITION_HANDLERS_HPP
#define POSITION_HANDLERS_HPP

#include <iostream>

template <ig_nms::interp_type IT>
pos_nms::grid_position<IT>::grid_position(const cg_nms::cart_coord_grid& coord_grid_in)
{
    init_grid_position(coord_grid_in);
}

template <ig_nms::interp_type IT>
void pos_nms::grid_position<IT>::init_grid_position(const cg_nms::cart_coord_grid& coord_grid_in)
{
for(int i = 0; i < 3; ++i) {std::cout << coord_grid_in.up_lims[i] << " ";}
std::cout << std::endl;
for(int i = 0; i < 3; ++i) {std::cout << coord_grid_in.lo_lims[i] << " ";}
std::cout << std::endl;
for(int i = 0; i < 3; ++i) {std::cout << coord_grid_in.dr[i] << " ";}
std::cout << std::endl;

    pos_coord_grid = coord_grid_in;
    
    lo_lims = bvec_nms::basic_vec<>{1, 1, 1};
    up_lims = bvec_nms::basic_vec<>{0, 0, 0};
    
    auto tmp_up_lims {pos_coord_grid.up_lims - (3*pos_coord_grid.dr)};
    auto tmp_lo_lims {pos_coord_grid.lo_lims + (3*pos_coord_grid.dr)};
    auto tmp_max_rad {pos_coord_grid.max_rad - (3*std::max({pos_coord_grid.dr[0], pos_coord_grid.dr[1], pos_coord_grid.dr[2]}))};
    
    for(unsigned comp = 0; comp < 3; ++comp) {
        if((tmp_lo_lims[comp] >= tmp_up_lims[comp]) || (tmp_max_rad <= 0)) {
            throw cg_nms::cg_exc{"Coordinate grid has too few valid points"};
        }
    }
    
    for(unsigned comp = 0; comp < 3; ++comp) {
        if((tmp_lo_lims[comp] >= 0) || (tmp_up_lims[comp] <= 0)) {
            throw cg_nms::cg_exc{"Limits should straddle the origin"};
        }
    }
    
    if(false && (std::min({std::abs(tmp_lo_lims[0]), std::abs(tmp_lo_lims[1]), std::abs(tmp_up_lims[0]), std::abs(tmp_up_lims[1]), tmp_max_rad}) < trap_radius)) {
	std::vector<double> tmp_vec {std::abs(tmp_lo_lims[0]), std::abs(tmp_lo_lims[1]), std::abs(tmp_up_lims[0]), std::abs(tmp_up_lims[1]), tmp_max_rad, trap_radius};
	for(auto elem : tmp_vec) {std::cout << elem << " ";}
	std::cout << std::endl;
        throw cg_nms::cg_exc{"Interpolation grid is too small"};
    }
    
    abs_z_lim = std::min(std::abs(tmp_up_lims[2]), std::abs(tmp_lo_lims[2]));
}

#endif /* POSITION_HANDLERS_HPP */

