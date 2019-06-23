/* laser_path functions */
inline bool las_path_nms::laser_path::in_beam(const bvec_nms::basic_vec<>& pos, global_nms::default_data_type buffer_rad) const
{
    auto in_beam_rad {(2*waist) + buffer_rad};
    auto r_sq {norm_sq(cross_pr(pos - focus, unit_wave_vector))};
    return (r_sq < in_beam_rad*in_beam_rad);
}

inline global_nms::default_data_type las_path_nms::laser_path::intensity(const bvec_nms::basic_vec<>& pos) const
{
    auto r_sq {norm_sq(cross_pr(pos - focus, unit_wave_vector))};
    return (2*one_way_P/(M_PI*waist*waist))*std::exp(-2*r_sq/(waist*waist));
}


/* path_mngr */
inline bool las_path_nms::path_mngr::in_beam(const bvec_nms::basic_vec<>& pos, global_nms::default_data_type buffer_rad) const
{
    for(const auto& path : paths) {
        if(path.in_beam(pos, buffer_rad)) {return true;}
    }
    
    return false;
}

