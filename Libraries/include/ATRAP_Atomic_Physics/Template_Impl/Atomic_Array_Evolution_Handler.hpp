#ifndef ATOMIC_ARRAY_EVOLUTION_HANDLER_HPP
#define ATOMIC_ARRAY_EVOLUTION_HANDLER_HPP

size_t prop_num {0};
size_t poss_num {0};

template <class Illum_T, class Decay_T, class SSAH_T>
void aae_hand_nms::atomic_array_evolution_handler<Illum_T, Decay_T, SSAH_T>::fill(const boost::filesystem::path& opts_fn, const std::array<int, 3>& det_lims)
{
    ill.fill(opts_fn, det_lims);
    dec.fill(opts_fn, ill.detunings.size());
    ssah.initialize(ill.detunings.size());
    
    gqt_nms::fill_params(opts_fn, *this, val_str);
    
    dt = dt_nrm;
    for(const auto& path : ill.paths) {dt = std::min(dt, (path.waist/100.0)/100.0);}
    
    if(ill.pulsed) {dt = ill.pulse_len;}
    
    buffer = 150.0*std::max(dt_nrm, dt);
}

template <class Illum_T, class Decay_T, class SSAH_T>
std::vector<pid_nms::param_info> aae_hand_nms::atomic_array_evolution_handler<Illum_T, Decay_T, SSAH_T>::get_param_info()
{
    return std::vector<pid_nms::param_info>
    {
        {
            "Time Per Step",
            "In seconds",
            gqt_nms::fnc_true,
            [this](std::string str){gqt_nms::str_to_data(str, &(this->dt_nrm));}
        }
    };
}

template <class Illum_T, class Decay_T, class SSAH_T>
void aae_hand_nms::atomic_array_evolution_handler<Illum_T, Decay_T, SSAH_T>::post_read_ops(boost::program_options::variables_map)
{
    if(dt_nrm <= 0) {throw aae_hand_exc{"Time per step must be positive"};}
}

template <class Illum_T, class Decay_T, class SSAH_T>
template <ig_nms::interp_quant IQ, class KH_T>
inline void aae_hand_nms::atomic_array_evolution_handler<Illum_T, Decay_T, SSAH_T>::do_steps_in_beam(KH_T& kh)
{
    build_params_arr<IQ>(kh);
    std::vector<size_t> idxs_to_evolve;
    
    for(size_t idx = 0; idx < ill.detunings.size(); ++idx)
    {
        auto pee {std::norm(ill.d_mat_vec[idx][0])};
        auto pgg {std::norm(ill.d_mat_vec[idx][1])};
        
        vec_t apP_vec_i {2.0*std::sqrt(pee*pgg), pee, (pgg + pee)};
        vec_t apP_vec_f {bds_b};
        
        for(int k = 0; k < 3; ++k) {
            for (int l = 0; l < 3; ++l) {
                apP_vec_f[k] += bds_A[k][l]*apP_vec_i[l];
            }
        }
        
        if(ill.active[idx] && ((ill.decay_threshold_vec[idx] > apP_vec_f[2]) || ((apP_vec_f[2] > 0.0) && (apP_vec_f[1] > ill.excitation_threshold_vec[idx]*apP_vec_f[2])))) {
            idxs_to_evolve.push_back(idx);
        }
    }
//    std::cout << "Indexes to evolve : " << idxs_to_evolve.size() << std::endl;

    prop_num += idxs_to_evolve.size();
    poss_num += ill.detunings.size();

    if(idxs_to_evolve.empty())
    {
        kh.t = t_f;
        kh.pos_vel = pos_vel_f;
    }
    
    for(; kh.t < t_f; kh.t += kh.dt)
    {
        ill.t_in_beam += kh.dt;
        
        ill.update(kh.pos_vel);
        
        auto Gamma {ill.prob_decay_rate};
        auto Omega {std::get<0>(ill.Rabi_freq_Doppler_by_path[0])};
        auto delta_0 {2.0*M_PI*std::get<1>(ill.Rabi_freq_Doppler_by_path[0])};
        
        for(size_t idx : idxs_to_evolve) {
            if(ill.active[idx])
            {
                ill.integration_step_elem(Gamma, Omega, delta_0 + ill.dets_2pi[idx], kh.dt, idx);
                if(ill.P(idx) < ill.decay_threshold_vec[idx]) {ill.decaying_idxs.push_back(idx);}
            }
        }
        dec.execute_vec_decay(kh, ill, ssah);
        
        kh.template do_step<IQ>();
    }

    kh.dt = dt_nrm;
// std::cout << "Done evolving" << std::endl << std::endl;
}

