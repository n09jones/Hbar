#ifndef ENERGY_HANDLER_HPP
#define ENERGY_HANDLER_HPP

template <class EM_Quant_Gen>
void en_hand_nms::energy_handler<EM_Quant_Gen>::fill(const boost::filesystem::path& opts_fn)
{
    this->initialize(opts_fn);
    gr_state.fill(opts_fn);
    gr_E.fill("Gr_Energy", gr_state);
}

template <class EM_Quant_Gen>
global_nms::default_data_type en_hand_nms::energy_handler<EM_Quant_Gen>::get_gr_E(const pdef_nms::state_t& pos_vel)
{
    std::array<global_nms::default_data_type, 1> tmp_Bnorm {{0.0}};
    this->template EM_quant<ig_nms::interp_quant::Bnorm>(tmp_Bnorm, pos_vel.first);
    return ((0.5*(pvsi_nms::mass_p + pvsi_nms::mass_e)*norm_sq(pos_vel.second)) + (gr_E(tmp_Bnorm[0]) - gr_E(this->Bnorm_min)))/pvsi_nms::k_B;
}

#endif /* ENERGY_HANDLER_HPP */

