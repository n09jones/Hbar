#ifndef BIOT_SAVART_SEG_H
#define BIOT_SAVART_SEG_H

#include "ATRAP_base.h"
#include "Coord_Grid.h"
#include "Common_Grid_Quantities.h"

namespace bss_nms
{
    /* Exception for functions in this header */
    class bss_exc {
    public:
        bss_exc(std::string& msg): err_message{msg} {}
        bss_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
    std::array<mem_arr_vec_nms::mem_arr_vec<Calc_T>, 2> biot_savart_coil_prep(const cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>&);
    
    template <class T>
    std::vector<bvec_nms::basic_vec<T>> biot_savart_seg_half_pt(const mem_arr_vec_nms::mem_arr_vec<T>&, const mem_arr_nms::mem_arr<T>&, const mem_arr_nms::mem_arr<T>&, const mem_arr_vec_nms::mem_arr_vec<T>&, const mem_arr_nms::mem_arr<T>&, size_t);
    
    template <class T>
    std::vector<bvec_nms::basic_vec<T>> biot_savart_grad_seg_half_pt(const mem_arr_vec_nms::mem_arr_vec<T>&, const mem_arr_nms::mem_arr<T>&, const mem_arr_nms::mem_arr<T>&, const mem_arr_vec_nms::mem_arr_vec<T>&, const mem_arr_nms::mem_arr<T>&, size_t);
    
    template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
    void biot_savart_seg(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>&);
}

#include "Template_Impl/Biot_Savart/Biot_Savart_Seg.hpp"

#endif /* BIOT_SAVART_SEG_H */
