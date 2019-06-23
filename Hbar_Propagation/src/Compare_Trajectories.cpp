#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_sy1_t = kin_hand_nms::kinematic_handler<true, kin_hand_nms::simple_integrator, 0, EM_Quant_Gen>;
using kh_rk5_t = kin_hand_nms::kinematic_handler<false, boost::numeric::odeint::runge_kutta_cash_karp54<pdef_nms::state_t, global_nms::default_data_type, pdef_nms::state_t, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;
using kh_rk8_t = kin_hand_nms::kinematic_handler<false, boost::numeric::odeint::runge_kutta_fehlberg78<pdef_nms::state_t, global_nms::default_data_type, pdef_nms::state_t, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;
using kh_sy4_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

template <class KH_T> std::string kh_str;
template <> std::string kh_str<kh_sy1_t> {"sy1"};
template <> std::string kh_str<kh_rk5_t> {"rk5"};
template <> std::string kh_str<kh_rk8_t> {"rk8"};
template <> std::string kh_str<kh_sy4_t> {"sy4"};

const size_t IC_idx {0};

template <ig_nms::interp_quant IQ, class KH_T>
void run_sim(KH_T& kh, KH_T& kh_2, size_t ns_per_step)
{
    kh.dt = ns_per_step*(1.e-9);
    kh_2.dt = kh.dt/2;
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    
    kh.reset_out_of_bnds();
    kh_2.reset_out_of_bnds();
    kh.final_states.clear();
    
    st_sum_nms::state_summary tmp_smry;
    global_nms::default_data_type max_diff_sq;
    
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()));)
    {
        double tmp_lim {std::min(kh.t_lim, kh.t + 0.001)};
        max_diff_sq = 0.0;
        for(; ((kh.t < tmp_lim) && !(kh.out_of_bounds())); kh.t += kh.dt) {
            kh_2.pos_vel = kh.pos_vel;
            kh.template do_step<IQ>();
            kh_2.template do_step<IQ>();
            kh_2.template do_step<IQ>();
            max_diff_sq = std::max(max_diff_sq, norm_sq(kh_2.pos_vel.first - kh.pos_vel.first));
        }
        
        tmp_smry.pos_vel = kh.pos_vel;
        tmp_smry.pos_vel.second[0] = max_diff_sq;
        tmp_smry.E = kh.eh.get_gr_E(kh.pos_vel);
        kh.final_states.push_back(tmp_smry);
    }
    
    auto tmp_fn {kh.out_fn};
    
    auto dir {tmp_fn.parent_path()};
    std::string fn {tmp_fn.stem().c_str()};
    auto ext {tmp_fn.extension()};
    
    fn += "_" + kh_str<KH_T> + "_" + std::to_string(ns_per_step) + "_ns";
    
    kh.out_fn = (dir / boost::filesystem::path{fn}).replace_extension(ext);
    kh.print_contents();
    
    kh.out_fn = tmp_fn;
}

template <class KH_T>
void pick_and_run_sum(KH_T& kh, KH_T& kh_2, size_t ns_per_step)
{
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sc>(kh, kh_2, ns_per_step);
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sd>(kh, kh_2, ns_per_step);
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_sy1_t kh_sy1;
    kh_sy4_t kh_sy4;
    kh_rk5_t kh_rk5;
    kh_rk8_t kh_rk8;
    
    kh_sy1_t kh_sy1_h;
    kh_sy4_t kh_sy4_h;
    kh_rk5_t kh_rk5_h;
    kh_rk8_t kh_rk8_h;
    
    kh_sy1.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh_sy4.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh_rk5.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh_rk8.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    kh_sy1_h.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh_sy4_h.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh_rk5_h.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh_rk8_h.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    
    std::vector<size_t> ns_per_step_vec {10, 100, 1000, 10000};
    
    for(auto ns_per_step : ns_per_step_vec)
    {
        std::cout << "Starting ns_per_step = " << ns_per_step << std::endl;
        
        pick_and_run_sum(kh_sy1, kh_sy1_h, ns_per_step);
        pick_and_run_sum(kh_sy4, kh_sy4_h, ns_per_step);
        pick_and_run_sum(kh_rk5, kh_rk5_h, ns_per_step);
        pick_and_run_sum(kh_rk8, kh_rk8_h, ns_per_step);
        
        std::cout << "Completed ns_per_step = " << ns_per_step << std::endl << std::endl;
    }
}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_sy1_t::output_files, kh_sy1_t::write_opt_file_template, kh_sy1_t::print_out_mem);
    
    return 0;
}



