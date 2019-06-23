#ifndef FIELD_PARAMS_HPP
#define FIELD_PARAMS_HPP

/* Out_Params */
template <const std::string * Header, const std::string * Prompt>
boost::filesystem::path fp_nms::out_params<Header, Prompt>::coord_grid_src()
{
    return (output_dir / global_nms::default_opts_fn);
}

template <const std::string * Header, const std::string * Prompt>
std::string fp_nms::out_params<Header, Prompt>::header()
{
    return (*Header);
};

template <const std::string * Header, const std::string * Prompt>
std::vector<pid_nms::param_info> fp_nms::out_params<Header, Prompt>::get_param_info()
{
    std::vector<pid_nms::param_info> param_info_vec
    {
        {
            *Prompt,
            "Relative to \"large_storage_dir\" in Global_Info.h",
            gqt_nms::fnc_true,
            [this](std::string str){(this->output_dir) = (global_nms::large_storage_dir / boost::filesystem::path{str});}
        }
    };
    
    return param_info_vec;
}

template <const std::string * Header, const std::string * Prompt>
void fp_nms::out_params<Header, Prompt>::post_read_ops(boost::program_options::variables_map)
{
    grads = false;
}

#endif /* FIELD_PARAMS_HPP */
