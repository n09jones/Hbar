#include "Laser_Paths.h"

/* laser_path_fn functions */
std::vector<pid_nms::param_info> las_path_nms::laser_path_fn::get_param_info()
{
    std::vector<pid_nms::param_info> param_info_vec
    {
        {
            "Laser Path " + std::to_string(path_idx) + " File",
            "",
            gqt_nms::fnc_false,
            [this](std::string str) {this->fn = str;}
        }
    };
    
    return param_info_vec;
}

las_path_nms::laser_path_fn::laser_path_fn(const boost::filesystem::path& opts_fn, size_t path_idx_in) :
path_idx {path_idx_in}
{
    if(!path_idx) {throw las_path_exc{"Path index must be positive"};}
    gqt_nms::fill_params(opts_fn, *this, val_str);
}


/* Laser path functions */
std::vector<pid_nms::param_info> las_path_nms::laser_path::get_param_info()
{
    std::vector<pid_nms::param_info> param_info_vec
    {
        {
            "Power",
            "In Watts, Uni-directional if in buildup cavity",
            gqt_nms::fnc_true,
            [this](std::string str) {gqt_nms::str_to_data(str, &(this->one_way_P));}
        }
    };
    
    std::vector<std::string> comp_labels {"_x", "_y", "_z"};
    for(unsigned comp = 0; comp < 3; ++comp)
    {
        param_info_vec.push_back
        ({
            "Wave vector" + comp_labels[comp],
            (comp ? "" : "Vector will be normalized after input"),
            gqt_nms::fnc_true,
            [this, comp](std::string str) {gqt_nms::str_to_data(str, &(this->unit_wave_vector[comp]));}
        });
    }
    
    for(unsigned comp = 0; comp < 3; ++comp)
    {
        param_info_vec.push_back
        ({
            "Focus" + comp_labels[comp],
            (comp ? "" : "Coordinates in meters"),
            gqt_nms::fnc_true,
            [this, comp](std::string str) {gqt_nms::str_to_data(str, &(this->focus[comp]));}
        });
    }
    
    param_info_vec.push_back
    ({
        "Beam waist (radius)",
        "In meters",
        gqt_nms::fnc_true,
        [this](std::string str) {gqt_nms::str_to_data(str, &(this->waist));}
    });
    
    std::vector<std::string> re_im_labels {"real part", "imaginary part"};
    for(unsigned comp = 0; comp < 6; ++comp)
    {
        param_info_vec.push_back
        ({
            "Polarization" + comp_labels[comp / 2] + " " + re_im_labels[comp % 2],
            (comp ? "" : "Vector will be normalized after input"),
            gqt_nms::fnc_true,
            [this, comp](std::string str) {
                global_nms::default_data_type tmp;
                gqt_nms::str_to_data(str, &tmp);
                if(comp % 2) {this->pol[comp/2].imag(tmp);}
                else{this->pol[comp/2].real(tmp);}
            }
        });
    }
    
    return param_info_vec;
}

void las_path_nms::laser_path::post_read_ops(boost::program_options::variables_map vm)
{
    if(one_way_P <= 0) {throw las_path_exc{"Power should be positive"};}
    if(!norm_sq(unit_wave_vector)) {throw las_path_exc{"Unit wave vector can't be zero"};}
    if(waist <= 0) {throw las_path_exc{"Beam radius should be positive"};}
    if(pol == std::array<std::complex<global_nms::default_data_type>, 3>{{0, 0, 0}}) {
        throw las_path_exc{"Polarization can't be zero"};
    }
    
    unit_wave_vector.normalize();
    
    global_nms::default_data_type pol_norm_sq {0};
    for(auto elem : pol) {pol_norm_sq += norm(elem);}
    for(auto& elem : pol) {elem /= std::sqrt(pol_norm_sq);}
}


/* path_mngr functions */
void las_path_nms::path_mngr::fill_path_mngr(const boost::filesystem::path& opts_fn)
{
    paths.clear();
    
    for(size_t path_idx = 1; (laser_path_fn{opts_fn, path_idx}.fn != ""); ++path_idx) {
        paths.push_back(laser_path{laser_path_fn{opts_fn, path_idx}.fn});
    }
    
    if(paths.empty()) {throw las_path_exc{"No paths selected"};}
}

