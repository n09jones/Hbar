#ifndef INTERPOLATION_DATA_H
#define INTERPOLATION_DATA_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "Interpolation_Grids.h"

namespace id_nms
{
    /* Convenient type definitions */
    using power_t = bvec_nms::basic_vec<unsigned>;
    using grid_offset_fund_t = ptrdiff_t;
    using grid_offset_t = bvec_nms::basic_vec<grid_offset_fund_t>;
    using xyz_to_coeff_map_t = std::map<power_t, global_nms::default_data_type>;
    using xyz_x0y0z0_to_coeff_map_t = std::map<std::array<power_t, 2>, global_nms::default_data_type>;
    using grid_offset_to_xyz_to_coeff_map_t = std::map<grid_offset_t, xyz_to_coeff_map_t>;
    using grid_offset_to_xyz_x0y0z0_to_coeff_map_t = std::map<grid_offset_t, xyz_x0y0z0_to_coeff_map_t>;
    
    /* Set monomials from position vectors. */
    template <ig_nms::interp_type IT>
    struct monomial_handler;
    
    /* Get lists of monomial powers in the same order as the monomial vectors in the first arguments of
     set_monomials_*(_, _). */
    template <ig_nms::interp_type IT>
    std::vector<power_t> index_to_xyz_powers();
    template <ig_nms::interp_type IT>
    std::vector<power_t> index_to_xyz_powers_wrapper();
    
    /* Get a map from grid point offsets to secondary maps. These secondary maps map pairs consisting of
     interpolation point powers and base point powers to corresponding coefficients. */
    template <ig_nms::interp_type IT>
    grid_offset_to_xyz_x0y0z0_to_coeff_map_t get_coeff_map();
    template <ig_nms::interp_type IT>
    grid_offset_to_xyz_x0y0z0_to_coeff_map_t get_coeff_map_wrapper();
}

#include "Template_Impl/Interpolation_Data.hpp"

#endif /* INTERPOLATION_DATA_H */

