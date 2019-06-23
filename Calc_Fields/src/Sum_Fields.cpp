#include "ATRAP_base.h"
#include "ATRAP_EM.h"

template <cg_nms::grid_type Grid_Type>
using part_fields_t = cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type>;

template <cg_nms::grid_type Grid_Type>
using sum_fields_t = cgq_nms::sum_coil<par_arr_nms::storage::mem, Grid_Type, true>;


bool is_sum_grid_cartesian(int argc, char ** argv)
{
    sum_fields_t<cg_nms::grid_type::cart> tmp_sum;
    tmp_sum.fill_in_params(jm_nms::get_opts_fn(argc, argv));
    
    return cg_nms::gives_cart_grid(tmp_sum.in_params.coord_grid_src());
}


template <cg_nms::grid_type Grid_Type>
void sum_fields_spec_grid(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    sum_fields_t<Grid_Type> sum {jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx};
    
    sum.data["B"] += (sum.in_params.offset_field)*mem_arr_vec_nms::mem_arr_vec<>{sum.coord_grid.in_bounds_mask(tot_parts, part_idx)};
    
    for(const auto& p: sum.in_params.coil_info)
    {
        part_fields_t<Grid_Type> part_fld {p.second.first, tot_parts, part_idx};
        
        if((part_fld.in_params.coil_nm != p.first) || (sum.coord_grid != part_fld.coord_grid)) {
            throw fp_nms::fp_exc{"Mismatch between part field and sum field"};
        }
        
        for(const std::string& str : gd_nms::base_strs(gd_nms::quant::B, sum.in_params.grads)) {
            sum.data[str] += part_fld.data[str]*p.second.second;
        }
    }
    
    sum.print_contents();
}


void sum_fields(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    if(is_sum_grid_cartesian(argc, argv)) {
        sum_fields_spec_grid<cg_nms::grid_type::cart>(argc, argv, tot_parts, part_idx);
    }
    else {
        sum_fields_spec_grid<cg_nms::grid_type::file>(argc, argv, tot_parts, part_idx);
    }
}


int main(int argc, char ** argv)
{
    return jm_nms::pseudo_main(argc, argv, sum_fields, sum_fields_t<cg_nms::grid_type::cart>::output_files, sum_fields_t<cg_nms::grid_type::cart>::write_opt_file_template, sum_fields_t<cg_nms::grid_type::cart>::print_out_mem);
}
