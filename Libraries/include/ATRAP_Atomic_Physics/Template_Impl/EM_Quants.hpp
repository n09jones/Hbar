#ifndef EM_QUANTS_HPP
#define EM_QUANTS_HPP

#include "Template_Options_File_Printer.h"

/* grid_EM_quants */
template <ig_nms::interp_type IT>
size_t emq_nms::grid_EM_quants<IT>::users {0};

template <ig_nms::interp_type IT>
interp_nms::interpolator<IT> emq_nms::grid_EM_quants<IT>::interp;

template <ig_nms::interp_type IT>
global_nms::default_data_type emq_nms::grid_EM_quants<IT>::Bnorm_min {0.0};

template <ig_nms::interp_type IT>
void emq_nms::grid_EM_quants<IT>::write_opt_file_template(const boost::filesystem::path& opts_fn)
{
    interp_nms::interpolator<IT>::write_opt_file_template(opts_fn);
}

template <ig_nms::interp_type IT>
void emq_nms::grid_EM_quants<IT>::initialize(const boost::filesystem::path& opts_fn)
{
    if(is_user) {return;}
    is_user = true;

    if(users++) {return;}
    interp.fill_contents(opts_fn);
    
    /* Find minimum field */
    auto sub_opts_fn {interp.in_params.interp_quant_src_dirs.at(ig_nms::interp_quant::Bnorm) / global_nms::default_opts_fn};
    ig_nms::interp_grid<ig_nms::interp_quant::Bnorm, par_arr_nms::storage::map> tmp;
    tmp.fill_in_params(sub_opts_fn);
    auto fn {tmp.in_params.input_dir / boost::filesystem::path{"Bnorm.bin"}};
    mapped_arr_nms::mapped_arr<> arr {fn, false};
    auto begin_it {std::find_if(arr.cbegin(), arr.cend(), [](auto x) {return (x > 0.0);})};
    auto it {std::min_element(begin_it, arr.cend(), [](auto x, auto y) {return ((x < y) && (x > 0.0));})};
    Bnorm_min = *it;
}

template <ig_nms::interp_type IT>
void emq_nms::grid_EM_quants<IT>::reset_out_of_bnds()
{
    interp.out_of_bounds = false;
}

template <ig_nms::interp_type IT>
emq_nms::grid_EM_quants<IT>::~grid_EM_quants()
{
    if(!is_user || --users) {return;}
    interp = interp_nms::interpolator<IT>{};
}


/* grid_EM_quants_changing_currents */
template <ig_nms::interp_type IT>
std::string emq_nms::grid_EM_quants_changing_currents<IT>::header()
{
    return std::string{"Interpolation Files"};
}

template <ig_nms::interp_type IT>
std::vector<pid_nms::param_info> emq_nms::grid_EM_quants_changing_currents<IT>::get_param_info()
{
    std::vector<std::string> str_vec {"Quadrupole", "Octupole", "Pinch", "Bucking"};
    
    interp_by_coil.clear();
    curr_by_coil.clear();
    
    std::vector<pid_nms::param_info> param_info_vec;
    
    for(const auto& coil_str : str_vec)
    {
        param_info_vec.push_back
        ({
            coil_str + " Interpolation Options File",
            "",
            gqt_nms::fnc_true,
            [this, coil_str](std::string str){(this->interp_by_coil)[coil_str].fill_contents(str);}
        });
        param_info_vec.push_back
        ({
            coil_str + " Current File",
            "",
            gqt_nms::fnc_true,
            [this, coil_str](std::string str){(this->curr_by_coil)[coil_str] = mem_arr_nms::mem_arr<>{str};}
        });
    }

    return param_info_vec;
}

template <ig_nms::interp_type IT>
global_nms::default_data_type emq_nms::grid_EM_quants_changing_currents<IT>::Bnorm_min {0.0};

template <ig_nms::interp_type IT>
void emq_nms::grid_EM_quants_changing_currents<IT>::write_opt_file_template(const boost::filesystem::path& opts_fn)
{
    grid_EM_quants_changing_currents tmp;
    tofp_nms::app_opt_file(opts_fn, tmp);
}

