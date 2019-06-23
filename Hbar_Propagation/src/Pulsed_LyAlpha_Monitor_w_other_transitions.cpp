#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

size_t curr_min {0};

size_t survivors {0};
double max_survival_t {0.0};
double avg_survival_t {0.0};
double var_survival_t {0.0};
using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_hand_nms::illumination_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using dec_t = decay_nms::decay_handler<pdef_nms::illum_type::LyAlph, EM_Quant_Gen>;
using aseh_t = ase_hand_nms::atomic_state_evolution_handler<ill_t, dec_t>;

std::vector<st_sum_nms::state_summary> overall_vec;
std::vector<st_sum_nms::state_summary> one_particle_vec;

size_t one_particle_vec_idx;
const double sample_interval {1.0};

void record_data(kh_t& kh, aseh_t& aseh, std::vector<aseh_t>& aseh_vec)
{
    kh.final_states.back().pos_vel = kh.pos_vel;
    kh.final_states.back().t_end = one_particle_vec_idx*sample_interval;
    kh.final_states.back().E = kh.eh.get_gr_E(kh.pos_vel);
    
    kh.final_states.back().N = 1;
    kh.final_states.back().F = aseh.dec.decay_st("F");
    kh.final_states.back().mF = aseh.dec.decay_st("mF");
    
    kh.final_states.back().one_photon_decays = aseh.dec.one_ph_decays;
    
    kh.final_states.back().two_photon_decays = 0;
    for(const auto& aseh_mod : aseh_vec) {
        if(aseh_mod.dec.two_ph_decays > 0) {
            if(kh.final_states.back().two_photon_decays == 0) {
                kh.final_states.back().two_photon_decays = aseh_mod.dec.two_ph_decays;
            }
            else {
                kh.final_states.back().two_photon_decays = std::min(kh.final_states.back().two_photon_decays, aseh_mod.dec.two_ph_decays);
            }
        }
    }
    for(const auto& aseh_mod : aseh_vec) {
        kh.final_states.back().two_photon_decays += 10000*aseh_mod.dec.one_ph_decays;
    }
    
    kh.final_states.back().ionized = aseh.dec.ionized;
    
    kh.final_states.back().out_of_bounds = kh.out_of_bounds();
    
    one_particle_vec[one_particle_vec_idx++] = kh.final_states.back();
}

void finish_recording_data(kh_t& kh, aseh_t& aseh, std::vector<aseh_t>& aseh_vec)
{
    if(kh.t >= kh.t_lim) {++survivors;}
    max_survival_t = std::max(max_survival_t, std::min(kh.t, kh.t_lim));
    avg_survival_t += std::min(kh.t, kh.t_lim);
    var_survival_t += std::min(kh.t, kh.t_lim)*std::min(kh.t, kh.t_lim);

    const size_t one_particle_vec_idx_end {one_particle_vec_idx};
    size_t tmp_idx {one_particle_vec_idx};
    bool tmp_oob {kh.out_of_bounds()};
    
    if(kh.out_of_bounds())
    {
        kh.pos_vel.first = kh.out_of_bounds_position();
        kh.reset_out_of_bnds();
    }
    
    while(one_particle_vec_idx < one_particle_vec.size()) {record_data(kh, aseh, aseh_vec);}
    while(tmp_idx < one_particle_vec.size())
    {
	one_particle_vec[tmp_idx].out_of_bounds = tmp_oob;
	one_particle_vec[tmp_idx].t_end = one_particle_vec_idx_end*sample_interval;
	++tmp_idx;
    }
    
    overall_vec.insert(overall_vec.end(), one_particle_vec.begin(), one_particle_vec.end());
}

template <ig_nms::interp_quant IQ>
void run_sim(kh_t& kh, aseh_t& aseh, std::vector<aseh_t>& aseh_vec, size_t IC_idx)
{
    one_particle_vec_idx = 0;
    one_particle_vec.assign((kh.t_lim/sample_interval) + 1, st_sum_nms::state_summary{});
    
    for(kh.t = 0.0; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()));)
    {
        double t_lim_next {std::min(kh.t_lim, kh.t + (1.0/aseh.ill.rep_rate))};
        
        for(;((kh.t < t_lim_next) && !(kh.out_of_bounds())); kh.t += kh.dt)
        {
            while(kh.t >= one_particle_vec_idx*sample_interval) {record_data(kh, aseh, aseh_vec);}
            kh.template do_step<IQ>();
        }

if(kh.t > curr_min*60.0) {std::cout << "Current minute : " << curr_min++ << std::endl;}
        
        if((t_lim_next != kh.t_lim) && !(kh.out_of_bounds()) && aseh.in_beam(kh.pos_vel))
        {
            ++(kh.final_states.back().beam_passes);
            kh.final_states.back().t_in_beam += aseh.dt;
            
            aseh.initialize();
            if(aseh.do_step(kh.pos_vel) == ase_hand_nms::scatt_status::out_cycle) {return;}
            if(aseh.ill.is_excited())
            {
                aseh.dec.execute_decay_chain(kh.pos_vel);
                kh.pos_vel.second += aseh.dec.vel_kick;
                if(aseh.dec.ionized || (aseh.dec.decay_st != aseh.ill.gr_state)) {return;}
            }
            
            for (aseh_t& aseh_mod : aseh_vec)
            {
		size_t init_one_ph {aseh_mod.dec.one_ph_decays};
                auto tmp_pos_vel {kh.pos_vel};
                
                aseh_mod.initialize();
                if((aseh_mod.do_step(tmp_pos_vel) == ase_hand_nms::scatt_status::out_cycle) && (aseh_mod.dec.two_ph_decays == 0)) {
			aseh_mod.dec.two_ph_decays = t_lim_next;
		}
                if(aseh_mod.ill.is_excited())
                {
                    aseh_mod.dec.execute_decay_chain(tmp_pos_vel);
                    tmp_pos_vel.second += aseh_mod.dec.vel_kick;
                    if((aseh_mod.dec.ionized || (aseh_mod.dec.decay_st != aseh_mod.ill.gr_state)) && (aseh_mod.dec.two_ph_decays == 0)) {
			aseh_mod.dec.two_ph_decays = t_lim_next;
		    }
                }

		if(aseh_mod.dec.one_ph_decays != init_one_ph) {
			std::cout << "Innocuous wrong 1-photon decay :";
			std::cout << " 2J = " << aseh_mod.ill.ex_state("2J");
			std::cout << ", 2mJ = " << aseh_mod.ill.ex_state("2mJ");
			std::cout << ", 2mI = " << aseh_mod.ill.ex_state("2mI");
			std::cout << std::endl;
		}
            }
        }
    }
}

