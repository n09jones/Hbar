#ifndef LASER_PROPERTIES_H
#define LASER_PROPERTIES_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"

namespace las_props_nms
{
    /* Exception for functions in this header */
    class las_props_exc {
    public:
        las_props_exc(std::string& msg): err_message{msg} {}
        las_props_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    struct laser_properties
    {
        std::string header() {return "LASER PROPERTIES";}
        std::vector<std::string> val_str;
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        laser_properties() {}
        laser_properties(const boost::filesystem::path& opts_fn) {fill_laser_properties(opts_fn);}
        void fill_laser_properties(const boost::filesystem::path& opts_fn) {gqt_nms::fill_params(opts_fn, *this, val_str);}
        
        global_nms::default_data_type det;
        global_nms::default_data_type B_0_det;
        global_nms::default_data_type FWHM;
        
        bool pulsed;
        
        global_nms::default_data_type pulse_len;
        global_nms::default_data_type rep_rate;
    };
}

#endif /* LASER_PROPERTIES_H */

