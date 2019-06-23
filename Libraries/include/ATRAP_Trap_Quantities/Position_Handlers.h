#ifndef POSITION_HANDLERS_H
#define POSITION_HANDLERS_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"
#include "Interpolation_Grids.h"
#include "Interpolation_Data.h"

namespace pos_nms
{
    class position
    {
    protected:
        global_nms::default_data_type trap_radius;
        global_nms::default_data_type trap_radius_sq;
        
    public:
        position();
        
        bool out_of_bounds;
        bvec_nms::basic_vec<> out_of_bounds_position;
        
        bvec_nms::basic_vec<> pos;
        
        void update_position_info(bvec_nms::basic_vec<> pos_new)
        {            
            if((pos_new[0]*pos_new[0]) + (pos_new[1]*pos_new[1]) > trap_radius_sq)
            {
                if(!out_of_bounds) {out_of_bounds_position = pos;}
                out_of_bounds = true;
            }

            pos = pos_new;
        }
    };
    
    template <ig_nms::interp_type IT>
    class grid_position:
    public position,
    public id_nms::monomial_handler<IT>
    {
    private:
        global_nms::default_data_type abs_z_lim;
        
    public:
        grid_position() {}
        grid_position(const cg_nms::cart_coord_grid&);
        
        void init_grid_position(const cg_nms::cart_coord_grid&);
        
        cg_nms::cart_coord_grid pos_coord_grid;
        
        bvec_nms::basic_vec<> lo_lims;
        bvec_nms::basic_vec<> up_lims;
        size_t grid_idx;
        
        void update_position_info(bvec_nms::basic_vec<> pos_new)
        {            
            if(((pos_new[0]*pos_new[0]) + (pos_new[1]*pos_new[1]) > trap_radius_sq) || std::abs(pos_new[2]) > abs_z_lim)
            {
                if(!out_of_bounds) {out_of_bounds_position = pos;}
                out_of_bounds = true;
            }

            pos = pos_new;
            
            for(unsigned comp = 0; comp < 3; ++comp)
            {
                if((pos[comp] < lo_lims[comp]) || (pos[comp] >= up_lims[comp]))
                {
                    grid_idx = pos_coord_grid.grid_idx(pos);
                    lo_lims = pos_coord_grid.coords(grid_idx);
                    up_lims = lo_lims + pos_coord_grid.dr;
                    break;
                }
            }
            
            this->set_monomials(pos);
        }
    };
}

#include "Template_Impl/Position_Handlers.hpp"

#endif /* POSITION_HANDLERS_H */

