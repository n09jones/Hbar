#ifndef BIOT_SAVART_SEGS_HPP
#define BIOT_SAVART_SEGS_HPP

#include <numeric>
#include "Coil_Vec_Conversion.h"

template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
std::array<mem_arr_vec_nms::mem_arr_vec<Calc_T>, 2> bss_nms::biot_savart_coil_prep(const cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld)
{
    mem_arr_nms::mem_arr<> raw_coil {fld.in_params.coil_fn};
    mem_arr_vec_nms::mem_arr_vec<Calc_T> coil_vec {mem_arr_vec_nms::convert<Calc_T>(cvc_nms::coil_to_vec(raw_coil))};
    mem_arr_vec_nms::mem_arr_vec<Calc_T> segs {coil_vec.arr_len()};
    
    for(unsigned comp = 0; comp < 3; ++comp)
    {
        if(coil_vec[comp][0] != coil_vec[comp][coil_vec.arr_len() - 1]) {
            throw bss_exc{"Coils must make a closed loop"};
        }
        std::adjacent_difference(coil_vec[comp].begin(), coil_vec[comp].end(), segs[comp].begin());
        segs[comp][0] = segs[comp][segs.arr_len() - 1];
    }
    
    return std::array<mem_arr_vec_nms::mem_arr_vec<Calc_T>, 2>{{std::move(coil_vec), std::move(segs)}};
}


template <class T>
std::vector<bvec_nms::basic_vec<T>> bss_nms::biot_savart_seg_half_pt(const mem_arr_vec_nms::mem_arr_vec<T>& r, const mem_arr_nms::mem_arr<T>& r_sq, const mem_arr_nms::mem_arr<T>& r_norm, const mem_arr_vec_nms::mem_arr_vec<T>& seg, const mem_arr_nms::mem_arr<T>& seg_sq, size_t offset)
{
    std::vector<bvec_nms::basic_vec<T>> ret_vec(1, bvec_nms::basic_vec<T>{0, 0, 0});
    
    for (size_t idx = 0; idx < r_sq.size() - 1; ++idx)
    {
        bvec_nms::basic_vec<T> r_loc {r.slice(idx)};
        T r_sq_loc {r_sq[idx]};
        T r_norm_loc {r_norm[idx]};
        bvec_nms::basic_vec<T> seg_loc {seg.slice(idx + offset)};
        T seg_sq_loc {seg_sq[idx + offset]};
        
        T seg_dot_r {inner_pr(seg_loc, r_loc)};
        T A {((seg_sq_loc*r_sq_loc) - (seg_dot_r*seg_dot_r))*r_norm_loc};
        bvec_nms::basic_vec<T> cr_pr {cross_pr(seg_loc, r_loc)/A};
        
        ret_vec[0] += cr_pr*seg_dot_r;
    }
    
    for(auto& vec: ret_vec) {vec *= 1.0e-7;}
    return ret_vec;
}

template <class T>
std::vector<bvec_nms::basic_vec<T>> bss_nms::biot_savart_grad_seg_half_pt(const mem_arr_vec_nms::mem_arr_vec<T>& r, const mem_arr_nms::mem_arr<T>& r_sq, const mem_arr_nms::mem_arr<T>& r_norm, const mem_arr_vec_nms::mem_arr_vec<T>& seg, const mem_arr_nms::mem_arr<T>& seg_sq, size_t offset)
{
    std::vector<bvec_nms::basic_vec<T>> ret_vec(4, bvec_nms::basic_vec<T>{0, 0, 0});
    
    for (size_t idx = 0; idx < r_sq.size() - 1; ++idx)
    {
        bvec_nms::basic_vec<T> r_loc {r.slice(idx)};
        T r_sq_loc {r_sq[idx]};
        T r_norm_loc {r_norm[idx]};
        bvec_nms::basic_vec<T> seg_loc {seg.slice(idx + offset)};
        T seg_sq_loc {seg_sq[idx + offset]};
        
        T seg_dot_r {inner_pr(seg_loc, r_loc)};
        T A {((seg_sq_loc*r_sq_loc) - (seg_dot_r*seg_dot_r))*r_norm_loc};
        bvec_nms::basic_vec<T> cr_pr {cross_pr(seg_loc, r_loc)/A};
        
        T cr_coeff {seg_dot_r/A};
        bvec_nms::basic_vec<T> cr_pr_coeff {(seg_loc*((seg_dot_r*cr_coeff*r_norm_loc*2) + 1)) - (r_loc*((seg_sq_loc*seg_dot_r*r_norm_loc*2/A) + (seg_dot_r/r_sq_loc)))};
        
        ret_vec[0] += cr_pr*seg_dot_r;
        for(unsigned comp = 0; comp < 3; ++comp) {
            ret_vec[1+comp] += (cr_pr*cr_pr_coeff[comp]) + (cross_pr(seg_loc, bvec_nms::unit_vec<T>(comp))*cr_coeff);
        }
    }
    
    for(auto& vec: ret_vec) {vec *= 1.0e-7;}
    return ret_vec;
}


template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
void bss_nms::biot_savart_seg(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld)
{
    decltype(&biot_savart_seg_half_pt<Calc_T>) func = (fld.in_params.grads ? biot_savart_grad_seg_half_pt<Calc_T> : biot_savart_seg_half_pt<Calc_T>);
    std::vector<std::string> der_strs {gd_nms::base_strs(gd_nms::quant::B, fld.in_params.grads)};
    
    std::array<mem_arr_vec_nms::mem_arr_vec<Calc_T>, 2> coil_data_arr {biot_savart_coil_prep<Calc_T, Rslt_T, Grid_Type>(fld)};
    mem_arr_nms::mem_arr<Calc_T> segs_sq {norm_sq(coil_data_arr[1])};
    
    size_t grid_idx_0 {fld.data.part.get_first_elem_idx()};
    for(size_t grid_idx = 0; grid_idx < fld.data.part.get_elems_in_part(); ++grid_idx) {
        if(!fld.coord_grid.out_of_bds(grid_idx + grid_idx_0))
        {
            mem_arr_vec_nms::mem_arr_vec<Calc_T> r {bvec_nms::convert<Calc_T>(fld.coord_grid.coords(grid_idx + grid_idx_0)) - coil_data_arr[0]};
            mem_arr_nms::mem_arr<Calc_T> r_sq {norm_sq(r)};
            mem_arr_nms::mem_arr<Calc_T> r_norm {sqrt(r_sq)};
            
            std::vector<bvec_nms::basic_vec<Calc_T>> bs_res {func(r, r_sq, r_norm, coil_data_arr[1], segs_sq, 1)};
            std::vector<bvec_nms::basic_vec<Calc_T>> rot_bs_res {func(r, r_sq, r_norm, coil_data_arr[1], segs_sq, 0)};
            
            for(unsigned comp = 0; comp < 3; ++comp)
            {
                for(unsigned der_comp = 0; der_comp < der_strs.size(); ++der_comp) {
                    fld.data[der_strs[der_comp]][comp][grid_idx] = bs_res[der_comp][comp] - rot_bs_res[der_comp][comp];
                }
            }
        }
    }
}

#endif /* BIOT_SAVART_SEGS_HPP */
