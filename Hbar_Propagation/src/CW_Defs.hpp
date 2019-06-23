#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

size_t survivors {0};
double max_survival_t {0.0};
double avg_survival_t {0.0};
double var_survival_t {0.0};
using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_hand_nms::illumination_handler<it_const, EM_Quant_Gen>;
using dec_t = decay_nms::decay_handler<it_const, EM_Quant_Gen>;
using aseh_t = ase_hand_nms::atomic_state_evolution_handler<ill_t, dec_t>;

using func_ptr_t = void (*)(kh_t&, aseh_t&);
func_ptr_t next_func {nullptr};

template <ig_nms::interp_quant IQ> void run_reg(kh_t& kh, aseh_t& aseh);
template <ig_nms::interp_quant IQ> void in_beam_func(kh_t& kh, aseh_t& aseh);
template <ig_nms::interp_quant IQ> void decaying_func(kh_t& kh, aseh_t& aseh);

/* pseudo_propagator */
template <ig_nms::interp_quant IQ>
struct pseudo_propagator
{
    double t_init;
    pdef_nms::state_t pos_vel_init;
    
    bool min_max_init;
    global_nms::default_data_type Rabi_freq_max;
    global_nms::default_data_type decay_rate_min;
    
    global_nms::default_data_type P_exc_0;
    global_nms::default_data_type P_no_dec_lo_bd;
    
    void init(const kh_t& kh, const aseh_t& aseh)
    {
        t_init = kh.t;
        pos_vel_init = kh.pos_vel;
        
        min_max_init = false;
        
        P_exc_0 = (aseh.ill.d_mat[ill_t::P] + aseh.ill.d_mat[ill_t::w])/2.0;
        P_no_dec_lo_bd = aseh.ill.d_mat[ill_t::P];
    }
    
    void reset_kh(kh_t& kh)
    {
        kh.t = t_init;
        kh.pos_vel = pos_vel_init;
        kh.reset_out_of_bnds();
    }
    
    bool test_propagation(kh_t& kh, aseh_t& aseh)
    {
static size_t tot {0};
static size_t detailed {0};

tot++;
        init(kh, aseh);
        
        for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && aseh.in_beam(kh.pos_vel)); kh.t += kh.dt)
        {
            aseh.ill.update(kh.pos_vel);

            global_nms::default_data_type loc_Rabi_freq {0.0};
            for(const auto& RFDbp : aseh.ill.Rabi_freq_Doppler_by_path) {
                loc_Rabi_freq += std::abs(std::get<0>(RFDbp));
            }
            
            if(min_max_init)
            {
                Rabi_freq_max = std::max(Rabi_freq_max, loc_Rabi_freq);
                decay_rate_min = std::min(decay_rate_min, aseh.ill.prob_decay_rate);
            }
            else
            {
                Rabi_freq_max = loc_Rabi_freq;
                decay_rate_min = aseh.ill.prob_decay_rate;
                min_max_init = true;
            }
            
            auto P_exc_st_up_bd {P_exc_0 + (Rabi_freq_max*Rabi_freq_max/(decay_rate_min*(decay_rate_min + (2.0*M_PI*aseh.ill.FWHM))))};
            P_no_dec_lo_bd -= aseh.ill.prob_decay_rate*P_exc_st_up_bd*kh.dt;
                        
            if((P_no_dec_lo_bd < aseh.ill.decay_threshold) || (P_exc_st_up_bd > aseh.ill.excitation_threshold*P_no_dec_lo_bd))
            {
                reset_kh(kh);
                return true;
            }
            
            kh.template do_step<IQ>();
        }
        return false;
    }
};

template <ig_nms::interp_quant IQ>
void run_reg(kh_t& kh, aseh_t& aseh)
{
    for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && !(aseh.in_beam(kh.pos_vel))); kh.t += kh.dt) {
        kh.template do_step<IQ>();
    }
    if((kh.t >= kh.t_lim) || kh.out_of_bounds()) {next_func = nullptr;}
    else
    {
        next_func = in_beam_func<IQ>;
        aseh.initialize();
    }
}

template <ig_nms::interp_quant IQ>
void in_beam_func(kh_t& kh, aseh_t& aseh)
{
    ++(kh.final_states.back().beam_passes);
    double start_t_in_beam {kh.t};
    
    kh.dt = aseh.dt;
    
    bool full_propagation {pseudo_propagator<IQ>{}.test_propagation(kh, aseh)};
    if(full_propagation)
    {
        for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && aseh.in_beam(kh.pos_vel)); kh.t += kh.dt)
        {
            if(aseh.do_step(kh.pos_vel) == ase_hand_nms::scatt_status::out_cycle)
            {
                kh.final_states.back().t_in_beam += kh.t - start_t_in_beam;
                next_func = nullptr;
		kh.dt = aseh.dt_nrm;
                return;
            }
            kh.template do_step<IQ>();
        }
    }
    
    kh.dt = aseh.dt_nrm;
    
    kh.final_states.back().t_in_beam += kh.t - start_t_in_beam;
    
    if((kh.t >= kh.t_lim) || kh.out_of_bounds()) {next_func = nullptr;}
    else if(full_propagation && aseh.ill.is_excited())
    {
        next_func = decaying_func<IQ>;
        aseh.initialize(false);
    }
    else {next_func = run_reg<IQ>;}
}

template <ig_nms::interp_quant IQ>
void decaying_func(kh_t& kh, aseh_t& aseh)
{
    kh.dt = aseh.dt;
    aseh.dec.vel_kick = {0.0, 0.0, 0.0};
    
    for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && !(aseh.in_beam(kh.pos_vel))); kh.t += kh.dt)
    {
        if(aseh.do_step(kh.pos_vel) == ase_hand_nms::scatt_status::out_cycle)
        {
            next_func = nullptr;
            return;
        }
        else if(norm_sq(aseh.dec.vel_kick)) {break;}
        kh.template do_step<IQ>();
    }
    kh.dt = aseh.dt_nrm;
    
    if((kh.t >= kh.t_lim) || kh.out_of_bounds()) {next_func = nullptr;}
    else if(aseh.in_beam(kh.pos_vel)) {next_func = in_beam_func<IQ>;}
    else {next_func = run_reg<IQ>;}
}

void finish_recording_data(kh_t& kh, aseh_t& aseh)
{
if(kh.t < kh.t_lim) {
    std::cout << "Lost at " << kh.t << std::endl;
}
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
        for(next_func = run_reg<ig_nms::interp_quant::acc_1Sc>; next_func;) {next_func(kh, aseh);}
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        for(next_func = run_reg<ig_nms::interp_quant::acc_1Sd>; next_func;) {next_func(kh, aseh);}
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
    
    kh.print_contents();
}

int main(int argc, char ** argv)
{
try
{
    jm_nms::pseudo_main(argc, argv, generate_ICs, kh_t::output_files, kh_t::write_opt_file_template, kh_t::print_out_mem);
}
catch(gqt_nms::gqt_exc& exc) {
std::cout << exc.give_err() << std::endl;
}    
    return 0;
}

