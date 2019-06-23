#include <iostream>
#include <chrono>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

constexpr pdef_nms::illum_type it_const {pdef_nms::illum_type::TwoPh};

extern const std::vector<int> gl_detuning_vec;
bool gl_out_of_bds;

using EM_Quant_Gen = emq_nms::grid_EM_quants<ig_nms::interp_type::lin>;

using kh_t = kin_hand_nms::kinematic_handler<true, boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, global_nms::default_data_type, bvec_nms::basic_vec<>, bvec_nms::basic_vec<>, double, boost::numeric::odeint::vector_space_algebra>, 0, EM_Quant_Gen>;

using ill_t = ill_hand_nms::illumination_handler<it_const, EM_Quant_Gen>;
using dec_t = decay_nms::decay_handler<it_const, EM_Quant_Gen>;
using aseh_t = ase_hand_nms::atomic_state_evolution_handler<ill_t, dec_t>;

using aseh_vec_elem_t = std::tuple<aseh_t, st_sum_nms::state_summary, size_t, bool>;
using aseh_vec_t = std::vector<aseh_vec_elem_t>;

using func_ptr_t = void (*)(kh_t&, aseh_vec_t&);
func_ptr_t next_func {nullptr};

template <ig_nms::interp_quant IQ> void run_reg(kh_t&, aseh_vec_t&);
template <ig_nms::interp_quant IQ> void in_beam_func(kh_t&, aseh_vec_t&);

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
    
    void init(const kh_t& kh)
    {
        t_init = kh.t;
        pos_vel_init = kh.pos_vel;
        
        min_max_init = false;
        
        P_exc_0 = 0.0;
        P_no_dec_lo_bd = 1.0;
    }
    
    void reset_kh(kh_t& kh)
    {
        kh.t = t_init;
        kh.pos_vel = pos_vel_init;
        kh.reset_out_of_bnds();
    }
    
    bool test_propagation(kh_t& kh, aseh_vec_t& aseh_vec)
    {
        static size_t tot {0};
        static size_t detailed {0};
        
        aseh_t& aseh {std::get<0>(aseh_vec.front())};

        ++tot;
        init(kh);
        
        global_nms::default_data_type decay_threshold {0.0};
        global_nms::default_data_type excitation_threshold {1.0};
        
        for(auto& aseh_elem : aseh_vec)
        {
            if(std::get<3>(aseh_elem))
            {
                decay_threshold = std::max(decay_threshold, std::get<0>(aseh_elem).ill.decay_threshold);
                excitation_threshold = std::min(excitation_threshold, std::get<0>(aseh_elem).ill.excitation_threshold);
            }
        }
        
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
                        
            if((P_no_dec_lo_bd < decay_threshold) || (P_exc_st_up_bd > excitation_threshold*P_no_dec_lo_bd))
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
void run_reg(kh_t& kh, aseh_vec_t& aseh_vec)
{
    aseh_t& aseh {std::get<0>(aseh_vec.front())};
    
    for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && !(aseh.in_beam(kh.pos_vel))); kh.t += kh.dt) {
        kh.template do_step<IQ>();
    }
    
    if((kh.t >= kh.t_lim) || kh.out_of_bounds()) {next_func = nullptr;}
    else
    {
        next_func = in_beam_func<IQ>;
        for(aseh_vec_elem_t& aseh_elem : aseh_vec) {std::get<0>(aseh_elem).initialize();}
    }
}

void finish_recording_aseh_elem(kh_t&, aseh_vec_elem_t&);

template <ig_nms::interp_quant IQ>
void in_beam_func(kh_t& kh, aseh_vec_t& aseh_vec)
{
    aseh_t& aseh {std::get<0>(aseh_vec.front())};
    
    for(aseh_vec_elem_t& aseh_elem : aseh_vec) {++(std::get<1>(aseh_elem).beam_passes);}
    double start_t_in_beam {kh.t};
    
    kh.dt = aseh.dt;
    
    bool full_propagation {pseudo_propagator<IQ>{}.test_propagation(kh, aseh_vec)};
    if(full_propagation)
    {
        for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && aseh.in_beam(kh.pos_vel)); kh.t += kh.dt)
        {
            for(aseh_vec_elem_t& aseh_elem : aseh_vec)
            {
                auto tmp_pos_vel {kh.pos_vel};
                if(std::get<3>(aseh_elem) &&
                   (std::get<0>(aseh_elem).do_step(tmp_pos_vel) == ase_hand_nms::scatt_status::out_cycle))
                {
                    std::get<1>(aseh_elem).t_in_beam += kh.t - start_t_in_beam;
                    finish_recording_aseh_elem(kh, aseh_elem);
                    
                    /* Check that there's anything left to do */
                    bool something_active {false};
                    for(aseh_vec_elem_t& aseh_elem_check : aseh_vec) {
                        something_active |= std::get<3>(aseh_elem_check);
                    }
                    if(!something_active)
                    {
                        next_func = nullptr;
                        return;
                    }
                }
            }
            
            kh.template do_step<IQ>();
        }
    }
    
    kh.dt = aseh.dt_nrm;
    
    for(aseh_vec_elem_t& aseh_elem : aseh_vec) {std::get<1>(aseh_elem).t_in_beam += kh.t - start_t_in_beam;}
    
    if((kh.t >= kh.t_lim) || kh.out_of_bounds())
    {
        next_func = nullptr;
        return;
    }
    
    if(full_propagation) {
        for(aseh_vec_elem_t& aseh_elem : aseh_vec)
        {
            if(std::get<3>(aseh_elem) && std::get<0>(aseh_elem).ill.is_excited())
            {
                std::get<0>(aseh_elem).dec.execute_decay_chain(kh.pos_vel);
                if(std::get<0>(aseh_elem).dec.ionized || (std::get<0>(aseh_elem).dec.decay_st != std::get<0>(aseh_elem).ill.gr_state))
                {
                    finish_recording_aseh_elem(kh, aseh_elem);
                    
                    /* Check that there's anything left to do */
                    bool something_active {false};
                    for(aseh_vec_elem_t& aseh_elem_check : aseh_vec) {
                        something_active |= std::get<3>(aseh_elem_check);
                    }
                    if(!something_active)
                    {
                        next_func = nullptr;
                        return;
                    }
                }
            }
        }
    }
    
    next_func = run_reg<IQ>;
}

