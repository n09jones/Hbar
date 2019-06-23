#include "ATRAP_base.h"
#include "ATRAP_EM.h"

using calc_t = long double;

template <cg_nms::grid_type Grid_Type>
using fields_t = cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true>;

template <cg_nms::grid_type Grid_Type>
void run_biot_savart_calc_spec_grid(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    fields_t<Grid_Type> fld {jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx};
    bs_nms::biot_savart<calc_t, global_nms::default_data_type, Grid_Type>(fld);
    fld.print_contents();
}

void run_biot_savart_calc(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    if(cg_nms::gives_cart_grid(jm_nms::get_opts_fn(argc, argv))) {
        run_biot_savart_calc_spec_grid<cg_nms::grid_type::cart>(argc, argv, tot_parts, part_idx);
    }
    else {
        run_biot_savart_calc_spec_grid<cg_nms::grid_type::file>(argc, argv, tot_parts, part_idx);
    }
}

int main(int argc, char ** argv)
{
    return jm_nms::pseudo_main(argc, argv, run_biot_savart_calc, fields_t<cg_nms::grid_type::cart>::output_files, fields_t<cg_nms::grid_type::cart>::write_opt_file_template, fields_t<cg_nms::grid_type::cart>::print_out_mem);
}
