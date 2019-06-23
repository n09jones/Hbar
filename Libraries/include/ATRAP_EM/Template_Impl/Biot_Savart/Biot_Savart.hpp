#ifndef BIOT_SAVART_HPP
#define BIOT_SAVART_HPP

#include "Biot_Savart/Biot_Savart_Seg.h"
#include "Biot_Savart/Biot_Savart_Loop.h"

template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
void bs_nms::biot_savart(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld)
{
    switch (fld.in_params.coil_nm)
    {
        case fp_nms::coil::quad :
        case fp_nms::coil::oct :
        case fp_nms::coil::custom :
            bss_nms::biot_savart_seg<Calc_T, Rslt_T, Grid_Type>(fld);
            break;
            
        case fp_nms::coil::pinch :
            bsl_nms::biot_savart_pinch<Calc_T, Rslt_T, Grid_Type>(fld);
            break;
            
        case fp_nms::coil::buck :
            bsl_nms::biot_savart_buck<Calc_T, Rslt_T, Grid_Type>(fld);
            break;
    }
}

#endif /* BIOT_SAVART_HPP */
