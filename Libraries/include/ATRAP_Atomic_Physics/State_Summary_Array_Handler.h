#ifndef STATE_SUMMARY_ARRAY_HANDLER_H
#define STATE_SUMMARY_ARRAY_HANDLER_H

#include <iostream>
#include "State_Summary.h"

namespace st_sum_arr_hand_nms
{
    struct st_sum_arr_hand
    {
        std::vector<st_sum_nms::state_summary> st_sum_arr;
        bool out_of_bounds;
        
        void initialize(size_t n)
        {
            st_sum_arr.resize(n);
            out_of_bounds = false;
        }
        
        template <class KH_T, class Illum_T, class Decay_T>
        void add_elem(size_t idx, KH_T& kh, Illum_T& ill, Decay_T& dec)
        {
            st_sum_nms::state_summary& ss {st_sum_arr[idx]};
            
            if(kh.t < kh.t_lim) {std::cout << "Particle " << idx << " lost at " << kh.t << std::endl;}
            else {std::cout << "Particle " << idx << " finishing" << std::endl;}
            
            if(kh.out_of_bounds())
            {
                kh.pos_vel.first = kh.out_of_bounds_position();
                kh.reset_out_of_bnds();
                out_of_bounds = true;
            }
            
            ss.pos_vel = kh.pos_vel;
            ss.t_end = kh.t;
            ss.E = kh.eh.get_gr_E(kh.pos_vel);
            
            ss.N = 1;
            ss.F = dec.decay_st_vec[idx]("F");
            ss.mF = dec.decay_st_vec[idx]("mF");
            
            ss.one_photon_decays = dec.one_ph_decays_vec[idx];
            ss.two_photon_decays = dec.two_ph_decays_vec[idx];
            ss.ionized = dec.ionized_vec[idx];
            
            ss.out_of_bounds = out_of_bounds;
            
            ss.beam_passes = ill.beam_passes;
            ss.t_in_beam = ill.t_in_beam;
        }
        
        template <class KH_T, class Illum_T, class Decay_T>
        void finish_st_sum_arr(KH_T& kh, Illum_T& ill, Decay_T& dec)
        {
            for(size_t idx = 0; idx < ill.active.size(); ++idx) {
                if(ill.active[idx]) {add_elem(idx, kh, ill, dec);}
            }
        }
    };
}

#endif /* STATE_SUMMARY_ARRAY_HANDLER_H */

