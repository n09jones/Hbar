#include "ATRAP_base.h"
#include "ATRAP_EM.h"

template <cg_nms::grid_type Grid_Type>
using der_B_t = cgq_nms::derived_B<par_arr_nms::storage::mem, Grid_Type, true>;

template <cg_nms::grid_type Grid_Type>
using der_Bnorm_t = cgq_nms::derived_Bnorm<par_arr_nms::storage::mem, Grid_Type, true>;

template <cg_nms::grid_type Grid_Type>
using der_acc_1Sc_t = cgq_nms::derived_acc_1Sc<par_arr_nms::storage::mem, Grid_Type, true>;

template <cg_nms::grid_type Grid_Type>
using der_acc_1Sd_t = cgq_nms::derived_acc_1Sd<par_arr_nms::storage::mem, Grid_Type, true>;

template <cg_nms::grid_type Grid_Type>
using der_acc_2Sc_t = cgq_nms::derived_acc_2Sc<par_arr_nms::storage::mem, Grid_Type, true>;

template <cg_nms::grid_type Grid_Type>
using der_acc_2Sd_t = cgq_nms::derived_acc_2Sd<par_arr_nms::storage::mem, Grid_Type, true>;

template <cg_nms::grid_type Grid_Type>
using sum_fields_t = cgq_nms::sum_coil<par_arr_nms::storage::mem, Grid_Type>;

bool sup_B;
bool sup_Bnorm;
bool sup_acc;

const std::vector<pow_nms::mob_desc> suppressed_calculations
{
    {"sup_B", [](){return (boost::program_options::bool_switch(&sup_B)->default_value(false));}, "Prevent the calculation of B"},
    {"sup_Bnorm", [](){return (boost::program_options::bool_switch(&sup_Bnorm)->default_value(false));}, "Prevent the calculation of B's norm"},
    {"sup_acc", [](){return (boost::program_options::bool_switch(&sup_acc)->default_value(false));}, "Prevent the calculation of the accelerations"}
};


std::vector<std::pair<boost::filesystem::path, bool>> fn_pair_vec_func(const boost::filesystem::path& opts_fn)
{
    std::vector<std::pair<boost::filesystem::path, bool>> ret_vec;
    
    if(!sup_B)
    {
        auto tmp {der_B_t<cg_nms::grid_type::cart>::output_files(opts_fn)};
        ret_vec.insert(ret_vec.end(), tmp.begin(), tmp.end());
    }
    
    if(!sup_Bnorm)
    {
        auto tmp {der_Bnorm_t<cg_nms::grid_type::cart>::output_files(opts_fn)};
        ret_vec.insert(ret_vec.end(), tmp.begin(), tmp.end());
    }
    
    if(!sup_acc)
    {
        auto tmp {der_acc_1Sc_t<cg_nms::grid_type::cart>::output_files(opts_fn)};
        ret_vec.insert(ret_vec.end(), tmp.begin(), tmp.end());
        
        tmp = der_acc_1Sd_t<cg_nms::grid_type::cart>::output_files(opts_fn);
        ret_vec.insert(ret_vec.end(), tmp.begin(), tmp.end());
        
        tmp = der_acc_2Sc_t<cg_nms::grid_type::cart>::output_files(opts_fn);
        ret_vec.insert(ret_vec.end(), tmp.begin(), tmp.end());
        
        tmp = der_acc_2Sd_t<cg_nms::grid_type::cart>::output_files(opts_fn);
        ret_vec.insert(ret_vec.end(), tmp.begin(), tmp.end());
    }
    
    return ret_vec;
}


void print_out_mem_func(const boost::filesystem::path& opts_fn)
{
    if(!sup_B) {
        std::cout << "For B:" << std::endl;
        der_B_t<cg_nms::grid_type::cart>::print_out_mem(opts_fn);
        std::cout << std::endl;
    }
    
    if(!sup_Bnorm) {
        std::cout << "For B's norm:" << std::endl;
        der_Bnorm_t<cg_nms::grid_type::cart>::print_out_mem(opts_fn);
        std::cout << std::endl;
    }
    
    if(!sup_acc) {
        std::cout << "For each of four acceleration files:" << std::endl;
        der_acc_1Sc_t<cg_nms::grid_type::cart>::print_out_mem(opts_fn);
        std::cout << std::endl;
    }
}


void print_suppressed_calculations_job_opts()
{
    boost::program_options::options_description desc {"Suppress Some Calculations"};
    pow_nms::append_desc_vec(desc, suppressed_calculations);
    std::cout << desc << std::endl;
}

