#ifndef DECAY_ARRAY_HANDLER_H
#define DECAY_ARRAY_HANDLER_H

#include "Decay_Handler.h"

namespace decay_arr_nms
{
    template <pdef_nms::illum_type IT, class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct decay_array_handler :
    decay_nms::decay_handler<IT, EM_Quant_Gen>
    {
        std::vector<size_t> one_ph_decays_vec;
        std::vector<size_t> two_ph_decays_vec;
        std::vector<bool> ionized_vec;
        
        std::vector<asts_nms::S_gr> decay_st_vec;
        
        void fill(const boost::filesystem::path&, size_t);
        
        template<class KH_T, class Illum_T, class SSAH_T>
        void execute_vec_decay(KH_T&, Illum_T&, SSAH_T&);
    };
}

#include "Template_Impl/Decay_Array_Handler.hpp"

#endif /* DECAY_ARRAY_HANDLER_H */

