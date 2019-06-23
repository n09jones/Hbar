#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using dec_t = decay_nms::decay_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using aseh_t = ase_hand_nms::atomic_state_evolution_handler<ill_t, dec_t>;

template <ig_nms::interp_quant IQ>
void run_sim(kh_t& kh, aseh_t& aseh, size_t IC_idx)
{
    const global_nms::default_data_type freq_incr {100.e6};
    const double s_per_incr {1000.0};
    double next_incr {s_per_incr};
    
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()));)
    {
        double t_lim_next {std::min(kh.t_lim, kh.t + (1.0/aseh.ill.rep_rate))};
        for(;((kh.t < t_lim_next) && !(kh.out_of_bounds())); kh.t += kh.dt) {kh.template do_step<IQ>();}
        if((t_lim_next != kh.t_lim) && !(kh.out_of_bounds()) && aseh.in_beam(kh.pos_vel))
        {
            while (kh.t >= next_incr)
            {
                /* change what needs changed */
                aseh.ill.det += freq_incr;
                aseh.ill.det_0 += freq_incr;
                aseh.ill.freq_drive += freq_incr;
                next_incr += s_per_incr;
            }
            
            ++(kh.final_states.back().beam_passes);
            kh.final_states.back().t_in_beam += aseh.dt;
            
            aseh.initialize();
            if(aseh.do_step(kh.pos_vel) == ase_hand_nms::scatt_status::out_cycle)
	    {
		return;
	    }
            if(aseh.ill.is_excited())
            {
                aseh.dec.execute_decay_chain(kh.pos_vel);
                kh.pos_vel.second += aseh.dec.vel_kick;
                if(aseh.dec.ionized || (aseh.dec.decay_st != aseh.ill.gr_state))
		{
			return;
		}
            }
        }
    }
}

void finish_recording_data(kh_t& kh, aseh_t& aseh)
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
    
    kh.final_states.back().N = 1;
    kh.final_states.back().F = aseh.dec.decay_st("F");
    kh.final_states.back().mF = aseh.dec.decay_st("mF");
    
    kh.final_states.back().one_photon_decays = aseh.dec.one_ph_decays;
    kh.final_states.back().two_photon_decays = aseh.dec.two_ph_decays;
    kh.final_states.back().ionized = aseh.dec.ionized;
}

void pick_and_run_sum(kh_t& kh, aseh_t& aseh, size_t IC_idx)
{
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh.reset_out_of_bnds();
    kh.t = 0.0;
    
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sc>(kh, aseh, IC_idx);
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sd>(kh, aseh, IC_idx);
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_t kh;
    aseh_t aseh;
    
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    
    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
        aseh.fill(jm_nms::get_opts_fn(argc, argv));
        auto wcts = std::chrono::system_clock::now();
        kh.final_states.push_back(st_sum_nms::state_summary{});
        pick_and_run_sum(kh, aseh, IC_idx);
        finish_recording_data(kh, aseh);
        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << kh.t << std::endl;
    }
    
    kh.print_contents();
}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, kh_t::print_out_mem);
    
    return 0;
}

