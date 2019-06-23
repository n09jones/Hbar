#ifndef LOCAL_FIELDS_HPP
#define LOCAL_FIELDS_HPP

#include "Rotated_Bases.h"

/* */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>::load_for_illumination(const pdef_nms::state_t& pos_vel)
{
    load_B_fields(pos_vel);
    if(pdef_nms::is_TwoPh<IT>) {compute_E_fields(pos_vel);}
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>::load_B_fields(const pdef_nms::state_t& pos_vel)
{
    std::array<global_nms::default_data_type, 1> tmp_Bnorm {};
    this->template EM_quant<ig_nms::interp_quant::Bnorm>(tmp_Bnorm, pos_vel.first);
    Bnorm = tmp_Bnorm[0];
    
    this->template EM_quant<ig_nms::interp_quant::B>(B, pos_vel.first);
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>::compute_E_fields(const pdef_nms::state_t& pos_vel)
{
    E = cross_pr(pos_vel.second, B);
    E_sq = norm_sq(E);
}


/* */
template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>::load_per_path(const pdef_nms::state_t& pos_vel, const las_path_nms::laser_path& path)
{
    compute_intensity(pos_vel, path);
    if(pdef_nms::is_LyAlph<IT>) {compute_loc_pol(path);}
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>::compute_intensity(const pdef_nms::state_t& pos_vel, const las_path_nms::laser_path& path)
{
    intensity = path.intensity(pos_vel.first);
}

template <pdef_nms::illum_type IT, class EM_Quant_Gen>
void loc_fld_nms::local_EM_fields<IT, EM_Quant_Gen>::compute_loc_pol(const las_path_nms::laser_path& path)
{
    loc_pol = rotb_nms::gen_rot_base(B).abs_to_rot_basis(path.pol);
}

#endif /* LOCAL_FIELDS_HPP */

