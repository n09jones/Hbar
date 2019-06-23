#ifndef ILLUMINATION_HANDLER_HPP
#define ILLUMINATION_HANDLER_HPP

#include "Random_Distributions.h"

/* Illumination handler base functions */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void ill_hand_nms::illumination_handler_base<IT, EM_Quant_Gen>::fill(const boost::filesystem::path& opts_fn)
{
    this->initialize(opts_fn);
    this->fill_laser_properties(opts_fn);
    this->fill_path_mngr(opts_fn);
    
    if(this->B_0_det < 0.0) {this->B_0_det = EM_Quant_Gen::Bnorm_min;}
    
    gr_state.fill(opts_fn);
    ex_state.fill(opts_fn);
    
    decay_rt_1ph_fact.fill("Decay_Rate_1Ph", ex_state);
    Rabi_freq_fact.fill("Rabi_Freq", ex_state, gr_state);
    Zeeman_fact.fill("Zeeman_Shift", ex_state, gr_state);
    
    if(pdef_nms::is_TwoPh<IT>)
    {
        decay_rt_2ph_fact.fill("Decay_Rate_2Ph", ex_state);
        ion_rt_fact.fill("Ionization_Rate", ex_state);
        AC_Stark_fact.fill("AC_Stark_Shift", ex_state, gr_state);
        DC_Stark_fact.fill("DC_Stark_Shift", ex_state, gr_state);
    }
    
    global_nms::default_data_type two_ph_dbling {(pdef_nms::is_TwoPh<IT> ? 2.0 : 1.0)};
    
    FWHM = two_ph_dbling*las_props_nms::laser_properties::FWHM;
    det_0 = (two_ph_dbling*(this->det)) + Zeeman_fact(this->B_0_det);
    
    TQam_nms::arr_mngr<> trans_freq;
    trans_freq.fill("Transition_Freq", ex_state, gr_state);
    freq_drive = trans_freq.quants[1] - Zeeman_fact.quants[1] + det_0;
    
    Rabi_freq_Doppler_by_path.resize(paths.size());
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void ill_hand_nms::illumination_handler_base<IT, EM_Quant_Gen>::update(const pdef_nms::state_t& pos_vel)
{
    this->load_for_illumination(pos_vel);
    
    decay_rt_1ph_fact.update_quant(this->Bnorm);
    Rabi_freq_fact.update_quant(this->Bnorm);
    Zeeman_fact.update_quant(this->Bnorm);
    
    if(pdef_nms::is_TwoPh<IT>)
    {
        decay_rt_2ph_fact.update_quant(this->Bnorm);
        ion_rt_fact.update_quant(this->Bnorm);
        AC_Stark_fact.update_quant(this->Bnorm);
        DC_Stark_fact.update_quant(this->Bnorm);
    }
    
    update_derived_quants(pos_vel);
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void ill_hand_nms::illumination_handler_base<IT, EM_Quant_Gen>::update_derived_quants(const pdef_nms::state_t& pos_vel)
{
    prob_decay_rate = (pdef_nms::is_TwoPh<IT> ? ((decay_rt_1ph_fact()*(this->E_sq)) + decay_rt_2ph_fact()) : decay_rt_1ph_fact());
    auto non_Doppler_detuning {det_0 - Zeeman_fact() - (pdef_nms::is_TwoPh<IT> ? DC_Stark_fact()*(this->E_sq) : 0)};
    
    for(size_t p_idx = 0; p_idx < paths.size(); ++p_idx)
    {
        this->load_per_path(pos_vel, paths[p_idx]);
        
        if(pdef_nms::is_TwoPh<IT>)
        {
            prob_decay_rate += ion_rt_fact()*(this->intensity);
            non_Doppler_detuning -= AC_Stark_fact()*(this->intensity);
            std::get<0>(Rabi_freq_Doppler_by_path[p_idx]) = Rabi_freq_fact()[0]*(this->intensity);
        }
        else
        {
            auto path_Rabi {std::inner_product((this->loc_pol).begin(), (this->loc_pol).end(), Rabi_freq_fact().begin(), std::complex<global_nms::default_data_type>{})};
            std::get<0>(Rabi_freq_Doppler_by_path[p_idx]) = path_Rabi*std::sqrt(this->intensity);
        }
        
        std::get<1>(Rabi_freq_Doppler_by_path[p_idx]) = Doppler_shift(pos_vel.second, paths[p_idx]);
    }
    
    for (auto& RfDbp : Rabi_freq_Doppler_by_path) {std::get<1>(RfDbp) += non_Doppler_detuning;}
}


/* Compute Doppler shift */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
global_nms::default_data_type ill_hand_nms::illumination_handler_base<IT, EM_Quant_Gen>::Doppler_shift(const bvec_nms::basic_vec<>& v, const las_path_nms::laser_path& path)
{
    auto beta {v / pvsi_nms::c};
    return freq_drive*(((pdef_nms::is_TwoPh<IT> ? 1 : (1 - inner_pr(beta, path.unit_wave_vector)))/std::sqrt(1 - norm_sq(beta))) - 1);
}


/* Illumination handler functions */
/*
 This should be run when
 - each time the atom first comes into view of a laser
 - each time the atom leaves the laser's view in the excited state
 */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
inline void ill_hand_nms::illumination_handler<IT, EM_Quant_Gen>::initialize(bool in_gr_state)
{
    d_mat[u] = 0.0;
    d_mat[v] = 0.0;
    d_mat[w] = (in_gr_state ? -1.0 : 1.0);
    d_mat[P] = 1.0;
    
    acc_phases.assign(this->Rabi_freq_Doppler_by_path.size(), 0.0);
    
    decay_threshold = distr_gen::rand();
    excitation_threshold = distr_gen::rand();
}


template <pdef_nms::illum_type IT, class EM_Quant_Gen>
inline void ill_hand_nms::illumination_handler<IT, EM_Quant_Gen>::operator() (const ill_hand_nms::illumination_handler<IT, EM_Quant_Gen>::d_mat_t& x, ill_hand_nms::illumination_handler<IT, EM_Quant_Gen>::d_mat_t& dxdt, const double t)
{
    std::complex<global_nms::default_data_type> Rabi_freq {0.0};
    
    for(unsigned idx = 0; idx < this->Rabi_freq_Doppler_by_path.size(); ++idx)
    {
        std::complex<global_nms::default_data_type> phase {0.0, -2.0*M_PI*(acc_phases[idx] + (t*std::get<1>(this->Rabi_freq_Doppler_by_path[idx])))};
        Rabi_freq += std::get<0>(this->Rabi_freq_Doppler_by_path[idx])*std::exp(phase);
    }
    
    auto wP_decay {this->prob_decay_rate*(x[P] + x[w])/2.0};
    auto uv_decay_coeff {(this->prob_decay_rate + (2.0*M_PI*illumination_handler_base<IT, EM_Quant_Gen>::FWHM))/2.0};
    
    dxdt[u] = -(Rabi_freq.imag()*x[w]) - (uv_decay_coeff*x[u]);
    dxdt[v] = -(Rabi_freq.real()*x[w]) - (uv_decay_coeff*x[v]);
    dxdt[w] = (Rabi_freq.real()*x[v]) + (Rabi_freq.imag()*x[u]) - wP_decay;
    dxdt[P] = -wP_decay;
}

/*
 This function should be run for each kinematic step while in view of the laser
 or in the excited state.
 */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
template <class Func>
inline double ill_hand_nms::illumination_handler<IT, EM_Quant_Gen>::integration_step(const pdef_nms::state_t& pos_vel, double dt, Func&& observer)
{
    this->update(pos_vel);
    
    auto loc_decay_threshold {decay_threshold};
    double dt_executed {dt};
    auto mod_observer
    {
        [&dt_executed, loc_decay_threshold, &observer](const d_mat_t& x, double t)
        {
            observer(x, t);
            if((x[P] < loc_decay_threshold) && (t < dt_executed)) {dt_executed = t;}
        }
    };
    boost::numeric::odeint::integrate(std::ref(*this), d_mat, 0.0, dt, dt, mod_observer);
    
    for(unsigned idx = 0; idx < acc_phases.size(); ++idx) {
        acc_phases[idx] += dt_executed*std::get<1>(this->Rabi_freq_Doppler_by_path[idx]);
    }
    
    return dt_executed;
}

#endif /* ILLUMINATION_HANDLER_HPP */

