#ifndef ILLUMINATION_ARRAY_HANDLER_H
#define ILLUMINATION_ARRAY_HANDLER_H

#include "Illumination_Handler.h"

namespace ill_arr_hand_nms
{
    template <pdef_nms::illum_type IT, class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct illumination_array_handler :
    ill_hand_nms::illumination_handler_base<IT, EM_Quant_Gen>
    {
        using d_mat_t = std::array<std::complex<global_nms::default_data_type>, 2>;
        enum dmat_comp {ce, cg};
        
        size_t beam_passes;
        double t_in_beam;
        
        std::vector<int> detunings;
        std::vector<std::complex<global_nms::default_data_type>> dets_2pi;
        
        size_t active_members;
        std::vector<bool> active;
        
        std::vector<d_mat_t> d_mat_vec;
        
        std::vector<global_nms::default_data_type> decay_threshold_vec;
        std::vector<global_nms::default_data_type> excitation_threshold_vec;
        
        std::vector<size_t> decaying_idxs;
        
        void fill(const boost::filesystem::path&, const std::array<int, 3>&);
        
        void initialize(bool = true);
        void integration_step_elem(std::complex<global_nms::default_data_type>, std::complex<global_nms::default_data_type>, std::complex<global_nms::default_data_type>, std::complex<global_nms::default_data_type>, size_t);
        
        void check_for_excitation();
        
        global_nms::default_data_type P(size_t idx) {
            return (std::norm(d_mat_vec[idx][ce]) + std::norm(d_mat_vec[idx][cg]));
        }
    };
}

#include "Template_Impl/Illumination_Array_Handler.hpp"

#endif /* ILLUMINATION_ARRAY_HANDLER_H */

