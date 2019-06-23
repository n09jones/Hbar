#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using dec_t = decay_nms::decay_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using aseh_t = ase_hand_nms::atomic_state_evolution_handler<ill_t, dec_t>;

void handle_laser_exposure(kh_t& kh, aseh_t& aseh, global_nms::default_data_type Ef, bool forced_scatter)
{
    using namespace pvsi_nms;
    
    auto Ei {kh.eh.get_gr_E(kh.pos_vel)};
    auto vi {kh.pos_vel.second};
    auto decsi {aseh.dec.one_ph_decays};
    auto mass_H {mass_e + mass_p};
    auto dv {h*aseh.ill.freq_drive/(mass_H*c)};
    
    aseh.initialize();
    aseh.do_step(kh.pos_vel);
    if(aseh.ill.is_excited())
    {
        aseh.dec.execute_decay_chain(kh.pos_vel);
        kh.pos_vel.second += aseh.dec.vel_kick;
    }
    
    if((kh.eh.get_gr_E(kh.pos_vel) > Ei) || (kh.eh.get_gr_E(kh.pos_vel) < Ef)) {kh.pos_vel.second = vi;}
    else if(kh.eh.get_gr_E(kh.pos_vel) < Ei) {std::cout << "t = " << kh.t << ", E = " << (1000.0*kh.eh.get_gr_E(kh.pos_vel)/1.5) << " mK, E_i = " << (1000.0*Ei/1.5) << ", E_f = " << (1000.0*Ef/1.5) << " mK" << std::endl;}
    
    if(((aseh.dec.one_ph_decays > decsi) || forced_scatter) && (kh.pos_vel.second == vi) &&
       (((kh.eh.get_gr_E(kh.pos_vel) - Ef)*k_B < 0.5*mass_H*dv*dv) || forced_scatter))
    {
        std::cout << "Scaling from " << (1000.0*kh.eh.get_gr_E(kh.pos_vel)/1.5) << "mK, E_f = " << (1000.0*Ef/1.5) << " mK" << std::endl;
        auto Pot_E {(kh.eh.get_gr_E(kh.pos_vel)*k_B)-(0.5*mass_H*norm_sq(kh.pos_vel.second))};
        auto ideal_KE {(Ef*k_B) - Pot_E};
        if(ideal_KE >= 0.0) {
            if(norm(kh.pos_vel.second) == 0.0) {kh.pos_vel.second[2] += 1.0e-6;}
            kh.pos_vel.second *= std::sqrt(ideal_KE/(0.5*mass_H))/norm(kh.pos_vel.second);
        }
	else {kh.pos_vel.second *= 0.1;}
        std::cout << "Scaled to " << (1000.0*kh.eh.get_gr_E(kh.pos_vel)/1.5) << "mK, E_f = " << (1000.0*Ef/1.5) << " mK" << std::endl;
    }
}

template <ig_nms::interp_quant IQ>
void run_sim(kh_t& kh, aseh_t& aseh, size_t IC_idx, global_nms::default_data_type Ef)
{
    double t_last_scatt {0.0};

    for(kh.t = 0.0; (!(kh.out_of_bounds()) && (kh.t < 3600.0));)
    {
        double t_lim_next {kh.t + (1.0/aseh.ill.rep_rate)};
        for(;((kh.t < t_lim_next) && !(kh.out_of_bounds())); kh.t += kh.dt) {kh.template do_step<IQ>();}
        
        if(!(kh.out_of_bounds()) && (aseh.in_beam(kh.pos_vel) || (kh.t - t_last_scatt > 60.0)))
        {
	    if(kh.t - t_last_scatt > 60.0) {std::cout << "Extra scatter at " << kh.t << " seconds" << std::endl;}
            ++(kh.final_states.back().beam_passes);
            kh.final_states.back().t_in_beam += aseh.dt;
            handle_laser_exposure(kh, aseh, Ef, (kh.t - t_last_scatt > 60.0));
	    t_last_scatt = kh.t;
        }
        
        if(std::abs(kh.eh.get_gr_E(kh.pos_vel) - Ef) <= std::abs(Ef)*(1.0e-6)) {
            std::cout << "Finishing up at " << (1000.0*kh.eh.get_gr_E(kh.pos_vel)/1.5) << "mK, E_f = " << (1000.0*Ef/1.5) << " mK" << std::endl;
            break;
        }
    }
    
    auto fin_lim {kh.t + 10.0};
    
    for(;((kh.t < fin_lim) && !(kh.out_of_bounds())); kh.t += kh.dt) {kh.template do_step<IQ>();}
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

void pick_and_run_sum(kh_t& kh, aseh_t& aseh, size_t IC_idx, global_nms::default_data_type Ef)
{
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh.reset_out_of_bnds();
    kh.t = 0.0;
    
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sc>(kh, aseh, IC_idx, Ef);
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sd>(kh, aseh, IC_idx, Ef);
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

std::vector<global_nms::default_data_type> generate_Ef_vec(const kh_t& kh)
{
    std::vector<global_nms::default_data_type> Ef_vec {};
    for(const auto& elem : kh.ICs) {Ef_vec.push_back(elem.E);}
    
    auto mean {std::accumulate(Ef_vec.begin(), Ef_vec.end(), 0.0)/Ef_vec.size()};
    auto target_mean {0.75*pvsi_nms::h*(99.58e6)/pvsi_nms::k_B};
    
    for(auto& elem : Ef_vec) {elem *= target_mean/mean;}
    
    return Ef_vec;
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_t kh;
    aseh_t aseh;
    
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    std::cout <<  "Minimum B : " << kh.Bnorm_min << std::endl;
    
    auto Ef_vec {generate_Ef_vec(kh)};
    
    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
        kh.final_states.push_back(st_sum_nms::state_summary{});
        
        auto wcts = std::chrono::system_clock::now();
        do
        {
            aseh.fill(jm_nms::get_opts_fn(argc, argv));
            std::cout <<  "Minimum B : " << aseh.ill.B_0_det << ", ";
            std::cout << "Detuning : " << aseh.ill.det_0 << ", ";
            std::cout << "rel_det : " << aseh.ill.det << std::endl;
            std::cout << "Starting simulation for particle " << (IC_idx+1) << " of " << kh.ICs.size() << std::endl;
            
            pick_and_run_sum(kh, aseh, IC_idx, Ef_vec[IC_idx]);
        } while (kh.out_of_bounds());

        finish_recording_data(kh, aseh);
        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << "Finished " << kh.t << " s of simulation time in " << wctduration.count() << " s of real time" << std::endl;
    }
    
    kh.print_contents();
}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, kh_t::print_out_mem);
    
    return 0;
}