template <class Illum_T, class Decay_T, class SSAH_T>
template <ig_nms::interp_quant IQ, class KH_T>
inline void aae_hand_nms::atomic_array_evolution_handler<Illum_T, Decay_T, SSAH_T>::do_steps_decay(KH_T& kh)
{
    std::vector<size_t> idxs_to_evolve;
    std::vector<bool> has_decayed;
    
    idxs_to_evolve = ill.decaying_idxs;
    ill.decaying_idxs.clear();
//    std::cout << "Indexes to decay : " << idxs_to_evolve.size() << std::endl;
    
    has_decayed.assign(ill.detunings.size(), false);
    size_t undecayed {idxs_to_evolve.size()};
    
    for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && !(this->in_beam(kh.pos_vel)) && undecayed); kh.t += kh.dt)
    {
        ill.update(kh.pos_vel);
        
        auto Gamma {ill.prob_decay_rate};
        auto Omega {std::get<0>(ill.Rabi_freq_Doppler_by_path[0])};
        auto delta_0 {2.0*M_PI*std::get<1>(ill.Rabi_freq_Doppler_by_path[0])};
        
        for(size_t idx : idxs_to_evolve)
        {
            if(!(has_decayed[idx]))
            {
                ill.integration_step_elem(Gamma, Omega, delta_0 + ill.dets_2pi[idx], kh.dt, idx);
                if(ill.P(idx) < ill.decay_threshold_vec[idx])
                {
                    ill.decaying_idxs.push_back(idx);
                    has_decayed[idx] = true;
                    --undecayed;
                }
            }
        }
        dec.execute_vec_decay(kh, ill, ssah);
        
        kh.template do_step<IQ>();
    }
    
//    std::cout << "Done decaying" << std::endl << std::endl;
}

template <class Illum_T, class Decay_T, class SSAH_T>
template <class KH_T>
inline void aae_hand_nms::atomic_array_evolution_handler<Illum_T, Decay_T, SSAH_T>::finish_and_add_to_final_states(KH_T& kh)
{
    ssah.finish_st_sum_arr(kh, ill, dec);
    kh.final_states.insert(kh.final_states.end(), ssah.st_sum_arr.begin(), ssah.st_sum_arr.end());
}

template <class Illum_T, class Decay_T, class SSAH_T>
template <ig_nms::interp_quant IQ, class KH_T>
inline void aae_hand_nms::atomic_array_evolution_handler<Illum_T, Decay_T, SSAH_T>::build_params_arr(KH_T& kh)
{
    kh.dt = dt;
    t_i = kh.t;
    pos_vel_i = kh.pos_vel;

    bds_A = {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}};
    bds_b = {0.0, 0.0, 0.0};
    
    mat_t tmp_bds_A;
    vec_t tmp_bds_b;
    
    mat_t tmp_bds_A_pr;

    for(; ((kh.t < kh.t_lim) && !(kh.out_of_bounds()) && (this->in_beam(kh.pos_vel))); kh.t += kh.dt)
    {
        ill.update(kh.pos_vel);
        
        auto Gamma {ill.prob_decay_rate};
        auto Omega {std::abs(std::get<0>(ill.Rabi_freq_Doppler_by_path[0]))};

        auto O_G {Omega/Gamma};
        auto exp_fact {std::exp(-Gamma*(kh.dt)/2.0)};

        tmp_bds_A[0] = {exp_fact, 0.0, 0.0};
        tmp_bds_b[0] = (2.0*O_G*(1.0 - exp_fact));
        
        tmp_bds_A[1] = {(1.0 - exp_fact)*O_G*exp_fact, exp_fact*exp_fact, 0.0};
        tmp_bds_b[1] = O_G*O_G*(1.0 + ((exp_fact - 2.0)*exp_fact));
        
        tmp_bds_A[2] = {O_G*((2.0 - exp_fact)*exp_fact - 1.0), ((exp_fact*exp_fact) - 1.0), 1.0};
        tmp_bds_b[2] = O_G*O_G*(((exp_fact - 4.0)*exp_fact) + 3.0 - Gamma*(kh.dt));
        
        
        for(int k = 0; k < 3; ++k)
        {
            for(int l = 0; l < 3; ++l)
            {
                tmp_bds_A_pr[k][l] = 0.0;
                for(int m = 0; m < 3; ++m) {tmp_bds_A_pr[k][l] += tmp_bds_A[k][m]*bds_A[m][l];}
                
                tmp_bds_b[k] += tmp_bds_A[k][l]*bds_b[l];
            }
        }
        
        bds_b = tmp_bds_b;
        bds_A = tmp_bds_A_pr;
        
        kh.template do_step<IQ>();
    }

    t_f = kh.t;
    pos_vel_f = kh.pos_vel;
    
    kh.reset_out_of_bnds();
    kh.t = t_i;
    kh.pos_vel = pos_vel_i;
}

#endif /* ATOMIC_ARRAY_EVOLUTION_HANDLER_HPP */

