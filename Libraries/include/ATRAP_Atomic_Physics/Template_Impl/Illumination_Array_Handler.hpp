#ifndef ILLUMINATION_ARRAY_HANDLER_HPP
#define ILLUMINATION_ARRAY_HANDLER_HPP

#include "Random_Distributions.h"


/* Illumination handler functions */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
inline void ill_arr_hand_nms::illumination_array_handler<IT, EM_Quant_Gen>::fill(const boost::filesystem::path& opts_fn, const std::array<int, 3>& det_lims)
{
    ill_hand_nms::illumination_handler_base<IT, EM_Quant_Gen>::fill(opts_fn);
    
    beam_passes = 0;
    t_in_beam = 0.0;
    
    detunings.clear();
    dets_2pi.clear();
    for(int det = det_lims[0]; det <= det_lims[1]; det += det_lims[2]) {
        detunings.push_back(2*det);
        dets_2pi.push_back(2.0*2.0*M_PI*det);
    }
    
    active_members = detunings.size();
    active.assign(detunings.size(), true);
    
    d_mat_vec.resize(detunings.size());
    
    decay_threshold_vec.resize(detunings.size());
    excitation_threshold_vec.resize(detunings.size());
    
    decaying_idxs.clear();
}


/*
 This should be run when
 - each time the atom first comes into view of a laser
 - each time the atom leaves the laser's view in the excited state
 */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
inline void ill_arr_hand_nms::illumination_array_handler<IT, EM_Quant_Gen>::initialize(bool in_gr_st)
{
    for(size_t idx = 0; idx < detunings.size(); ++idx)
    {
        if(active[idx])
        {
            d_mat_vec[idx] = {{0.0, 1.0}};
            decay_threshold_vec[idx] = distr_gen::rand();
            excitation_threshold_vec[idx] = distr_gen::rand();
        }
    }

    if(!in_gr_st) {
        for(size_t idx : decaying_idxs) {
            if(active[idx]) {d_mat_vec[idx] = {{1.0, 0.0}};}
        }
    }

}

