#ifndef FIELD_PARAMS_H
#define FIELD_PARAMS_H

#include "ATRAP_base.h"
#include "Param_Info_Def.h"
#include "Grid_Quantity_Template.h"

namespace fp_nms
{
    /* Exception for functions in this header */
    class fp_exc {
    public:
        fp_exc(std::string& msg): err_message{msg} {}
        fp_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    enum class coil {quad, oct, pinch, buck, custom};
    
    struct single_coil_in_params
    {
        static const bool coord_grid_deduced {false};
        boost::filesystem::path coord_grid_src();
        
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        coil coil_nm;
        boost::filesystem::path coil_fn;
        bool grads;
    };
    
    struct sum_coil_in_params
    {
        static const bool coord_grid_deduced {true};
        boost::filesystem::path coord_grid_src();
        
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        std::map<coil, std::pair<boost::filesystem::path, global_nms::default_data_type>> coil_info;
        bvec_nms::basic_vec<> offset_field;
        bool grads;
    };
    
    template <const std::string * Header, const std::string * Prompt>
    struct out_params
    {
        static const bool coord_grid_deduced {true};
        boost::filesystem::path coord_grid_src();
        
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        bool grads;
        boost::filesystem::path output_dir;
    };
    
    extern const std::string scop_header;
    extern const std::string scop_prompt;
    using single_coil_out_params = out_params<&scop_header, &scop_prompt>;
    
    extern const std::string smcop_header;
    extern const std::string smcop_prompt;
    using sum_coil_out_params = out_params<&smcop_header, &smcop_prompt>;
    
    extern const std::string dqop_header;
    extern const std::string dqop_prompt;
    using derived_quantity_out_params = out_params<&dqop_header, &dqop_prompt>;
}

#include "Template_Impl/Field_Params.hpp"

#endif /* FIELD_PARAMS_H */
