#ifndef LASER_PATHS_H
#define LASER_PATHS_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"

namespace las_path_nms
{
    /* Exception for functions in this header */
    class las_path_exc {
    public:
        las_path_exc(std::string& msg): err_message{msg} {}
        las_path_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    struct laser_path_fn
    {
        std::string header() {return "LASER PATH FILES";}
        std::vector<std::string> val_str;
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map) {}
        
        size_t path_idx;
        std::string fn;
        
        laser_path_fn(const boost::filesystem::path&, size_t);
    };
    
    struct laser_path
    {
        std::vector<std::string> val_str;
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        laser_path(const boost::filesystem::path& opts_fn) {gqt_nms::fill_params(opts_fn, *this, val_str);}
        
        global_nms::default_data_type one_way_P;
        bvec_nms::basic_vec<> unit_wave_vector;
        bvec_nms::basic_vec<> focus;
        global_nms::default_data_type waist;
        std::array<std::complex<global_nms::default_data_type>, 3> pol;
        
        bool in_beam(const bvec_nms::basic_vec<>&, global_nms::default_data_type) const;
        global_nms::default_data_type intensity(const bvec_nms::basic_vec<>&) const;
    };
    
    struct path_mngr
    {
        std::vector<laser_path> paths;
        
        path_mngr() {}
        path_mngr(const boost::filesystem::path& opts_fn) {fill_path_mngr(opts_fn);}
        void fill_path_mngr(const boost::filesystem::path&);
        
        bool in_beam(const bvec_nms::basic_vec<>&, global_nms::default_data_type) const;
    };
}

#include "Template_Impl/Laser_Paths.hpp"

#endif /* LASER_PATHS_H */