void finish_recording_aseh_elem(kh_t& kh, aseh_vec_elem_t& aseh_elem)
{
    if(kh.t < kh.t_lim) {
        std::cout << "*** Detuning " << std::to_string(gl_detuning_vec[std::get<2>(aseh_elem) % (gl_detuning_vec.size())]) << " lost at " << kh.t << " ***" << std::endl;
    }
    else {
        std::cout << "Detuning " << std::to_string(gl_detuning_vec[std::get<2>(aseh_elem) % (gl_detuning_vec.size())]) << " finished" << std::endl;
    }
    std::get<3>(aseh_elem) = false;
    
    if(kh.out_of_bounds())
    {
        kh.pos_vel.first = kh.out_of_bounds_position();
        kh.reset_out_of_bnds();
        gl_out_of_bds = true;
    }
    
    st_sum_nms::state_summary& st_sum {kh.final_states[std::get<2>(aseh_elem)]};
    aseh_t& aseh {std::get<0>(aseh_elem)};
    
    st_sum.pos_vel = kh.pos_vel;
    
    st_sum.t_end = kh.t;
    st_sum.E = kh.eh.get_gr_E(kh.pos_vel);
    
    st_sum.N = 1;
    st_sum.F = aseh.dec.decay_st("F");
    st_sum.mF = aseh.dec.decay_st("mF");
    
    st_sum.one_photon_decays = aseh.dec.one_ph_decays;
    st_sum.two_photon_decays = aseh.dec.two_ph_decays;
    st_sum.ionized = aseh.dec.ionized;
    
    st_sum.out_of_bounds = gl_out_of_bds;
    
    st_sum.beam_passes = std::get<1>(aseh_elem).beam_passes;
    st_sum.t_in_beam = std::get<1>(aseh_elem).t_in_beam;
}

void finish_recording_data(kh_t& kh, aseh_vec_t& aseh_vec)
{
    for(aseh_vec_elem_t& aseh_elem : aseh_vec) {
        if(std::get<3>(aseh_elem)) {finish_recording_aseh_elem(kh, aseh_elem);}
    }
}

void pick_and_run_sum(kh_t& kh, aseh_vec_t& aseh_vec, size_t IC_idx)
{
    kh.pos_vel = kh.ICs[IC_idx].pos_vel;
    kh.reset_out_of_bnds();
    kh.t = 0.0;
    
    if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", 0}}}) {
        for(next_func = run_reg<ig_nms::interp_quant::acc_1Sc>; next_func;) {next_func(kh, aseh_vec);}
    }
    else if(kh.gr_state == asts_nms::S_gr{{{"F", 1}, {"mF", -1}}}) {
        for(next_func = run_reg<ig_nms::interp_quant::acc_1Sd>; next_func;) {next_func(kh, aseh_vec);}
    }
    else {
        throw kin_hand_nms::kin_hand_exc{"Not a trappable state"};
    }
}

boost::filesystem::path get_det_fn(boost::filesystem::path base_opts_fn, size_t det_idx)
{
    auto det_par_path {base_opts_fn.parent_path()};
    boost::filesystem::path det_stem {base_opts_fn.stem().string() + "_" + std::to_string(gl_detuning_vec[det_idx])};
    auto det_ext {base_opts_fn.extension()};
    
    return (det_par_path / det_stem.replace_extension(det_ext));
}

void generate_ICs(int argc, char ** argv, size_t tot_parts, size_t part_idx)
{
    kh_t kh;
    aseh_vec_t aseh_vec {gl_detuning_vec.size()};
    
    kh.fill(jm_nms::get_opts_fn(argc, argv), tot_parts, part_idx);
    kh.final_states.resize(kh.ICs.size()*gl_detuning_vec.size());
    
    std::cout << "Loaded" << std::endl;
    std::cout <<  "Minimum B : " << kh.Bnorm_min << std::endl;
    
    for(size_t IC_idx = 0; IC_idx < kh.ICs.size(); ++IC_idx)
    {
        gl_out_of_bds = false;
        for(size_t idx = 0; idx < aseh_vec.size(); ++idx)
        {
            std::get<0>(aseh_vec[idx]).fill(get_det_fn(jm_nms::get_opts_fn(argc, argv), idx));
            std::get<1>(aseh_vec[idx]) = st_sum_nms::state_summary{};
            std::get<2>(aseh_vec[idx]) = (IC_idx*gl_detuning_vec.size()) + idx;
            std::get<3>(aseh_vec[idx]) = true;
        }
        
        auto wcts = std::chrono::system_clock::now();
        std::cout << "Starting simulation for particle " << (IC_idx+1) << " of " << kh.ICs.size() << std::endl;

        pick_and_run_sum(kh, aseh_vec, IC_idx);
        finish_recording_data(kh, aseh_vec);

        std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
        std::cout << "Finished " << kh.t << " s of simulation time in " << wctduration.count() << " s of real time" << std::endl << std::endl;
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

