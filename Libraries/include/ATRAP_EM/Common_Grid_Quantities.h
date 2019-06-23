#ifndef COMMON_GRID_QUANTITIES_H
#define COMMON_GRID_QUANTITIES_H

#include "ATRAP_base.h"
#include "Grid_Quantity_Template.h"
#include "Field_Params.h"
#include "Coord_Grid.h"
#include "Grid_Data.h"

namespace cgq_nms
{
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using single_coil = gqt_nms::grid_quantity_template<fp_nms::single_coil_in_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::B, T>, fp_nms::single_coil_out_params, Constructing>;
    
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using sum_coil = gqt_nms::grid_quantity_template<fp_nms::sum_coil_in_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::B, T>, fp_nms::sum_coil_out_params, Constructing>;
    
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using derived_B = gqt_nms::grid_quantity_template<fp_nms::sum_coil_out_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::B, T>, fp_nms::derived_quantity_out_params, Constructing>;
    
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using derived_Bnorm = gqt_nms::grid_quantity_template<fp_nms::sum_coil_out_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::Bnorm, T>, fp_nms::derived_quantity_out_params, Constructing>;
    
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using derived_acc_1Sc = gqt_nms::grid_quantity_template<fp_nms::sum_coil_out_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::acc_1Sc, T>, fp_nms::derived_quantity_out_params, Constructing>;
    
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using derived_acc_1Sd = gqt_nms::grid_quantity_template<fp_nms::sum_coil_out_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::acc_1Sd, T>, fp_nms::derived_quantity_out_params, Constructing>;
    
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using derived_acc_2Sc = gqt_nms::grid_quantity_template<fp_nms::sum_coil_out_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::acc_2Sc, T>, fp_nms::derived_quantity_out_params, Constructing>;
    
    template <par_arr_nms::storage STORAGE, cg_nms::grid_type Grid_Type = cg_nms::grid_type::cart, bool Constructing = false, class T = global_nms::default_data_type>
    using derived_acc_2Sd = gqt_nms::grid_quantity_template<fp_nms::sum_coil_out_params, cg_nms::coord_grid<Grid_Type>, gd_nms::grid_data<STORAGE, gd_nms::quant::acc_2Sd, T>, fp_nms::derived_quantity_out_params, Constructing>;
}


#endif /* COMMON_GRID_QUANTITIES_H */
