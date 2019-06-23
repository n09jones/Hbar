#ifndef DECAY_HANDLER_HPP
#define DECAY_HANDLER_HPP

#include "Random_Distributions.h"

/*
 Choose an element of br_choices randomly, weighting choice x by func(x).
 Note that the weights should sum to 1.
 */
template <class U, class Func>
U decay_nms::pick(const std::vector<U>& br_choices, Func&& func)
{
    auto rand_num {distr_gen::rand()};
    
    for(const auto& br_choice : br_choices) {
        rand_num -= func(br_choice);
        if(rand_num < 0) {return br_choice;}
    }
    
    for(auto rit = br_choices.crbegin(); rit != br_choices.crend(); ++rit) {
        if((rand_num < 1.e-6) && func(*rit)) {return (*rit);}
    }
    
    throw decay_nms::decay_exc{"Branching ratios should sum to 1"};
}


/* decay_handler functions*/
/* Fill decay_handler */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void decay_nms::decay_handler<IT, EM_Quant_Gen>::fill(const boost::filesystem::path& opts_fn)
{
    this->initialize(opts_fn);
    this->fill_path_mngr(opts_fn);
    
    decay_st.fill(opts_fn);
    ex_state.fill(opts_fn);
    
    one_ph_decays = 0;
    two_ph_decays = 0;
    ionized = false;
    
    vel_kick = {0, 0, 0};
    
    decay_rt_1ph_fact.fill("Decay_Rate_1Ph", ex_state);
    
    if(pdef_nms::is_TwoPh<IT>)
    {
        decay_rt_2ph_fact.fill("Decay_Rate_2Ph", ex_state);
        ion_rt_fact.fill("Ionization_Rate", ex_state);
    }
    
    trans_freqs.clear();
    br_ratios_1ph.clear();
    DPM_coeffs_1ph.clear();
    for(const auto& Sket : asts_nms::S_gr::basis())
    {
        trans_freqs[Sket].fill("Transition_Freq", ex_state, Sket);
        br_ratios_1ph[Sket].fill("Branching_1Ph", ex_state, Sket);
        DPM_coeffs_1ph[Sket].fill("DPM_1Ph", ex_state, Sket);
    }
    
    if(pdef_nms::is_TwoPh<IT>)
    {
        br_ratios_2ph.clear();
        DPM_coeffs_2ph.clear();
        for(const auto& Sket : asts_nms::S_gr::basis())
        {
            br_ratios_2ph[Sket].fill("Branching_2Ph", ex_state, Sket);
            DPM_coeffs_2ph[Sket].fill("DPM_2Ph", ex_state, Sket);
        }
        
        two_ph_E_distr.fill("TwoPhoton_Energy_Distribution");
    }
}

/* Update decay_handler */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void decay_nms::decay_handler<IT, EM_Quant_Gen>::update(const pdef_nms::state_t& pos_vel)
{
    this->load_for_illumination(pos_vel);
    
    decay_rt_1ph_fact.update_quant(this->Bnorm);
    
    if(pdef_nms::is_TwoPh<IT>)
    {
        decay_rt_2ph_fact.update_quant(this->Bnorm);
        ion_rt_fact.update_quant(this->Bnorm);
    }
    
    for(const auto& Sket : asts_nms::S_gr::basis())
    {
        trans_freqs.at(Sket).update_quant(this->Bnorm);
        br_ratios_1ph.at(Sket).update_quant(this->Bnorm);
        DPM_coeffs_1ph.at(Sket).update_quant(this->Bnorm);
        
        if(pdef_nms::is_TwoPh<IT>)
        {
            br_ratios_2ph.at(Sket).update_quant(this->Bnorm);
            DPM_coeffs_2ph.at(Sket).update_quant(this->Bnorm);
        }
    }
    
    update_derived_quants(pos_vel);
}

/* Update the derived quantities in decay_handler */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void decay_nms::decay_handler<IT, EM_Quant_Gen>::update_derived_quants(const pdef_nms::state_t& pos_vel)
{
    using namespace pvsi_nms;
    
    vel_kick_mags.clear();
    for(const auto& Sket : asts_nms::S_gr::basis()) {
        vel_kick_mags[Sket] = h*trans_freqs.at(Sket)()/((mass_e + mass_p)*c);
    }
    
    if(pdef_nms::is_LyAlph<IT>)
    {
        br_ratios = {{decay_ch::dec_1ph, 1}, {decay_ch::dec_2ph, 0}, {decay_ch::dec_ion, 0}};
    }
    else
    {
        global_nms::default_data_type dr_1ph {decay_rt_1ph_fact()*(this->E_sq)};
        global_nms::default_data_type dr_2ph {decay_rt_2ph_fact()};
        
        global_nms::default_data_type dr_ion {0};
        for(const auto& path : this->paths)
        {
            this->load_per_path(pos_vel, path);
            dr_ion += ion_rt_fact()*(this->intensity);
        }
        
        global_nms::default_data_type dr_tot {dr_1ph + dr_2ph + dr_ion};
        
        br_ratios =
        {
            {decay_ch::dec_1ph, dr_1ph/dr_tot},
            {decay_ch::dec_2ph, dr_2ph/dr_tot},
            {decay_ch::dec_ion, dr_ion/dr_tot}
        };
    }
}

