#ifndef BIOT_SAVART_LOOP_H
#define BIOT_SAVART_LOOP_H

#include "ATRAP_base.h"
#include "Coord_Grid.h"
#include "Common_Grid_Quantities.h"

namespace bsl_nms
{
    /* Exception for functions in this header */
    class bsl_exc {
    public:
        bsl_exc(std::string& msg): err_message{msg} {}
        bsl_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class T>
    std::vector<bvec_nms::basic_vec<T>> biot_savart_loop_pt(bvec_nms::basic_vec<T> r, T z0, T a);
    
    template <class T>
    std::vector<bvec_nms::basic_vec<T>> biot_savart_grad_loop_pt(bvec_nms::basic_vec<T> r, T z0, T a);
    
    template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
    void biot_savart_loop(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld, global_nms::default_data_type z0, global_nms::default_data_type a);
    
    std::vector<global_nms::default_data_type> gen_mirror_coil_params(global_nms::default_data_type min, global_nms::default_data_type max, size_t turns);
    
    template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
    void biot_savart_pinch(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld);
    
    template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
    void biot_savart_buck(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld);
}

#include "Template_Impl/Biot_Savart/Biot_Savart_Loop.hpp"

#endif /* BIOT_SAVART_LOOP_H */