template <ig_nms::interp_type IT>
void emq_nms::grid_EM_quants_changing_currents<IT>::reset_out_of_bnds()
{
    for(auto& elem : interp_by_coil) {elem.second.out_of_bounds = false;}
}

template <ig_nms::interp_type IT>
template <ig_nms::interp_quant IQ>
inline void emq_nms::grid_EM_quants_changing_currents<IT>::EM_quant(std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)>& output, bvec_nms::basic_vec<> pos, const double t)
{
    output.fill(0.0);

    constexpr double curr_sample_rate {0.0001};
    size_t t_idx {static_cast<size_t>(t/curr_sample_rate)};
    double t_offset {(t - (t_idx*curr_sample_rate))/curr_sample_rate};
    
    std::array<global_nms::default_data_type, 12> BdB_arr {};
    std::array<global_nms::default_data_type, 12> tmp_BdB_arr {};

    BdB_arr[2] = -1.0;

    for(auto& elem : interp_by_coil)
    {
        auto& curr_arr {(this->curr_by_coil)[elem.first]};
        auto curr {((1.0 - t_offset)*curr_arr[t_idx]) + (t_offset*curr_arr[t_idx + 1])};
        elem.second.template calc_trap_quant<ig_nms::interp_quant::BdB, true>(tmp_BdB_arr, pos);
        
        for(size_t idx = 0; idx < tmp_BdB_arr.size(); ++idx) {
            BdB_arr[idx] += curr*tmp_BdB_arr[idx];
        }
    }

    using namespace pvsi_nms;
    
    global_nms::default_data_type mu_pos_1S {g_positron(1)*mu_B/hbar};
    global_nms::default_data_type mu_pos_2S {g_positron(2)*mu_B/hbar};
    global_nms::default_data_type mu_pbar_1S {g_pbar(1)*mu_N/hbar};
    global_nms::default_data_type mu_pbar_2S {g_pbar(2)*mu_N/hbar};
    
    global_nms::default_data_type HFS;
    global_nms::default_data_type eff_B_mom;
    
    switch (IQ)
    {
        case ig_nms::interp_quant::Bnorm:
            for(int comp = 0; comp < 3; ++comp) {output[0] += BdB_arr[comp]*BdB_arr[comp];}
            output[0] = std::sqrt(output[0]);
            break;
            
        case ig_nms::interp_quant::B:
            for(int comp = 0; comp < 3; ++comp) {output[comp] = BdB_arr[comp];}
            break;
        
        case ig_nms::interp_quant::acc_1Sc:
            HFS = S_HFS(1);
            eff_B_mom = (hbar/2)*(mu_pos_1S - mu_pbar_1S);
        case ig_nms::interp_quant::acc_1Sd:
            HFS = 0.0;
            eff_B_mom = (hbar/2)*(mu_pos_1S + mu_pbar_1S);
        case ig_nms::interp_quant::acc_2Sc:
            HFS = S_HFS(2);
            eff_B_mom = (hbar/2)*(mu_pos_2S - mu_pbar_2S);
        case ig_nms::interp_quant::acc_2Sd:
            HFS = 0.0;
            eff_B_mom = (hbar/2)*(mu_pos_2S + mu_pbar_2S);
        {
            global_nms::default_data_type Bnorm_sq {0.0};
            for(int comp = 0; comp < 3; ++comp) {Bnorm_sq += BdB_arr[comp]*BdB_arr[comp];}
            
            auto eff_B_mom_sq = eff_B_mom*eff_B_mom;
            auto den {sqrt((HFS*HFS/4) + (eff_B_mom_sq*Bnorm_sq))};
            auto pre_factor {(-1.0)*eff_B_mom_sq/(mass_e + mass_p)};
            
            for(int d_comp = 0; d_comp < 3; ++d_comp) {
                for(int B_comp = 0; B_comp < 3; ++B_comp) {
                    output[d_comp] += pre_factor*BdB_arr[B_comp]*BdB_arr[B_comp + (3*(1 + d_comp))]/den;
                }
            }
        }
            break;
            
        default:
            throw emq_exc{"Invalid interp quant"};
            break;
    }
}

#endif /* EM_QUANTS_HPP */