void pick_and_run_sum(kh_t& kh, aseh_t& aseh, std::vector<aseh_t>& aseh_vec, size_t IC_idx)
{
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh.reset_out_of_bnds();
    kh.t = 0.0;
    
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sc>(kh, aseh, aseh_vec, IC_idx);
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        run_sim<ig_nms::interp_quant::acc_1Sd>(kh, aseh, aseh_vec, IC_idx);
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}


void fix_aseh_mod(const ill_t& ill_std, ill_t& ill_mod)
{
    TQam_nms::arr_mngr<> trans_freq_std;
    trans_freq_std.fill("Transition_Freq", ill_std.ex_state, ill_std.gr_state);
    
    TQam_nms::arr_mngr<> trans_freq_mod;
    trans_freq_mod.fill("Transition_Freq", ill_mod.ex_state, ill_mod.gr_state);
    
    ill_mod.det_0 = ill_std.det_0 + (trans_freq_std.quants[1] - ill_std.Zeeman_fact.quants[1]) - (trans_freq_mod.quants[1] - ill_mod.Zeeman_fact.quants[1]);
    
    ill_mod.det = ill_mod.det_0 - ill_mod.Zeeman_fact(ill_mod.B_0_det);
    
    ill_mod.freq_drive = trans_freq_mod.quants[1] - ill_mod.Zeeman_fact.quants[1] + ill_mod.det_0;
}


void fill_aseh_vec(int argc, char ** argv, const aseh_t& aseh_std, std::vector<aseh_t>& aseh_vec)
{
    size_t st_idx {0};
    for(const auto& st : asts_nms::P_ex::basis())
    {
        if(st != aseh_std.ill.ex_state)
        {
            boost::filesystem::path dir {jm_nms::get_opts_fn(argc, argv).parent_path()};
            dir /= boost::filesystem::path{"Bad_Transitions"};
            dir /= "2J_" + std::to_string(st("2J")) + "_2mJ_" + std::to_string(st("2mJ")) + "_2mI_" + std::to_string(st("2mI"));
            
            aseh_vec.at(st_idx).fill(dir / jm_nms::get_opts_fn(argc, argv).filename());
            fix_aseh_mod(aseh_std.ill, aseh_vec.at(st_idx).ill);
            
            ++st_idx;
        }
    }
}


void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    overall_vec.clear();

    kh_t kh;
    aseh_t aseh;
    std::vector<aseh_t> aseh_vec {asts_nms::P_ex::basis().size() - 1};
    
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    
    std::cout << "Loaded" << std::endl;
    std::cout <<  "Minimum B : " << kh.Bnorm_min << std::endl;
    
    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
        aseh.fill(jm_nms::get_opts_fn(argc, argv));
        fill_aseh_vec(argc, argv, aseh, aseh_vec);
        
        auto wcts = std::chrono::system_clock::now();
        std::cout << "Starting simulation for particle " << (IC_idx+1) << " of " << kh.ICs.size() << std::endl;
        
        kh.final_states.push_back(st_sum_nms::state_summary{});
        pick_and_run_sum(kh, aseh, aseh_vec, IC_idx);
        finish_recording_data(kh, aseh, aseh_vec);
        
        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << "Finished " << kh.t_lim << " s of simulation time in " << wctduration.count() << " s of real time" << std::endl;
	std::cout << "Survivors = " << survivors << " of " << IC_idx+1 << " : " << (survivors/(IC_idx+1))*100.0 << "%" << std::endl;
	std::cout << "Max survival time = " << max_survival_t << " s" << std::endl;
	std::cout << "Mean survival time = " << avg_survival_t/(IC_idx+1) << " +/- " << std::sqrt(var_survival_t/((IC_idx+1)*std::max(1.0*IC_idx, 1.0))) << " s" << std::endl;
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

