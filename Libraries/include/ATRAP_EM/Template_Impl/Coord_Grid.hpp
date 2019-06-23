#ifndef COORD_GRID_HPP
#define COORD_GRID_HPP

inline bvec_nms::basic_vec<> cg_nms::cart_coord_grid::coords(size_t grid_idx) const
{
    bvec_nms::basic_vec<> bv;
    
    for(int comp = 2; comp >= 0; --comp) {
        size_t coord_idx {grid_idx / stride[comp]};
        bv[comp] = lo_lims[comp] + (dr[comp]*coord_idx);
        grid_idx %= stride[comp];
    }
    
    return bv;
}

inline size_t cg_nms::cart_coord_grid::grid_idx(const bvec_nms::basic_vec<>& bv_coord) const
{
    bvec_nms::basic_vec<size_t> bv_idx;
    for(unsigned comp = 0; comp < 3; ++comp) {
        bv_idx[comp] = (bv_coord[comp] - lo_lims[comp])*inv_dr[comp];
    }
    return inner_pr(bv_idx, stride);
}

#endif /* COORD_GRID_HPP */
