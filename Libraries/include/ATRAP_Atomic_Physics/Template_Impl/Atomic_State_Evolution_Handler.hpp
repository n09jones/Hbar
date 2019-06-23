#ifndef ATOMIC_STATE_EVOLUTION_HANDLER_HPP
#define ATOMIC_STATE_EVOLUTION_HANDLER_HPP

template <class Illum_T, class Decay_T>
void ase_hand_nms::atomic_state_evolution_handler<Illum_T, Decay_T>::fill(const boost::filesystem::path& opts_fn)
{
    ill.fill(opts_fn);
    dec.fill(opts_fn);
    
    gqt_nms::fill_params(opts_fn, *this, val_str);
    
    dt = dt_nrm;
    for(const auto& path : ill.paths) {dt = std::min(dt, (path.waist/100.0)/100.0);}
    
    if(ill.pulsed) {dt = ill.pulse_len;}
    
    buffer = 150.0*std::max(dt_nrm, dt);
}

template <class Illum_T, class Decay_T>
std::vector<pid_nms::param_info> ase_hand_nms::atomic_state_evolution_handler<Illum_T, Decay_T>::get_param_info()
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

template <class Illum_T, class Decay_T>
void ase_hand_nms::atomic_state_evolution_handler<Illum_T, Decay_T>::post_read_ops(boost::program_options::variables_map)
{
    if(dt_nrm <= 0) {throw ase_hand_exc{"Time per step must be positive"};}
}

template <class Illum_T, class Decay_T>
inline ase_hand_nms::scatt_status ase_hand_nms::atomic_state_evolution_handler<Illum_T, Decay_T>::do_step(pdef_nms::state_t& pos_vel)
{
    double tmp_dt {dt};
    while(true)
    {
        tmp_dt -= ill.integration_step(pos_vel, tmp_dt);
        if(tmp_dt < (1.e-9)*dt) {return scatt_status::in_cycle;}
        dec.execute_decay_chain(pos_vel);
        pos_vel.second += dec.vel_kick;
        if(dec.ionized || (dec.decay_st != ill.gr_state)) {return scatt_status::out_cycle;}
        ill.initialize();
    }
}

#endif /* ATOMIC_STATE_EVOLUTION_HANDLER_HPP */

