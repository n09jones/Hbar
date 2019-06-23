#include <algorithm>
#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "ATRAP_Trap_Quantities.h"

bool output_file_already_printed {false};


std::vector<mapped_arr_nms::mapped_arr<>> interp_sources(const boost::filesystem::path& input_dir, ig_nms::interp_quant iq)
{
    std::vector<mapped_arr_nms::mapped_arr<>> ret_vec;
    
    for(auto fn : ig_nms::interp_source_files(input_dir, iq)) {
        ret_vec.push_back({fn, false});
    }
    
    return ret_vec;
}

template <ig_nms::interp_quant IQ>
bool interped_quant(const boost::filesystem::path& opts_fn)
{
    ig_nms::interp_grid<IQ, par_arr_nms::storage::mem> tmp_ig;
    tmp_ig.fill_in_params(opts_fn);
    
    auto fns {ig_nms::interp_source_files(tmp_ig.in_params.input_dir, IQ)};
    return std::all_of(fns.begin(), fns.end(), [](boost::filesystem::path fn) {return is_regular_file(fn);});
}

template <ig_nms::interp_quant IQ>
void add_to_fn_pair_vec(const boost::filesystem::path& opts_fn, std::vector<std::pair<boost::filesystem::path, bool>>& fn_pair_vec)
{
    if(!interped_quant<IQ>(opts_fn)) {return;}
    
    for(auto p : ig_nms::interp_grid<IQ, par_arr_nms::storage::mem>::output_files(opts_fn)) {
        fn_pair_vec.push_back(p);
    }
}

std::vector<std::pair<boost::filesystem::path, bool>> fn_pair_vec_func(const boost::filesystem::path& opts_fn)
{
    std::vector<std::pair<boost::filesystem::path, bool>> ret_vec;
    
    add_to_fn_pair_vec<ig_nms::interp_quant::BdB>(opts_fn, ret_vec);
    add_to_fn_pair_vec<ig_nms::interp_quant::B>(opts_fn, ret_vec);
    add_to_fn_pair_vec<ig_nms::interp_quant::Bnorm>(opts_fn, ret_vec);
    add_to_fn_pair_vec<ig_nms::interp_quant::acc_1Sc>(opts_fn, ret_vec);
    add_to_fn_pair_vec<ig_nms::interp_quant::acc_1Sd>(opts_fn, ret_vec);
    add_to_fn_pair_vec<ig_nms::interp_quant::acc_2Sc>(opts_fn, ret_vec);
    add_to_fn_pair_vec<ig_nms::interp_quant::acc_2Sd>(opts_fn, ret_vec);
    
    return ret_vec;
}

template <ig_nms::interp_quant IQ>
void print_single_mem(const boost::filesystem::path& opts_fn)
{
    if(!interped_quant<IQ>(opts_fn)) {return;}
    
    std::map<ig_nms::interp_quant, std::string> nm_map {{ig_nms::interp_quant::BdB, "BdB"}, {ig_nms::interp_quant::B, "B"}, {ig_nms::interp_quant::Bnorm, "Bnorm"}, {ig_nms::interp_quant::acc_1Sc, "acc_1Sc"}, {ig_nms::interp_quant::acc_1Sd, "acc_1Sd"}, {ig_nms::interp_quant::acc_2Sc, "acc_2Sc"}, {ig_nms::interp_quant::acc_2Sd, "acc_2Sd"}};
    
    std::cout << "For " << nm_map.at(IQ) << ": " << std::endl;
    ig_nms::interp_grid<IQ, par_arr_nms::storage::mem>::print_out_mem(opts_fn);
    std::cout << std::endl;
}

void print_out_mem_func(const boost::filesystem::path& opts_fn)
{
    print_single_mem<ig_nms::interp_quant::BdB>(opts_fn);
    print_single_mem<ig_nms::interp_quant::B>(opts_fn);
    print_single_mem<ig_nms::interp_quant::Bnorm>(opts_fn);
    print_single_mem<ig_nms::interp_quant::acc_1Sc>(opts_fn);
    print_single_mem<ig_nms::interp_quant::acc_1Sd>(opts_fn);
    print_single_mem<ig_nms::interp_quant::acc_2Sc>(opts_fn);
    print_single_mem<ig_nms::interp_quant::acc_2Sd>(opts_fn);
}

