#include "Interpolation_Grids.h"

/* */
std::vector<boost::filesystem::path> ig_nms::interp_source_files(const boost::filesystem::path& input_dir, ig_nms::interp_quant iq)
{
    std::map<interp_quant, std::pair<gd_nms::quant, bool>> iq_to_input_pair
    {
        {interp_quant::BdB, {gd_nms::quant::B, true}},
        {interp_quant::B, {gd_nms::quant::B, false}},
        {interp_quant::Bnorm, {gd_nms::quant::Bnorm, false}},
        {interp_quant::acc_1Sc, {gd_nms::quant::acc_1Sc, false}},
        {interp_quant::acc_1Sd, {gd_nms::quant::acc_1Sd, false}},
        {interp_quant::acc_2Sc, {gd_nms::quant::acc_2Sc, false}},
        {interp_quant::acc_2Sd, {gd_nms::quant::acc_2Sd, false}}
    };
    
    std::vector<std::pair<boost::filesystem::path, bool>> fn_pair_vec;
    auto input_pair {iq_to_input_pair.at(iq)};
    
    for(std::string str : gd_nms::base_strs(input_pair.first, input_pair.second)) {
        fn_pair_vec.push_back({gd_nms::base_str_to_fn(input_dir, str), !quant_is_scalar(input_pair.first)});
    }
    
    return fio_nms::expand_fns(fn_pair_vec);
}


/* ig_in_params */

boost::filesystem::path ig_nms::ig_in_params::coord_grid_src()
{
    return (input_dir / global_nms::default_opts_fn);
}

std::string ig_nms::ig_in_params::header()
{
    return std::string{"INTERPOLATION INFO"};
}

std::vector<pid_nms::param_info> ig_nms::ig_in_params::get_param_info()
{
    std::map<std::string, interp_type> str_to_interp_type {{"ZEROTH", interp_type::zeroth}, {"LINEAR", interp_type::lin}, {"QUADRATIC", interp_type::quad}, {"TRILINEAR", interp_type::trilin}};
    
    std::vector<pid_nms::param_info> param_info_vec
    {
        {
            "Interpolation Source Directory",
            "Relative to \"large_storage_dir\" in Global_Info.h",
            gqt_nms::fnc_true,
            [this](std::string str){(this->input_dir) = (global_nms::large_storage_dir / boost::filesystem::path{str});}
        },
        {
            "Interpolation Mode",
            "ZEROTH, LINEAR, QUADRATIC, or TRILINEAR",
            gqt_nms::fnc_true,
            [this, str_to_interp_type](std::string str) {(this->it) = str_to_interp_type.at(str);}
        }
    };
    
    return param_info_vec;
}

void ig_nms::ig_in_params::post_read_ops(boost::program_options::variables_map)
{
    grads = false;
}

/* Constant Strings */

const std::string ig_nms::iop_header {"INTERPOLATION STORAGE"};
const std::string ig_nms::iop_prompt {"Interpolation Output Directory"};