bool is_der_B_cartesian(int argc, char ** argv)
{
    der_B_t<cg_nms::grid_type::cart> tmp_der_B;
    tmp_der_B.fill_in_params(jm_nms::get_opts_fn(argc, argv));
    
    return cg_nms::gives_cart_grid(tmp_der_B.in_params.coord_grid_src());
}

template <class U, class V>
void acc_spec_grid(int argc, char ** argv, size_t tot_parts, size_t part_idx, V& flds, global_nms::default_data_type HFS, global_nms::default_data_type eff_B_mom, const std::string& str)
{
    U arr {jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx};
    
    global_nms::default_data_type eff_B_mom_sq = eff_B_mom*eff_B_mom;
    auto den {sqrt((HFS*HFS/4) + (eff_B_mom_sq*norm_sq(flds.data["B"])))};
    for(auto& elem : den) {
        if(!elem) {elem = 1;}
    }
    
    global_nms::default_data_type pre_factor {(-1.0)*eff_B_mom_sq/(pvsi_nms::mass_e + pvsi_nms::mass_p)};
    
    auto str_vec {gd_nms::base_strs(gd_nms::quant::B, true)};
    for(unsigned idx = 0; idx < 3; ++idx) {
        arr.data[str][idx] = std::move(pre_factor*inner_pr(flds.data["B"], flds.data[str_vec[idx+1]])/den);
    }
    
    static bool run_before {false};
    arr.print_contents(!sup_B || !sup_Bnorm || run_before);
    run_before = true;
}

template <cg_nms::grid_type Grid_Type>
void derive_quantities_spec_grid(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    sum_fields_t<Grid_Type> flds;
    {
        der_B_t<Grid_Type> tmp_der_B;
        tmp_der_B.fill_in_params(jm_nms::get_opts_fn(argc, argv));
        flds.fill_contents(tmp_der_B.in_params.coord_grid_src(), tot_parts, part_idx);
    }
    
    if(!sup_B)
    {
        der_B_t<Grid_Type> der_B {jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx};
        der_B.data["B"] = flds.data["B"];
        der_B.print_contents();
    }
    
    if(!sup_Bnorm)
    {
        der_Bnorm_t<Grid_Type> der_Bnorm {jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx};
        der_Bnorm.data["Bnorm"] = norm(flds.data["B"]);
        der_Bnorm.print_contents(!sup_B);
    }
    
    if(!sup_acc)
    {
        using namespace pvsi_nms;
        
        global_nms::default_data_type mu_pos_1S {g_positron(1)*mu_B/hbar};
        global_nms::default_data_type mu_pos_2S {g_positron(2)*mu_B/hbar};
        global_nms::default_data_type mu_pbar_1S {g_pbar(1)*mu_N/hbar};
        global_nms::default_data_type mu_pbar_2S {g_pbar(2)*mu_N/hbar};
        
        acc_spec_grid<der_acc_1Sc_t<Grid_Type>>(argc, argv, tot_parts, part_idx, flds, S_HFS(1), (hbar/2)*(mu_pos_1S - mu_pbar_1S), "acc_1Sc");
        acc_spec_grid<der_acc_1Sd_t<Grid_Type>>(argc, argv, tot_parts, part_idx, flds, 0, (hbar/2)*(mu_pos_1S + mu_pbar_1S), "acc_1Sd");
        acc_spec_grid<der_acc_2Sc_t<Grid_Type>>(argc, argv, tot_parts, part_idx, flds, S_HFS(2), (hbar/2)*(mu_pos_2S - mu_pbar_2S), "acc_2Sc");
        acc_spec_grid<der_acc_2Sd_t<Grid_Type>>(argc, argv, tot_parts, part_idx, flds, 0, (hbar/2)*(mu_pos_2S + mu_pbar_2S), "acc_2Sd");
    }
}


void derive_quantities(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    if(is_der_B_cartesian(argc, argv)) {
        derive_quantities_spec_grid<cg_nms::grid_type::cart>(argc, argv, tot_parts, part_idx);
    }
    else {
        derive_quantities_spec_grid<cg_nms::grid_type::file>(argc, argv, tot_parts, part_idx);
    }
}


int main(int argc, char ** argv)
{
    pow_nms::get_cmd_opt(argc, argv, suppressed_calculations);
    
    if(sup_B && sup_Bnorm && sup_acc) {
        throw fp_nms::fp_exc{"At least one calculation must be done"};
    }
    
    return jm_nms::pseudo_main(argc, argv, derive_quantities, fn_pair_vec_func, der_B_t<cg_nms::grid_type::cart>::write_opt_file_template, print_out_mem_func, print_suppressed_calculations_job_opts);
}
