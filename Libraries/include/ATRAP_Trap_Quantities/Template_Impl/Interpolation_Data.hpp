#ifndef INTERPOLATION_DATA_HPP
#define INTERPOLATION_DATA_HPP

template <ig_nms::interp_type IT>
std::vector<id_nms::power_t> id_nms::index_to_xyz_powers_wrapper()
{
    static std::vector<power_t> ret_vec {index_to_xyz_powers<IT>()};
    return ret_vec;
}

template <ig_nms::interp_type IT>
id_nms::grid_offset_to_xyz_x0y0z0_to_coeff_map_t id_nms::get_coeff_map_wrapper()
{
    static grid_offset_to_xyz_x0y0z0_to_coeff_map_t ret_map {get_coeff_map<IT>()};
    return ret_map;
}

/*
 Polynomials are ordered by
 - Lowest order
 - Highest order x
 - Highest order y
 */

template <>
struct id_nms::monomial_handler<ig_nms::interp_type::zeroth>
{
    monomial_handler() {monomials[0] = 1;}
    
    std::array<global_nms::default_data_type, ig_nms::enum_val(ig_nms::interp_type::zeroth)> monomials;
    
    void set_monomials(const bvec_nms::basic_vec<>& r) {}
};

template <>
struct id_nms::monomial_handler<ig_nms::interp_type::lin>
{
    monomial_handler() {monomials[0] = 1;}
    
    std::array<global_nms::default_data_type, ig_nms::enum_val(ig_nms::interp_type::lin)> monomials;
    
    void set_monomials(const bvec_nms::basic_vec<>& r)
    {
        monomials[1] = r[0];
        monomials[2] = r[1];
        monomials[3] = r[2];
    }
};

template <>
struct id_nms::monomial_handler<ig_nms::interp_type::quad>
{
    monomial_handler() {monomials[0] = 1;}
    
    std::array<global_nms::default_data_type, ig_nms::enum_val(ig_nms::interp_type::quad)> monomials;
    
    void set_monomials(const bvec_nms::basic_vec<>& r)
    {
        monomials[1] = r[0];
        monomials[2] = r[1];
        monomials[3] = r[2];
        monomials[4] = r[0]*r[0];
        monomials[5] = r[0]*r[1];
        monomials[6] = r[0]*r[2];
        monomials[7] = r[1]*r[1];
        monomials[8] = r[1]*r[2];
        monomials[9] = r[2]*r[2];
    }
};

template <>
struct id_nms::monomial_handler<ig_nms::interp_type::trilin>
{
    monomial_handler() {monomials[0] = 1;}
    
    std::array<global_nms::default_data_type, ig_nms::enum_val(ig_nms::interp_type::trilin)> monomials;
    
    void set_monomials(const bvec_nms::basic_vec<>& r)
    {
        monomials[1] = r[0];
        monomials[2] = r[1];
        monomials[3] = r[2];
        monomials[4] = r[0]*r[1];
        monomials[5] = r[0]*r[2];
        monomials[6] = r[1]*r[2];
        monomials[7] = r[0]*r[1]*r[2];
    }
};

#endif /* INTERPOLATION_DATA_HPP */

