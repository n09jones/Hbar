#include "Field_Params.h"
#include "Grid_Quantity_Template.h"

/* single_coil_in_params */
boost::filesystem::path fp_nms::single_coil_in_params::coord_grid_src()
{
    return boost::filesystem::path{};
}

std::string fp_nms::single_coil_in_params::header()
{
    return std::string{"SINGLE-COIL FIELD PARAMETERS"};
}

std::vector<pid_nms::param_info> fp_nms::single_coil_in_params::get_param_info()
{
    std::map<std::string, coil> str_to_coil {{"QUAD", coil::quad}, {"OCT", coil::oct}, {"PINCH", coil::pinch}, {"BUCK", coil::buck}, {"CUSTOM", coil::custom}};
    std::map<std::string, bool> str_to_tf {{"TRUE", true}, {"FALSE", false}};
    
    std::vector<pid_nms::param_info> param_info_vec
    {
        {
            "Coil",
            "QUAD, OCT, PINCH, BUCK, or CUSTOM",
            gqt_nms::fnc_true,
            [this, str_to_coil](std::string str){(this->coil_nm) = str_to_coil.at(str);}
        },
        {
            "Coil File Name",
            "Relative to \"coil_dir\" in Global_Info.h, omit if Coil = PINCH or BUCK",
            [this](){return (((this->coil_nm) != coil::pinch) && ((this->coil_nm) != coil::buck));},
            [this](std::string str){(this->coil_fn) = (global_nms::coil_dir / boost::filesystem::path{str});}
        },
        {
            "Include Gradients",
            "TRUE or FALSE",
            gqt_nms::fnc_true,
            [this, str_to_tf](std::string str){(this->grads) = str_to_tf.at(str);}
        }
    };
    
    return param_info_vec;
}

void fp_nms::single_coil_in_params::post_read_ops(boost::program_options::variables_map)
{
    if(((coil_nm == coil::pinch) || (coil_nm == coil::buck)) && (coil_fn != global_nms::coil_dir)) {
        throw fp_exc{"Coil file name isn't needed for Pinch and Bucking coils"};
    }
}


/* sum_coil_in_params */
boost::filesystem::path fp_nms::sum_coil_in_params::coord_grid_src()
{
    return coil_info.at(coil::quad).first;
}

std::string fp_nms::sum_coil_in_params::header()
{
    return std::string{"SUMMED FIELD PARAMETERS"};
}

std::vector<pid_nms::param_info> fp_nms::sum_coil_in_params::get_param_info()
{
    std::vector<std::string> coil_nms {"Quadrupole", "Octupole", "Pinch", "Bucking"};
    std::map<std::string, coil> str_to_coil {{"Quadrupole", coil::quad}, {"Octupole", coil::oct}, {"Pinch", coil::pinch}, {"Bucking", coil::buck}};
    std::vector<std::string> comp_nms {"x", "y", "z"};
    std::map<std::string, bool> str_to_tf {{"TRUE", true}, {"FALSE", false}};
    
    std::vector<pid_nms::param_info> param_info_vec;
    
    for(size_t idx = 0; idx < coil_nms.size(); ++idx) {
        param_info_vec.push_back
        ({
            coil_nms[idx] + std::string{" Directory"},
            ((!idx) ? "All directory paths are relative to \"large_storage_dir\" in Global_Info.h." : ""),
            gqt_nms::fnc_true,
            [this, str_to_coil, coil_nms, idx](std::string str){
                (this->coil_info)[str_to_coil.at(coil_nms[idx])].first = (global_nms::large_storage_dir / boost::filesystem::path{str} / global_nms::default_opts_fn);
            }
        });
    }
    
    for(size_t idx = 0; idx < coil_nms.size(); ++idx) {
        param_info_vec.push_back
        ({
            coil_nms[idx] + std::string{" Current"},
            ((!idx) ? "All currents are in Amperes." : ""),
            gqt_nms::fnc_true,
            [this, str_to_coil, coil_nms, idx](std::string str){
                gqt_nms::str_to_data(str, &((this->coil_info)[str_to_coil.at(coil_nms[idx])].second));
            }
        });
    }
    
    for(size_t idx = 0; idx < comp_nms.size(); ++idx) {
        param_info_vec.push_back
        ({
            std::string{"Offset field's "} + comp_nms[idx] + std::string{"-component"},
            ((!idx) ? "All field components are in Tesla." : ""),
            gqt_nms::fnc_true,
            [this, idx](std::string str){gqt_nms::str_to_data(str, &((this->offset_field)[idx]));}
        });
    }
    
    param_info_vec.push_back
    ({
        "Include Gradients",
        "TRUE or FALSE",
        gqt_nms::fnc_true,
        [this, str_to_tf](std::string str){(this->grads) = str_to_tf.at(str);}
    });
    
    return param_info_vec;
}

void fp_nms::sum_coil_in_params::post_read_ops(boost::program_options::variables_map)
{}


/* Constant Strings */
const std::string fp_nms::scop_header {"SINGLE-COIL FIELD STORAGE"};
const std::string fp_nms::scop_prompt {"Single-Coil Field Output Directory"};

const std::string fp_nms::smcop_header {"SUMMED FIELD STORAGE"};
const std::string fp_nms::smcop_prompt {"Summed Field Output Directory"};

const std::string fp_nms::dqop_header {"DERIVED QUANTITIES STORAGE"};
const std::string fp_nms::dqop_prompt {"Derived Quantities Output Directory"};