template <ig_nms::interp_type IT>
std::vector<global_nms::default_data_type> get_base_pt_coeffs(cg_nms::cart_coord_grid cg, const mapped_arr_nms::mapped_arr<>& arr, id_nms::grid_offset_t base_pt)
{
    std::vector<global_nms::default_data_type> ret_vec (ig_nms::enum_val(IT));
    
    id_nms::monomial_handler<IT> base_pt_monomials {};
    id_nms::monomial_handler<IT> obsv_pt_monomials {};
    
    obsv_pt_monomials.set_monomials(cg.inv_dr);

    size_t grid_idx_0 {inner_pr(bvec_nms::convert<size_t>(base_pt), cg.stride)};
    base_pt_monomials.set_monomials(cg.coords(grid_idx_0)*cg.inv_dr);

    for (auto offset_map : id_nms::get_coeff_map_wrapper<IT>())
    {
        id_nms::grid_offset_t grid_pt {base_pt + offset_map.first};
        size_t grid_idx {inner_pr(bvec_nms::convert<size_t>(grid_pt), cg.stride)};
        
        for(unsigned comp = 0; comp < 3; ++comp) {
            if((grid_pt[comp] < 0) || (static_cast<size_t>(grid_pt[comp]) >= cg.pts[comp]) || cg.out_of_bds(grid_idx)) {
                return std::vector<global_nms::default_data_type>(ig_nms::enum_val(IT));
            }
        }
        
        size_t xyz_idx {0};
        for(auto xyz_powers : id_nms::index_to_xyz_powers_wrapper<IT>())
        {
            size_t x0y0z0_idx {0};
            for (auto x0y0z0_powers : id_nms::index_to_xyz_powers_wrapper<IT>())
            {
                ret_vec[xyz_idx] += offset_map.second[{{xyz_powers, x0y0z0_powers}}]*arr[grid_idx]*base_pt_monomials.monomials[x0y0z0_idx]*obsv_pt_monomials.monomials[xyz_idx];
                ++x0y0z0_idx;
            }
            ++xyz_idx;
        }
    }
    
    return ret_vec;
}

template <ig_nms::interp_quant IQ>
void calculate_quant_coefficients(const boost::filesystem::path& opts_fn, size_t tot_parts = 1, size_t part_idx = 0)
{
std::cout << "Starting " << static_cast<int>(IQ) << std::endl;
    if(!interped_quant<IQ>(opts_fn))
    {
std::cout << "Ending " << static_cast<int>(IQ) << std::endl;
	return;
    }
    
    ig_nms::interp_grid<IQ, par_arr_nms::storage::mem, false, true> ig {opts_fn, tot_parts, part_idx};
    std::vector<mapped_arr_nms::mapped_arr<>> src_list {interp_sources(ig.in_params.input_dir, IQ)};
    
    std::map<ig_nms::interp_type, std::vector<global_nms::default_data_type>(*)(cg_nms::cart_coord_grid, const mapped_arr_nms::mapped_arr<>&, id_nms::grid_offset_t)> fn_ptr_map
    {
        {ig_nms::interp_type::zeroth, get_base_pt_coeffs<ig_nms::interp_type::zeroth>},
        {ig_nms::interp_type::lin, get_base_pt_coeffs<ig_nms::interp_type::lin>},
        {ig_nms::interp_type::quad, get_base_pt_coeffs<ig_nms::interp_type::quad>},
        {ig_nms::interp_type::trilin, get_base_pt_coeffs<ig_nms::interp_type::trilin>}
    };
    
    auto base_coeff_fn {fn_ptr_map.at(ig.in_params.it)};
    size_t arr_pt {0};
    
    size_t grid_idx_0 {ig.data.part.get_first_elem_idx() / ig.data.part.get_chunk_size()};
    size_t grid_idx_len {ig.data.part.get_elems_in_part() / ig.data.part.get_chunk_size()};
    
    for(size_t grid_idx = grid_idx_0; grid_idx < grid_idx_0 + grid_idx_len; ++grid_idx)
    {
if(grid_idx % 100000 == 0) {
std::cout << grid_idx << " of " << grid_idx_0 + grid_idx_len - 1 << std::endl;
}
        bvec_nms::basic_vec<size_t> base_pt {grid_idx % ig.coord_grid.stride[1], grid_idx % ig.coord_grid.stride[2], grid_idx};
        base_pt /= ig.coord_grid.stride;
        
        for(const auto& arr : src_list) {
            for(auto coeff : base_coeff_fn(ig.coord_grid, arr, bvec_nms::convert<id_nms::grid_offset_fund_t>(base_pt))) {
                ig.data.arr[arr_pt++] = coeff;
            }
        }
    }
    
    ig.print_contents(output_file_already_printed);
    output_file_already_printed = true;

std::cout << "Ending " << static_cast<int>(IQ) << std::endl;
}

void calculate_all_coefficients(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    auto opts_fn {jm_nms::get_opts_fn(argc, argv)};
    
    calculate_quant_coefficients<ig_nms::interp_quant::BdB>(opts_fn, tot_parts, part_idx);
    calculate_quant_coefficients<ig_nms::interp_quant::B>(opts_fn, tot_parts, part_idx);
    calculate_quant_coefficients<ig_nms::interp_quant::Bnorm>(opts_fn, tot_parts, part_idx);
    calculate_quant_coefficients<ig_nms::interp_quant::acc_1Sc>(opts_fn, tot_parts, part_idx);
    calculate_quant_coefficients<ig_nms::interp_quant::acc_1Sd>(opts_fn, tot_parts, part_idx);
    calculate_quant_coefficients<ig_nms::interp_quant::acc_2Sc>(opts_fn, tot_parts, part_idx);
    calculate_quant_coefficients<ig_nms::interp_quant::acc_2Sd>(opts_fn, tot_parts, part_idx);
}

int main(int argc, char ** argv)
{
try{
    return jm_nms::pseudo_main(argc, argv, calculate_all_coefficients, fn_pair_vec_func, ig_nms::interp_grid<ig_nms::interp_quant::B, par_arr_nms::storage::mem>::write_opt_file_template, print_out_mem_func);
}
catch(gqt_nms::gqt_exc& exc) {std::cout << exc.give_err() << std::endl;}
}



















