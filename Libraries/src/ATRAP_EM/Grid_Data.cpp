#include "Grid_Data.h"

std::vector<std::string> gd_nms::base_strs(gd_nms::quant quantity, bool grads)
{
    const std::map<quant, std::string> quant_map {{quant::B, "B"}, {quant::Bnorm, "Bnorm"}, {quant::acc_1Sc, "acc_1Sc"}, {quant::acc_1Sd, "acc_1Sd"}, {quant::acc_2Sc, "acc_2Sc"}, {quant::acc_2Sd, "acc_2Sd"}};
    
    std::vector<std::string> ret_vec {(grads ? std::vector<std::string>{"", "dx", "dy", "dz"} : std::vector<std::string>{""})};
    for(std::string& str : ret_vec) {str += quant_map.at(quantity);}
    return ret_vec;
}


boost::filesystem::path gd_nms::base_str_to_fn(const boost::filesystem::path& output_dir, const std::string& str)
{
    return (output_dir / boost::filesystem::path{str + ".bin"});
}

