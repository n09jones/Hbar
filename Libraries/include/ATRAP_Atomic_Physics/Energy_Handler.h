#ifndef ENERGY_HANDLER_H
#define ENERGY_HANDLER_H

#include "ATRAP_base.h"
#include "Prop_Defs.h"
#include "Atomic_States.h"
#include "EM_Quants.h"
#include "TQ_Array_Manager.h"

namespace en_hand_nms
{
    template <class EM_Quant_Gen = emq_nms::grid_EM_quants<>>
    struct energy_handler :
    EM_Quant_Gen
    {
        asts_nms::S_gr gr_state;
        
        TQam_nms::arr_mngr<> gr_E;
        
        void fill(const boost::filesystem::path&);
        global_nms::default_data_type get_gr_E(const pdef_nms::state_t&);
    };
}

#include "Template_Impl/Energy_Handler.hpp"

#endif /* ENERGY_HANDLER_H */

