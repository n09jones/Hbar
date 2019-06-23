#include "Coord_Grid.h"
#include "Field_Params.h"
#include "Grid_Data.h"
#include "Grid_Quantity_Template.h"

/* Data */
std::string cg_nms::cart_coord_grid::header()
{
    return std::string{"COORDINATE GRID"};
}

std::vector<pid_nms::param_info> cg_nms::cart_coord_grid::get_param_info()
{
    std::vector<pid_nms::param_info> param_info_vec;
    std::vector<std::string> comp_nm {"x", "y", "z"};
    
    for(unsigned comp = 0; comp < 3; ++comp) {
        param_info_vec.push_back
        ({
            std::string{"Lower limit "} + comp_nm[comp],
            ((!comp) ? "All coordinate limits are in meters." : ""),
            gqt_nms::fnc_true,
            [this, comp](std::string str){gqt_nms::str_to_data(str, &(this->lo_lims[comp]));}
        });
        param_info_vec.push_back
        ({
            std::string{"Upper limit "} + comp_nm[comp],
            "",
            gqt_nms::fnc_true,
            [this, comp](std::string str){gqt_nms::str_to_data(str, &(this->up_lims[comp]));}
        });
        param_info_vec.push_back
        ({
            std::string{"Points along "} + comp_nm[comp],
            "",
            gqt_nms::fnc_true,
            [this, comp](std::string str){gqt_nms::str_to_data(str, &(this->pts[comp]));}
        });
    }
    
    param_info_vec.push_back
    ({
        "Maximum radius",
        "Max radius is in meters.",
        gqt_nms::fnc_true,
        [this](std::string str){gqt_nms::str_to_data(str, &(this->max_rad));}
    });
    
    return param_info_vec;
}

void cg_nms::cart_coord_grid::post_read_ops(boost::program_options::variables_map)
{
    for(unsigned comp = 0; comp < 3; ++comp)
    {
        if(lo_lims[comp] < up_lims[comp]) {
            if(pts[comp] <= 1) {throw cg_exc{"Need more than one point"};}
            dr[comp] = (up_lims[comp] - lo_lims[comp])/(pts[comp] - 1);
            inv_dr[comp] = (pts[comp] - 1)/(up_lims[comp] - lo_lims[comp]);
        }
        else if(lo_lims[comp] == up_lims[comp]) {
            if(pts[comp] != 1) {throw cg_exc{"Need exactly one point"};}
            dr[comp] = 0;
            inv_dr[comp] = 0;
        }
        else {
            throw cg_exc{"Lower limits must be <= upper limits"};
        }
    }
    
    if(max_rad <= 0) {throw cg_exc{"Maximum radius must be greater than zero"};}
    
    stride = bvec_nms::basic_vec<size_t>{1, pts[0], pts[0]*pts[1]};
}

/* Public functions */
mem_arr_nms::mem_arr<> cg_nms::cart_coord_grid::in_bounds_mask(size_t tot_parts, size_t part_idx)
{
    prt_nms::partition part {grid_size(), tot_parts, part_idx};
    mem_arr_nms::mem_arr<> arr {part.get_elems_in_part(), 1};
    
    size_t grid_idx {part.get_first_elem_idx()};
    for(auto& elem : arr) {
        if(out_of_bds(grid_idx++)) {elem = 0;}
    }
    
    return arr;
}


/*** File grid ***/
/* Data */
std::string cg_nms::file_coord_grid::header()
{
    return std::string{"FILE GRID"};
}

std::vector<pid_nms::param_info> cg_nms::file_coord_grid::get_param_info()
{
    std::vector<pid_nms::param_info> param_info_vec;
    param_info_vec.push_back
    ({
        "Coordinate Grid File Name",
        "Relative to \"large_storage_dir\" in Global_Info.h",
        gqt_nms::fnc_true,
        [this](std::string str){(this->grid_fn) = (global_nms::large_storage_dir / boost::filesystem::path{str});}
    });
    
    return param_info_vec;
}

void cg_nms::file_coord_grid::post_read_ops(boost::program_options::variables_map)
{
    coord_arr = mapped_arr_nms::mapped_arr<>(grid_fn, false);
}

/* Public functions */
bvec_nms::basic_vec<> cg_nms::file_coord_grid::coords(size_t grid_idx) const
{
    return bvec_nms::basic_vec<>{coord_arr[(3*grid_idx) + 0], coord_arr[(3*grid_idx) + 1], coord_arr[(3*grid_idx) + 2]};
}

mem_arr_nms::mem_arr<> cg_nms::file_coord_grid::in_bounds_mask(size_t tot_parts, size_t part_idx)
{
    prt_nms::partition part {grid_size(), tot_parts, part_idx};
    mem_arr_nms::mem_arr<> arr {part.get_elems_in_part(), 1};
    
    size_t grid_idx {part.get_first_elem_idx()};
    for(auto& elem : arr) {
        if(out_of_bds(grid_idx++)) {elem = 0;}
    }
    
    return arr;
}


/* Comparison */
bool cg_nms::operator==(const cg_nms::cart_coord_grid& cg_1, const cg_nms::cart_coord_grid& cg_2)
{
    return ((cg_1.lo_lims == cg_2.lo_lims) && (cg_1.up_lims == cg_2.up_lims) &&
            (cg_1.pts == cg_2.pts) && (cg_1.max_rad && cg_2.max_rad));
}

bool cg_nms::operator!=(const cg_nms::cart_coord_grid& cg_1, const cg_nms::cart_coord_grid& cg_2)
{
    return !(cg_1 == cg_2);
}

bool cg_nms::operator==(const cg_nms::file_coord_grid& cg_1, const cg_nms::file_coord_grid& cg_2)
{
    return (cg_1.grid_fn == cg_2.grid_fn);
}

bool cg_nms::operator!=(const cg_nms::file_coord_grid& cg_1, const cg_nms::file_coord_grid& cg_2)
{
    return !(cg_1 == cg_2);
}


/*** Determine source of grid from options file ***/
bool cg_nms::gives_cart_grid(const boost::filesystem::path& opts_fn)
{
    bool is_cart_grid {true};
    bool is_file_grid {true};
    
    try {
        gqt_nms::grid_quantity_template<fp_nms::single_coil_in_params, cart_coord_grid, gd_nms::grid_data<par_arr_nms::storage::mem, gd_nms::quant::B>, fp_nms::single_coil_out_params> cg_test;
        cg_test.fill_coord_grid(opts_fn);
    } catch (...) {
        is_cart_grid = false;
    }
    
    try {
        gqt_nms::grid_quantity_template<fp_nms::single_coil_in_params, file_coord_grid, gd_nms::grid_data<par_arr_nms::storage::mem, gd_nms::quant::B>, fp_nms::single_coil_out_params> fg_test;
        fg_test.fill_coord_grid(opts_fn);
    } catch (...) {
        is_file_grid = false;
    }
    
    if(is_cart_grid == is_file_grid) {throw cg_exc{"Must be either a file grid or a cartesian grid"};}
    
    return is_cart_grid;
}

