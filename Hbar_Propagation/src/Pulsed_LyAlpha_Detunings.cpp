#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using dec_t = decay_nms::decay_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using aseh_t = ase_hand_nms::atomic_state_evolution_handler<ill_t, dec_t>;

std::vector<st_sum_nms::state_summary> overall_vec;
std::vector<st_sum_nms::state_summary> one_particle_vec;

size_t one_particle_vec_idx;
const double sample_interval {1.0};

void record_data(kh_t& kh, aseh_t& aseh)
{
    kh.final_states.back().pos_vel = kh.pos_vel;
    kh.final_states.back().t_end = one_particle_vec_idx*sample_interval;
    kh.final_states.back().E = kh.eh.get_gr_E(kh.pos_vel);
    
    kh.final_states.back().N = 1;
    kh.final_states.back().F = aseh.dec.decay_st("F");
    kh.final_states.back().mF = aseh.dec.decay_st("mF");
    
    kh.final_states.back().one_photon_decays = aseh.dec.one_ph_decays;
    kh.final_states.back().two_photon_decays = aseh.dec.two_ph_decays;
    kh.final_states.back().ionized = aseh.dec.ionized;
    
    kh.final_states.back().out_of_bounds = kh.out_of_bounds();
    
    one_particle_vec[one_particle_vec_idx++] = kh.final_states.back();
}

void finish_recording_data(kh_t& kh, aseh_t& aseh)
{
    const size_t one_particle_vec_idx_end {one_particle_vec_idx};
    size_t tmp_idx {one_particle_vec_idx};
    bool tmp_oob {kh.out_of_bounds()};
    
    if(kh.out_of_bounds())
    {
        kh.pos_vel.first = kh.out_of_bounds_position();
        kh.reset_out_of_bnds();
    }
    
    while(one_particle_vec_idx < one_particle_vec.size()) {record_data(kh, aseh);}
    while(tmp_idx < one_particle_vec.size())
    {
	one_particle_vec[tmp_idx].out_of_bounds = tmp_oob;
	one_particle_vec[tmp_idx].t_end = one_particle_vec_idx_end*sample_interval;
	++tmp_idx;
    }
    
    overall_vec.insert(overall_vec.end(), one_particle_vec.begin(), one_particle_vec.end());
}

template <ig_nms::interp_quant IQ>
void run_sim(kh_t& kh, aseh_t& aseh, size_t IC_idx)
{
    one_particle_vec_idx = 0;
    one_particle_vec.assign((kh.t_lim/sample_interval) + 1, st_sum_nms::state_summary{});
    
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()));)
    {
        double t_lim_next {std::min(kh.t_lim, kh.t + (1.0/aseh.ill.rep_rate))};
        
        for(;((kh.t < t_lim_next) && !(kh.out_of_bounds())); kh.t += kh.dt)
        {
            while(kh.t >= one_particle_vec_idx*sample_interval) {record_data(kh, aseh);}
            kh.template do_step<IQ>();
        }
        
        if((t_lim_next != kh.t_lim) && !(kh.out_of_bounds()) && aseh.in_beam(kh.pos_vel))
        {
            ++(kh.final_states.back().beam_passes);
            kh.final_states.back().t_in_beam += aseh.dt;
            
            aseh.initialize();
            if(aseh.do_step(kh.pos_vel) == ase_hand_nms::scatt_status::out_cycle)
	    {
		std::cout << "Detuning (MHz) : " << std::get<1>(aseh.ill.Rabi_freq_Doppler_by_path[0])/1.e6;
		std::cout << ", Doppler Detuning (MHz) : " << aseh.ill.Doppler_shift(kh.pos_vel.second, aseh.ill.paths[0]);
		std::cout << std::endl;
		return;
	    }
            if(aseh.ill.is_excited())
            {
                aseh.dec.execute_decay_chain(kh.pos_vel);
                kh.pos_vel.second += aseh.dec.vel_kick;
                if(aseh.dec.ionized || (aseh.dec.decay_st != aseh.ill.gr_state))
		{
			std::cout << "Detuning (MHz) : " << std::get<1>(aseh.ill.Rabi_freq_Doppler_by_path[0])/1.e6;
			std::cout << ", Doppler Detuning (MHz) : " << aseh.ill.Doppler_shift(kh.pos_vel.second, aseh.ill.paths[0]);
			std::cout << std::endl;
			return;
		}
            }
		std::cout << "Detuning (MHz) : " << std::get<1>(aseh.ill.Rabi_freq_Doppler_by_path[0])/1.e6;
		std::cout << ", Doppler Detuning (MHz) : " << aseh.ill.Doppler_shift(kh.pos_vel.second, aseh.ill.paths[0]);
		std::cout << std::endl;
        }
    }
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
    overall_vec.clear();

    kh_t kh;
    aseh_t aseh;
    
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    std::cout <<  "Minimum B : " << kh.Bnorm_min << std::endl;
    
    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
        aseh.fill(jm_nms::get_opts_fn(argc, argv));
        auto wcts = std::chrono::system_clock::now();
        std::cout << "Starting simulation for particle " << (IC_idx+1) << " of " << kh.ICs.size() << std::endl;
        
        kh.final_states.push_back(st_sum_nms::state_summary{});
        pick_and_run_sum(kh, aseh, IC_idx);
        finish_recording_data(kh, aseh);
        
        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << "Finished " << kh.t_lim << " s of simulation time in " << wctduration.count() << " s of real time" << std::endl;
    }
    
    st_sum_nms::print_state_summary(overall_vec, kh.out_fn, kh.part);
}

void print_out_mem(const boost::filesystem::path& opts_fn)
{
    kh_t tmp_kh;
    gqt_nms::fill_params(opts_fn, tmp_kh, tmp_kh.val_str);
    gqt_nms::print_memory(static_cast<size_t>((tmp_kh.t_lim/sample_interval) + 1)*fio_nms::data_len<global_nms::default_data_type>(tmp_kh.IC_fn)*sizeof(global_nms::default_data_type));
}

int main(int argc, char ** argv)
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, print_out_mem);
    
    return 0;
}

