#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using EM_Quant_Gen_const_curr = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;
//using kh_const_curr_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen_const_curr>;
using kh_const_curr_t = kin_hand_nms::kinematic_handler<false, boost::numeric::odeint::runge_kutta_cash_karp54<pdef_nms::state_t, global_nms::default_data_type, pdef_nms::state_t, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen_const_curr>;

using EM_Quant_Gen_ch_curr = emq_nms::grid_EM_quants_changing_currents<ig_nms::interp_type::lin>;
using kh_ch_curr_t = kin_hand_nms::kinematic_handler<false,boost::numeric::odeint::runge_kutta_cash_karp54<pdef_nms::state_t, global_nms::default_data_type, pdef_nms::state_t, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen_ch_curr>;

template <class KH_T> std::string kh_str;
template <> std::string kh_str<kh_const_curr_t> {"const"};
template <> std::string kh_str<kh_ch_curr_t> {"changing"};

template <ig_nms::interp_quant IQ, class KH_T, class KH_2_T>
void run_sim(KH_T& kh, KH_2_T& kh_2, size_t IC_idx)
{
    kh.Bnorm_min = 0.0;

    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh_2.pos_vel = kh_2.ICs[IC_idx].pos_vel;
    
    kh.reset_out_of_bnds();
    kh_2.reset_out_of_bnds();
    
    kh.t = 0.0;
    kh_2.t = 0.0;
    global_nms::default_data_type max_diff_sq {0.0};
    
size_t idx {0};
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && !(kh_2.out_of_bounds()));)
    {
        kh.template do_step<IQ>();
        kh_2.template do_step<IQ>();
        max_diff_sq = std::max(max_diff_sq, norm_sq(kh_2.pos_vel.first - kh.pos_vel.first));
	kh.t += kh.dt;
	kh_2.t += kh_2.dt;

if(kh.t >= idx*0.01)
{
idx++;
std::cout << "max_diff" << " : " << std::sqrt(max_diff_sq) << std::endl;
std::cout << "max_2_diff" << " : " << norm(kh_2.pos_vel.first - kh_2.ICs[IC_idx].pos_vel.first) << std::endl;
std::cout << "Energies : " << 1000*kh.eh.get_gr_E(kh.pos_vel) << ", " << 1000*kh_2.eh.get_gr_E(kh_2.pos_vel) << ", diff : " << ((1000*kh_2.eh.get_gr_E(kh_2.pos_vel)) - (1000*kh.eh.get_gr_E(kh.pos_vel))) << std::endl;
}
    }
    
    std::cout << IC_idx << " - max_diff : " << std::sqrt(max_diff_sq) << std::endl;
    std::cout << "time - " << kh.t << std::endl << std::endl;
}

template <class KH_T, class KH_2_T>
void pick_and_run_sum(KH_T& kh, KH_2_T& kh_2, size_t IC_idx)
{
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sc>(kh, kh_2, IC_idx);
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sd>(kh, kh_2, IC_idx);
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_const_curr_t kh_const;
    kh_ch_curr_t kh_ch;
    
    kh_const.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh_ch.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    
    for(size_t IC_idx = 0; IC_idx < kh_const.ICs.size(); ++IC_idx) {
        pick_and_run_sum(kh_const, kh_ch, IC_idx);
    }
}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_const_curr_t::output_files, kh_ch_curr_t::write_opt_file_template, kh_const_curr_t::print_out_mem);
    
    return 0;
}



