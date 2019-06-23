#ifndef BIOT_SAVART_H
#define BIOT_SAVART_H

#include "ATRAP_base.h"
#include "Coord_Grid.h"
#include "Common_Grid_Quantities.h"

namespace bs_nms
{
    /* Exception for functions in this header */
    class bs_exc {
    public:
        bs_exc(std::string& msg): err_message{msg} {}
        bs_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
    void biot_savart(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>&);
}

#include "Template_Impl/Biot_Savart/Biot_Savart.hpp"

#endif /* BIOT_SAVART_H */
