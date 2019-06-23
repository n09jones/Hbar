#ifndef DECAY_ARRAY_HANDLER_HPP
#define DECAY_ARRAY_HANDLER_HPP

#include "Random_Distributions.h"

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void decay_arr_nms::decay_array_handler<IT, EM_Quant_Gen>::fill(const boost::filesystem::path& opts_fn, size_t det_ct)
{
    decay_nms::decay_handler<IT, EM_Quant_Gen>::fill(opts_fn);
    
    this->one_ph_decays_vec.assign(det_ct, 0);
    this->two_ph_decays_vec.assign(det_ct, 0);
    this->ionized_vec.assign(det_ct, false);
    
    this->decay_st_vec.assign(det_ct, decay_nms::decay_handler<IT, EM_Quant_Gen>::decay_st);
}


template <pdef_nms::illum_type IT, class EM_Quant_Gen>
template <class KH_T, class Illum_T, class SSAH_T>
void decay_arr_nms::decay_array_handler<IT, EM_Quant_Gen>::execute_vec_decay(KH_T& kh, Illum_T& ill, SSAH_T& ssah)
{
    if(ill.decaying_idxs.empty()) {return;}
    
    this->update(kh.pos_vel);
    
    auto ket_br_prob_1ph {[this](auto k) {return (this->br_ratios_1ph).at(k)();}};
    auto ket_br_prob_2ph {[this](auto k) {return (this->br_ratios_2ph).at(k)();}};
    
    for(size_t idx : ill.decaying_idxs)
    {
        switch (this->choose_channel())
        {
            case decay_nms::decay_ch::dec_1ph :
                ++(one_ph_decays_vec[idx]);
                decay_st_vec[idx] = decay_nms::pick(asts_nms::S_gr::basis(), ket_br_prob_1ph);
                break;
                
            case decay_nms::decay_ch::dec_2ph :
                ++(two_ph_decays_vec[idx]);
                decay_st_vec[idx] = decay_nms::pick(asts_nms::S_gr::basis(), ket_br_prob_2ph);
                break;
                
            case decay_nms::decay_ch::dec_ion :
                ionized_vec[idx] = true;
                break;
        }

        if(ionized_vec[idx] || (decay_st_vec[idx] != ill.gr_state))
        {
            ill.active[idx] = false;
            --(ill.active_members);
            ssah.add_elem(idx, kh, ill, *this);
        }

	ill.d_mat_vec[idx] = {{0.0, 1.0}};
        ill.decay_threshold_vec[idx] = distr_gen::rand();
        ill.excitation_threshold_vec[idx] = distr_gen::rand();
    }
    
    ill.decaying_idxs.clear();
}


#endif /* DECAY_ARRAY_HANDLER_HPP */
