#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using EM_Quant_Gen = emq_nms::grid_EM_quants_changing_currents<ig_nms::interp_type::lin>;
using kh_t = kin_hand_nms::kinematic_handler<false,boost::numeric::odeint::runge_kutta_cash_karp54<pdef_nms::state_t, global_nms::default_data_type, pdef_nms::state_t, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

template <ig_nms::interp_quant IQ>
void run_sim(kh_t& kh, size_t IC_idx)
{
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh.reset_out_of_bnds();    

    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds())); kh.t += kh.dt) {
        kh.template do_step<IQ>();
    }
    std::cout << IC_idx << " time - " << kh.t << std::endl;
}

void pick_and_run_sum(kh_t& kh, size_t IC_idx)
{
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sc>(kh, IC_idx);
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sd>(kh, IC_idx);
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

void finish_recording_data(kh_t& kh)
{
    kh.final_states.back().out_of_bounds = kh.out_of_bounds();
    if(kh.out_of_bounds())
    {
        kh.pos_vel.first = kh.out_of_bounds_position();
        kh.reset_out_of_bnds();
    }

    kh.final_states.back().pos_vel = kh.pos_vel;
    kh.final_states.back().t_end = kh.t;
    kh.final_states.back().E = kh.eh.get_gr_E(kh.pos_vel);
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_t kh;
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    
    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx) {
	kh.final_states.push_back(st_sum_nms::state_summary{});
        pick_and_run_sum(kh, IC_idx);
	finish_recording_data(kh);
    }

    kh.print_contents();
}

int main(int argc, char ** argv)
{
try{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, kh_t::print_out_mem);
}
catch(prt_nms::prt_exc& exc)
{
	std::cout << exc.give_err() << std::endl;
}    
    return 0;
}



