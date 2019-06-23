#ifndef ROTATED_BASES_HPP
#define ROTATED_BASES_HPP

inline rotb_nms::rot_base::rot_base(const std::array<bvec_nms::basic_vec<>, 3>& basis_in) :
basis {basis_in}
{
    for(auto& vec : basis)
    {
        if(!norm_sq(vec)) {throw rotb_exc{"Basis must be nonzero"};}
        vec.normalize();
    }
    
    for(unsigned comp = 0; comp < 3; ++comp) {
        if(norm_sq(cross_pr(basis[(comp+0)%3], basis[(comp+1)%3]) - basis[(comp+2)%3]) > (1.e-12)) {
            throw rotb_exc{"Basis must be right-handed and orthogonal"};
        }
    }
}

template <class U>
inline U rotb_nms::rot_base::rot_to_abs_basis(const U& vec_rot_basis)
{
    U vec_abs_basis;
    vec_abs_basis.fill(0.0);
    
    for(unsigned abs_comp = 0; abs_comp < 3; ++abs_comp) {
        for(unsigned rot_comp = 0; rot_comp < 3; ++rot_comp) {
            vec_abs_basis[abs_comp] += basis[rot_comp][abs_comp]*vec_rot_basis[rot_comp];
        }
    }
    
    return vec_abs_basis;
}

template <class U>
inline U rotb_nms::rot_base::abs_to_rot_basis(const U& vec_abs_basis)
{
    U vec_rot_basis;
    vec_rot_basis.fill(0.0);
    
    for(unsigned rot_comp = 0; rot_comp < 3; ++rot_comp) {
        for(unsigned abs_comp = 0; abs_comp < 3; ++abs_comp) {
            vec_rot_basis[rot_comp] += basis[rot_comp][abs_comp]*vec_abs_basis[abs_comp];
        }
    }
    
    return vec_rot_basis;
}

inline rotb_nms::rot_base rotb_nms::gen_rot_base(bvec_nms::basic_vec<> rot_z, bvec_nms::basic_vec<> rot_x)
{
    for(unsigned comp = 0; comp < 3; ++comp)
    {
        bvec_nms::basic_vec<> rot_y = cross_pr(rot_z, rot_x);
        if(norm_sq(rot_y)) {return rot_base{{{cross_pr(rot_y, rot_z), rot_y, rot_z}}};}
        rot_x[comp] += 1;
    }
    
    throw rotb_exc{"First argument must be nonzero"};
}

#endif /* ROTATED_BASES_HPP */