/* Choose the decay channel and modify internal variables consistent with the chosen decay. */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void decay_nms::decay_handler<IT, EM_Quant_Gen>::execute_decay_chain(const pdef_nms::state_t& pos_vel)
{
    update(pos_vel);
    
    switch (choose_channel())
    {
        case decay_ch::dec_1ph :
            ++one_ph_decays;
            execute_one_ph_decay(pos_vel);
            break;
            
        case decay_ch::dec_2ph :
            ++two_ph_decays;
            execute_two_ph_decay();
            break;
            
        case decay_ch::dec_ion :
            ionized = true;
            vel_kick = {0, 0, 0};
            break;
    }
}

/* Choose the decay channel. */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
decay_nms::decay_ch decay_nms::decay_handler<IT, EM_Quant_Gen>::choose_channel()
{
    std::vector<decay_ch> ch_choices {decay_ch::dec_1ph, decay_ch::dec_2ph, decay_ch::dec_ion};
    auto ch_br_prob {[this](auto ch) {return (this->br_ratios).at(ch);}};
    
    return pick(ch_choices, ch_br_prob);
}

/* */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void decay_nms::decay_handler<IT, EM_Quant_Gen>::execute_one_ph_decay(const pdef_nms::state_t& pos_vel)
{
    auto ket_br_prob {[this](auto k) {return (this->br_ratios_1ph).at(k)();}};
    decay_st = pick(asts_nms::S_gr::basis(), ket_br_prob);

    auto rot_vel_kick {vel_kick_mags.at(decay_st)*norm_em_recoil(DPM_coeffs_1ph.at(decay_st)())};
    vel_kick = rotb_nms::gen_rot_base(this->B, this->E).rot_to_abs_basis(rot_vel_kick);
    
    if(pdef_nms::is_LyAlph<IT>)
    {
        std::vector<size_t> path_choices;
        global_nms::default_data_type cum_intensity {0.0};
        std::map<size_t, global_nms::default_data_type> intensity_weights;
        for(size_t idx = 0; idx < (this->paths).size(); ++idx)
        {
            path_choices.push_back(idx);
            
            this->load_per_path(pos_vel, (this->paths)[idx]);
            cum_intensity += (this->intensity);
            intensity_weights[idx] = (this->intensity);
        }
        for(auto& elem : intensity_weights) {elem.second /= cum_intensity;}
        
        auto path_choice_func {[intensity_weights](auto idx) {return intensity_weights.at(idx);}};
        size_t abs_path_idx {pick(path_choices, path_choice_func)};
        
        vel_kick += vel_kick_mags.at(decay_st)*(this->paths)[abs_path_idx].unit_wave_vector;
    }
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void decay_nms::decay_handler<IT, EM_Quant_Gen>::execute_two_ph_decay()
{
    auto ket_br_prob {[this](auto k) {return (this->br_ratios_2ph).at(k)();}};
    decay_st = pick(asts_nms::S_gr::basis(), ket_br_prob);

    bvec_nms::basic_vec<> rot_norm_ph_1_kick {0, 0, -1};
    bvec_nms::basic_vec<> rot_norm_ph_2_kick {norm_em_recoil(DPM_coeffs_2ph.at(decay_st)())};
    
    auto ph_E_distr_func {[this](auto x) {return (this->two_ph_E_distr)(x[0]);}};
    auto ph_1_E_frac {distr_gen::rand<1>(ph_E_distr_func, {{{{0, 1}}}}, 1)[0]};
    auto ph_2_E_frac {1 - ph_1_E_frac};
    
    auto rot_vel_kick {
        vel_kick_mags.at(decay_st)*((ph_1_E_frac*rot_norm_ph_1_kick) + (ph_2_E_frac*rot_norm_ph_2_kick))
    };
    
    auto theta_phi_distr {[](auto x) {return std::sin(x[0]);}};
    auto ph_1_khat {theta_phi_vec(distr_gen::rand<2>(theta_phi_distr, {{{{0, M_PI}}, {{0, 2*M_PI}}}}, 1))};
    
    vel_kick = rotb_nms::gen_rot_base(ph_1_khat).rot_to_abs_basis(rot_vel_kick);
}


#endif /* DECAY_HANDLER_HPP */

