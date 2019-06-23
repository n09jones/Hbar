#include "Laser_Properties.h"

std::vector<pid_nms::param_info> las_props_nms::laser_properties::get_param_info()
{
    std::vector<pid_nms::param_info> param_info_vec
    {
        {
            "Laser Detuning",
            "Laser's detuning, in Hertz, from the resonance when B = Detuning Field",
            gqt_nms::fnc_true,
            [this](std::string str) {gqt_nms::str_to_data(str, &(this->det));}
        },
        {
            "Detuning Field",
            "Field, in Tesla, for which the laser is on-resonance for zero detuning - default is the trap minimum",
            gqt_nms::fnc_false,
            [this](std::string str) {gqt_nms::str_to_data(((str == "") ? "-1" : str), &(this->B_0_det));}
        },
        {
            "Laser FWHM",
            "In Hertz",
            gqt_nms::fnc_true,
            [this](std::string str) {gqt_nms::str_to_data(str, &(this->FWHM));}
        },
        {
            "Pulse Length",
            "In seconds",
            gqt_nms::fnc_false,
            [this](std::string str) {gqt_nms::str_to_data(((str == "") ? "0" : str), &(this->pulse_len));}
        },
        {
            "Repetition Rate",
            "In Hertz",
            gqt_nms::fnc_false,
            [this](std::string str) {gqt_nms::str_to_data(((str == "") ? "0" : str), &(this->rep_rate));}
        }
    };
    
    return param_info_vec;
}

void las_props_nms::laser_properties::post_read_ops(boost::program_options::variables_map vm)
{
    if(FWHM <= 0) {throw las_props_exc{"FWHM should be positive"};}
    
    std::vector<std::pair<boost::program_options::variables_map, std::string>> opt_vec {{vm, "Pulse Length"}, {vm, "Repetition Rate"}};
    pow_nms::dep_opts(opt_vec, pow_nms::opt_dep::bound);
    
    pulsed = !(vm["Pulse Length"].defaulted());
    if(pulsed && (pulse_len <= 0)) {throw las_props_exc{"Pulse length should be positive."};}
    if(pulsed && (rep_rate <= 0)) {throw las_props_exc{"Rep rate should be positive."};}
}

