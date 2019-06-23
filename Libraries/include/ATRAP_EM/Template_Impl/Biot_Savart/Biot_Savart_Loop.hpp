#ifndef BIOT_SAVART_LOOP_HPP
#define BIOT_SAVART_LOOP_HPP

#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>

template <class T>
std::vector<bvec_nms::basic_vec<T>> bsl_nms::biot_savart_loop_pt(bvec_nms::basic_vec<T> r, T z0, T a)
{
    std::vector<bvec_nms::basic_vec<T>> ret_vec_arr(1, bvec_nms::basic_vec<T>{0, 0, 0});
    
    T x = r[0];
    T y = r[1];
    T z = r[2] - z0;
    
    /* Do the calculations first with local variables. */
    T asq = a*a;
    T rho = std::sqrt(x*x + y*y);
    T mu0div4pi = 1.0e-7;
    
    /* Off-axis calculation */
    if(rho > 1.0e-7) {
        T rsq = (rho*rho) + (z*z);
        T alphasq = asq + rsq - (2*a*rho);
        T betasq = asq + rsq + (2*a*rho);
        T beta = std::sqrt(betasq);
        T ksq = 1 - (alphasq/(asq + rsq + (2*a*rho)));
        T k = std::sqrt(ksq);
        
        T El_E = boost::math::ellint_2(k);
        T El_K = boost::math::ellint_1(k);
        T coeff = (2*mu0div4pi*z/(rho*rho*beta))*((((asq + rsq)/alphasq)*El_E) - El_K);
        
        ret_vec_arr[0][0] = x*coeff;
        ret_vec_arr[0][1] = y*coeff;
        ret_vec_arr[0][2] = mu0div4pi*(2/beta)*((((asq - rsq)/alphasq)*El_E) + El_K);
    }
    
    /* On-axis calculation */
    else {
        T densq = asq + (z*z);
        T den = std::sqrt(densq);
        ret_vec_arr[0][2] = mu0div4pi*2*M_PI*asq/(den*densq);
    }
    
    return ret_vec_arr;
}

template <class T>
std::vector<bvec_nms::basic_vec<T>> bsl_nms::biot_savart_grad_loop_pt(bvec_nms::basic_vec<T> r, T z0, T a)
{
    std::vector<bvec_nms::basic_vec<T>> ret_vec_arr(4, bvec_nms::basic_vec<T>{0, 0, 0});
    
    T x = r[0];
    T y = r[1];
    T z = r[2] - z0;
    
    /* Do the calculations first with local variables. */
    T asq = a*a;
    T xsq = x*x;
    T ysq = y*y;
    T zsq = z*z;
    T rhosq = x*x + y*y;
    T rho = std::sqrt(rhosq);
    T mu0div4pi = 1.0e-7;
    
    /* Off-axis calculation */
    if(rho > 1.0e-7) {
        T rsq = rhosq + z*z;
        T alphasq = asq + rsq - (2*a*rho);
        T betasq = asq + rsq + (2*a*rho);
        T beta = std::sqrt(betasq);
        T ksq = 1 - (alphasq/(asq + rsq + (2*a*rho)));
        T k = std::sqrt(ksq);
        
        T El_E = boost::math::ellint_2(k);
        T El_K = boost::math::ellint_1(k);
        T coeff = (2*mu0div4pi*z/(rhosq*beta))*((((asq + rsq)/alphasq)*El_E) - El_K);
        T coeff1 = (2*mu0div4pi/(rhosq*beta*betasq))*((((asq + rsq)/alphasq)*El_E) - El_K);
        T coeff2 = (mu0div4pi*ksq/((1-ksq)*rhosq*beta))*((((1+ksq)/(1-ksq))*El_E) - El_K);
        T coeff3 = ((2*a)-(ksq*(a+rho)))*coeff2/(4*a);
        
        ret_vec_arr[0][0] = x*coeff;
        ret_vec_arr[0][1] = y*coeff;
        ret_vec_arr[0][2] = mu0div4pi*(2/beta)*((((asq - rsq)/alphasq)*El_E) + El_K);
        
        T temp1 = z*((((betasq*(ysq-xsq))-(xsq*rho*(a+rho)))*coeff1) + (xsq*coeff3))/rhosq;
        ret_vec_arr[1][0] = temp1;
        T temp2 = z*((((betasq*(xsq-ysq))-(ysq*rho*(a+rho)))*coeff1) + (ysq*coeff3))/rhosq;
        ret_vec_arr[2][1] = temp2;
        ret_vec_arr[3][2] = -temp1 - temp2;
        temp1 = x*y*z*((-(rhosq+(a*rho)+(2*betasq))*coeff1) + coeff3)/rhosq;
        ret_vec_arr[1][1] = ret_vec_arr[2][0] = temp1;
        temp1 = ((betasq-zsq)*coeff1) - (zsq*coeff2/betasq);
        ret_vec_arr[1][2] = ret_vec_arr[3][0] = x*temp1;
        ret_vec_arr[2][2] = ret_vec_arr[3][1] = y*temp1;
    }
    
    /* On-axis calculation */
    else {
        T num = mu0div4pi*M_PI*asq;
        T densq = asq + zsq;
        T den = std::sqrt(densq);
        
        ret_vec_arr[0][2] = 2*num/(den*densq);
        ret_vec_arr[1][0] = ret_vec_arr[2][1] = 3*z*num/(den*densq*densq);
        ret_vec_arr[3][2] = -6*z*num/(den*densq*densq);
    }
    
    return ret_vec_arr;
}


