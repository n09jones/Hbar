#ifndef COORD_GRID_H
#define COORD_GRID_H

#include "ATRAP_base.h"
#include "Param_Info_Def.h"

namespace cg_nms
{
    /* Exception for functions in this header */
    class cg_exc {
    public:
        cg_exc(std::string& msg): err_message{msg} {}
        cg_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    struct cart_coord_grid
    {
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        bvec_nms::basic_vec<> lo_lims;
        bvec_nms::basic_vec<> up_lims;
        bvec_nms::basic_vec<size_t> pts;
        
        global_nms::default_data_type max_rad;
        
        bvec_nms::basic_vec<> dr;
        bvec_nms::basic_vec<> inv_dr;
        bvec_nms::basic_vec<size_t> stride;
        
        size_t grid_size() const {return (pts[0]*pts[1]*pts[2]);}
        bvec_nms::basic_vec<> coords(size_t) const;
        
        size_t grid_idx(const bvec_nms::basic_vec<>&) const;
        
        bool out_of_bds(const bvec_nms::basic_vec<>& bv) const {return ((bv[0]*bv[0]) + (bv[1]*bv[1]) > max_rad*max_rad);}
        bool out_of_bds(size_t grid_idx) const {return out_of_bds(coords(grid_idx));}
        
        mem_arr_nms::mem_arr<> in_bounds_mask(size_t = 1, size_t = 0);
    };
    
    class file_coord_grid
    {
    private:
        mapped_arr_nms::mapped_arr<> coord_arr;
        
    public:
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        boost::filesystem::path grid_fn;
        
        size_t grid_size() const {return coord_arr.size()/3;}
        bvec_nms::basic_vec<> coords(size_t) const;
        
        bool out_of_bds(const bvec_nms::basic_vec<>&) const {return false;}
        bool out_of_bds(size_t) const {return false;}
        
        mem_arr_nms::mem_arr<> in_bounds_mask(size_t = 1, size_t = 0);
    };
    
    bool operator==(const cart_coord_grid&, const cart_coord_grid&);
    bool operator!=(const cart_coord_grid&, const cart_coord_grid&);
    bool operator==(const file_coord_grid&, const file_coord_grid&);
    bool operator!=(const file_coord_grid&, const file_coord_grid&);
    
    bool gives_cart_grid(const boost::filesystem::path&);
    
    enum class grid_type {cart, file};
    template <grid_type Grid_Type>
    using coord_grid = typename std::conditional<Grid_Type == grid_type::cart, cart_coord_grid, file_coord_grid>::type;
}

#include "Template_Impl/Coord_Grid.hpp"

#endif /* COORD_GRID_H */
