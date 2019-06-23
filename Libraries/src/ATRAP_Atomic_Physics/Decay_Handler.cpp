#include "Decay_Handler.h"
#include "Random_Distributions.h"

global_nms::default_data_type decay_nms::em_unnormed_PDF(const decay_nms::dpm_coeff_t& dpm_coeffs, const decay_nms::theta_phi_t& theta_phi)
{
    using std::cos;
    using std::sin;
    using std::pow;
    
    auto theta {theta_phi[0]};
    auto phi {theta_phi[1]};
    
    dpm_coeff_t em_distr_arr
    {{
        pow(cos(theta)*cos(phi), 2) + pow(sin(phi), 2),
        pow(cos(theta)*sin(phi), 2) + pow(cos(phi), 2),
        pow(sin(theta), 2),
        - pow(sin(theta), 2)*sin(2*phi),
        - sin(2*theta)*cos(phi),
        - sin(2*theta)*sin(phi)
    }};
    
    for(auto& elem : em_distr_arr) {elem *= sin(theta);}
    
    return std::inner_product(dpm_coeffs.begin(), dpm_coeffs.end(), em_distr_arr.begin(), decltype(dpm_coeffs[0]){0});
}

global_nms::default_data_type decay_nms::em_unnormed_PDF_bd(const decay_nms::dpm_coeff_t& dpm_coeffs)
{
    dpm_coeff_t em_distr_bounds {{1, 1, 1, 1, 1, 1}};
    global_nms::default_data_type bound {0};
    
    for(size_t idx = 0; idx < em_distr_bounds.size(); ++idx) {
        bound += std::abs(em_distr_bounds[idx]*dpm_coeffs[idx]);
    }
    
    return bound;
}

bvec_nms::basic_vec<> decay_nms::theta_phi_vec(const decay_nms::theta_phi_t& theta_phi)
{
    auto theta {theta_phi[0]};
    auto phi {theta_phi[1]};
    
    return bvec_nms::basic_vec<> {
        std::sin(theta)*std::cos(phi), std::sin(theta)*std::sin(phi), std::cos(theta)
    };
}

bvec_nms::basic_vec<> decay_nms::norm_em_recoil(const decay_nms::dpm_coeff_t& dpm_coeffs)
{
    auto theta_phi_distr {[&dpm_coeffs](auto x) {return em_unnormed_PDF(dpm_coeffs, x);}};
    auto PDF_bd {em_unnormed_PDF_bd(dpm_coeffs)};
    
    auto theta_phi {distr_gen::rand<2>(theta_phi_distr, {{{{0, M_PI}}, {{0, 2*M_PI}}}}, PDF_bd)};
    return (-1.0)*theta_phi_vec(theta_phi);
}