template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
void bsl_nms::biot_savart_loop(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld, global_nms::default_data_type z0, global_nms::default_data_type a)
{
    decltype(&biot_savart_loop_pt<Calc_T>) func = (fld.in_params.grads ? biot_savart_grad_loop_pt<Calc_T> : biot_savart_loop_pt<Calc_T>);
    std::vector<std::string> der_strs {gd_nms::base_strs(gd_nms::quant::B, fld.in_params.grads)};
    
    size_t grid_idx_0 {fld.data.part.get_first_elem_idx()};
    for(size_t grid_idx = 0; grid_idx < fld.data.part.get_elems_in_part(); ++grid_idx) {
        if(!fld.coord_grid.out_of_bds(grid_idx + grid_idx_0))
        {
            auto bs_res {func(bvec_nms::convert<Calc_T>(fld.coord_grid.coords(grid_idx + grid_idx_0)), z0, a)};
            
            for(unsigned comp = 0; comp < 3; ++comp)
            {
                for(unsigned der_comp = 0; der_comp < der_strs.size(); ++der_comp) {
                    fld.data[der_strs[der_comp]][comp][grid_idx] += bs_res[der_comp][comp];
                }
            }
        }
    }
}

template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
void bsl_nms::biot_savart_pinch(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld)
{
    auto z0_rng {gen_mirror_coil_params(0.09017, 0.14983, 38)};
    auto a_rng {gen_mirror_coil_params(0.083820, 0.099472, 13)};
    
    for(auto z0: z0_rng) {
        for(auto a: a_rng) {
            biot_savart_loop<Calc_T, Rslt_T, Grid_Type>(fld, z0, a);
            biot_savart_loop<Calc_T, Rslt_T, Grid_Type>(fld, -z0, a);
        }
    }
}

template <class Calc_T, class Rslt_T, cg_nms::grid_type Grid_Type>
void bsl_nms::biot_savart_buck(cgq_nms::single_coil<par_arr_nms::storage::mem, Grid_Type, true, Rslt_T>& fld)
{
    auto z0_rng {gen_mirror_coil_params(0.03201, 0.05399, 14)};
    auto a_rng {gen_mirror_coil_params(0.083820, 0.099472, 13)};
    
    for(auto z0: z0_rng) {
        for(auto a: a_rng) {
            biot_savart_loop<Calc_T, Rslt_T, Grid_Type>(fld, z0, a);
            biot_savart_loop<Calc_T, Rslt_T, Grid_Type>(fld, -z0, a);
        }
    }
}

#endif /* BIOT_SAVART_LOOP_HPP */