/*

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
inline void ill_arr_hand_nms::illumination_array_handler<IT, EM_Quant_Gen>::integration_step_elem(
                                                                                                  std::complex<global_nms::default_data_type> G,
                                                                                                  std::complex<global_nms::default_data_type> O,
                                                                                                  std::complex<global_nms::default_data_type> d,
                                                                                                  std::complex<global_nms::default_data_type> dt,
                                                                                                  size_t idx)
{
    auto GG = G*G;
    auto OO4 = 4.0*O*O;
    auto dd4 = 4.0*d*d;
    
    auto tmp = std::sqrt((-4.0*GG*OO4) + ((GG + dd4 + OO4)*(GG + dd4 + OO4)));
    auto tmp_p = std::sqrt(GG - dd4 - OO4 + tmp);
    auto tmp_m = std::sqrt(GG - dd4 - OO4 - tmp);
    
    auto sqrt2 = std::sqrt(2.0);
    
    auto cosh_p = std::cosh(dt*tmp_p/(2.0*sqrt2));
    auto cosh_m = std::cosh(dt*tmp_m/(2.0*sqrt2));
    auto sinh_p = std::sinh(dt*tmp_p/(2.0*sqrt2));
    auto sinh_m = std::sinh(dt*tmp_m/(2.0*sqrt2));
    
    auto prefactor {std::exp(-dt*G/2.0)/tmp};
    
    d_mat_t x_i {d_mat_vec[idx]};
    d_mat_t& x_f {d_mat_vec[idx]};
    
    x_f[0] =
    prefactor * (
                 ((((tmp + GG + dd4 - OO4)*cosh_m) + ((tmp - GG - dd4 + OO4)*cosh_p))/2.0)*x_i[0] +
                 ((((tmp + GG + dd4 - OO4)*tmp_m*sinh_m) + ((tmp - GG - dd4 + OO4)*tmp_p*sinh_p))/(4.0*sqrt2*d))*x_i[1] +
                 (-4.0*d*O*(cosh_m - cosh_p))*x_i[2] +
                 (4.0*sqrt2*G*d*O*((sinh_m/tmp_m) - (sinh_p/tmp_p)))*x_i[3]
                 );
    
    x_f[1] =
    prefactor * (
                 (sqrt2*d*(((tmp + GG + dd4 + OO4)*sinh_m/tmp_m) + ((tmp - GG - dd4 - OO4)*sinh_p/tmp_p)))*x_i[0] +
                 ((((tmp + GG + dd4 + OO4)*cosh_m) + ((tmp - GG - dd4 - OO4)*cosh_p))/2.0)*x_i[1] +
                 (sqrt2*O*((tmp_m*sinh_m) - (tmp_p*sinh_p)))*x_i[2] +
                 (-2.0*G*O*(cosh_m - cosh_p))*x_i[3]
                 );
    
    x_f[2] =
    prefactor * (
                 (-4.0*d*O*(cosh_m - cosh_p))*x_i[0] +
                 (sqrt2*O*((-tmp_m*sinh_m) + (tmp_p*sinh_p)))*x_i[1] +
                 ((((tmp - GG - dd4 + OO4)*cosh_m) + ((tmp + GG + dd4 - OO4)*cosh_p))/2.0)*x_i[2] +
                 (-(((tmp - GG - dd4 - OO4)*tmp_m*sinh_m) + ((tmp + GG + dd4 + OO4)*tmp_p*sinh_p))/(2.0*sqrt2*G))*x_i[3]
                 );
    
    x_f[3] =
    prefactor * (
                 (4.0*sqrt2*G*d*O*((sinh_m/tmp_m) - (sinh_p/tmp_p)))*x_i[0] +
                 (2.0*G*O*(cosh_m - cosh_p))*x_i[1] +
                 (-(((tmp - GG - dd4 + OO4)*sinh_m/tmp_m) + ((tmp + GG + dd4 - OO4)*sinh_p/tmp_p))*G/sqrt2)*x_i[2] +
                 ((((tmp - GG - dd4 - OO4)*cosh_m) + ((tmp + GG + dd4 + OO4)*cosh_p))/2.0)*x_i[3]
                 );
}
*/

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
inline void ill_arr_hand_nms::illumination_array_handler<IT, EM_Quant_Gen>::integration_step_elem(
                                                                                                  std::complex<global_nms::default_data_type> G /* decay rate */,
                                                                                                  std::complex<global_nms::default_data_type> O /* Rabi frequency */,
                                                                                                  std::complex<global_nms::default_data_type> d /* angular detuning */,
                                                                                                  std::complex<global_nms::default_data_type> dt /* exposure time */,
                                                                                                  size_t idx /* index to address */)
{
    using namespace std::complex_literals;
static size_t count {0};
++count;
    if(std::norm(O) < 1.0e-6)
    {
        d_mat_vec[idx][0] *= std::exp(-0.5*(G.real())*(dt.real()));
        return;
    }
    auto rt = std::sqrt((G*G) - ((4.0i)*G*d) - (4.0*d*d) - (4.0*O*O));
    
    auto l_p = 0.25*(-G + ((2.0i)*d) + rt);
    auto l_m = 0.25*(-G + ((2.0i)*d) - rt);
    
    auto IO = (0.5i)*O;
    
    auto exp_p = std::exp(l_p*dt);
    auto exp_m = std::exp(l_m*dt);
    
    auto x_i {d_mat_vec[idx]};
    auto& x_f {d_mat_vec[idx]};
    
    x_f[0] = (((l_p*exp_p) - (l_m*exp_m))*x_i[0] - (l_m/IO)*l_p*(exp_p - exp_m)*x_i[1])/(l_p - l_m);
    x_f[1] = (IO*(exp_p - exp_m)*x_i[0] - ((l_m*exp_p) - (l_p*exp_m))*x_i[1])/(l_p - l_m);
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
inline void ill_arr_hand_nms::illumination_array_handler<IT, EM_Quant_Gen>::check_for_excitation()
{
    for(size_t idx = 0; idx < d_mat_vec.size(); ++idx) {
        if(active[idx] && std::norm(d_mat_vec[idx][ce]) > excitation_threshold_vec[idx]*P(idx)) {
            decaying_idxs.push_back(idx);
        }
    }
}

#endif /* ILLUMINATION_ARRAY_HANDLER_HPP */

