#ifndef TEMPLATE_OPTIONS_FILE_PRINTER_H
#define TEMPLATE_OPTIONS_FILE_PRINTER_H

namespace tofp_nms
{
    /*
     This is an exact duplicate of the function found in Grid_Quantity_Template.hpp. However, that function
     is a member function of a class I don't want to use here. I could probably make it a non-member function,
     but I'd rather not deal with the headache of making sure nothing is screwed up doing that.
     */
    template <class U>
    void app_opt_file(const boost::filesystem::path& opts_fn, U& params, std::vector<std::string> val_str = {})
    {
        if((params.header() == "") && params.get_param_info().empty()) {return;}
        
        std::ofstream ofs {opts_fn.string(), std::ios_base::app};
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        
        ofs << std::string(params.header().length()+4, '#') << std::endl << "# " << params.header() << " #" << std::endl << std::string(params.header().length()+4, '#') << std::endl << std::endl;
        
        for(size_t idx = 0; idx < params.get_param_info().size(); ++idx) {
            if(std::get<1>(params.get_param_info()[idx]).length()) {
                if(idx) {ofs << std::endl;}
                ofs << "# " << std::get<1>(params.get_param_info()[idx]) << std::endl;
            }
            ofs << std::get<0>(params.get_param_info()[idx]) << " = " << (val_str.size() ? val_str[idx] : "") << std::endl;
        }
        ofs << std::endl << std::endl << std::endl;
    }
}

#endif /* TEMPLATE_OPTIONS_FILE_PRINTER_H */

