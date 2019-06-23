#include <iostream>
#include "Program_Options_Wrapper.h"
#include "Job_Manager.h"

using boost::filesystem::path;
using namespace boost::program_options;


/******************************************/
/*     CHECK INTER-OPTION DEPENDENCES     */
/******************************************/

/*
 Inputs:
 - opt_vec: Vector of pairs, each containing a variables_map and a string describing one of the options stored in the variables_map
 - dependence: opt_dep::excl if the options in opt_vec must be mutually exclusive, opt_dep::bound if we must have either all or none of the options in opt_vec specified
 
 Function:
 - Throw if opt_vec and dependence disagree.
 
 Output:
 - None
 */
void pow_nms::dep_opts(const std::vector<std::pair<variables_map, std::string>>& opt_vec, pow_nms::opt_dep dependence)
{
    size_t opt_ct {0};
    for(const auto& opt: opt_vec) {
        if(opt.first.count(opt.second.c_str()) && !opt.first[opt.second.c_str()].defaulted()) {
            opt_ct++;
        }
    }
    
    if((opt_dep::excl == dependence) && (opt_ct > 1)) {
        throw pow_exc{"Options are suppose to be mutually exclusive"};
    }
    if((opt_dep::bound == dependence) && (opt_ct != opt_vec.size()) && opt_ct) {
        throw pow_exc{"All or none of the options need to be specified"};
    }
}




/*********************************************/
/*     BUILD OPTIONS_DESCRIPTION OBJECTS     */
/*********************************************/

/*
 Inputs:
 - desc: Reference to the options_description object to be modified
 - vec(_vec): Container for options to add
 
 Function:
 - Add the options described by the second argument to desc.
 
 Output:
 - Reference to desc
 */
options_description& pow_nms::append_desc_vec(options_description& desc, const std::vector<pow_nms::mob_desc>& vec)
{
    for(const pow_nms::mob_desc& v: vec) {
        if(std::get<2>(v).empty()) {desc.add_options() (std::get<0>(v).c_str(), std::get<1>(v)());}
        else if(!std::get<1>(v)) {desc.add_options() (std::get<0>(v).c_str(), std::get<2>(v).c_str());}
        else {desc.add_options() (std::get<0>(v).c_str(), std::get<1>(v)(), std::get<2>(v).c_str());}
    }
    return desc;
}

options_description& pow_nms::append_desc_vec(options_description& desc, const std::vector<std::vector<pow_nms::mob_desc>>& vec_vec)
{
    for(const std::vector<pow_nms::mob_desc>& vec: vec_vec) {append_desc_vec(desc, vec);}
    return desc;
}




/************************************/
/*     GET COMMAND LINE OPTIONS     */
/************************************/

/*
 Inputs:
 - argc: Number of command line arguments
 - argv: Command line arguments
 ( - desc: Options description giving the information to look for )
 ( - vec: Container giving the information to look for )
 ( - vec_vec: Container giving the information to look for )
 
 Function:
 - Parse argv using the third argument, and store the recorded values.
 
 Output:
 - Variables_map containing the recorded values
 */
variables_map pow_nms::get_cmd_opt(int argc, char ** argv, const options_description& desc)
{
    command_line_parser parser{argc, argv};
    parsed_options opts = parser.options(desc).allow_unregistered().run();
    
    variables_map vm;
    store(opts, vm);
    notify(vm);
    
    return vm;
}

variables_map pow_nms::get_cmd_opt(int argc, char ** argv, const std::vector<pow_nms::mob_desc>& vec)
{
    options_description desc{};
    return get_cmd_opt(argc, argv, append_desc_vec(desc, vec));
}
variables_map pow_nms::get_cmd_opt(int argc, char ** argv, const std::vector<std::vector<pow_nms::mob_desc>>& vec_vec)
{
    options_description desc{};
    return get_cmd_opt(argc, argv, append_desc_vec(desc, vec_vec));
}




/****************************/
/*     GET FILE OPTIONS     */
/****************************/

/*
 Inputs:
 - fn: Path of the file to parse
 ( - desc: Options description giving the information to look for )
 ( - vec: Container giving the information to look for )
 ( - vec_vec: Container giving the information to look for )
 
 Function:
 - Parse fn using the second argument, and store the recorded values.
 
 Output:
 - Variables_map containing the recorded values
 */
variables_map pow_nms::get_file_opt(const path& fn, const options_description& desc)
{
    variables_map vm;
    store(parse_config_file<char>(fn.string().c_str(), desc, true), vm);
    notify(vm);
    
    return vm;
}

variables_map pow_nms::get_file_opt(const path& fn, const std::vector<pow_nms::mob_desc>& vec)
{
    options_description desc{};
    return get_file_opt(fn, append_desc_vec(desc, vec));
}

variables_map pow_nms::get_file_opt(const path& fn, const std::vector<std::vector<pow_nms::mob_desc>>& vec_vec)
{
    options_description desc{};
    return get_file_opt(fn, append_desc_vec(desc, vec_vec));
}


/*
 Inputs:
 - argc: Number of command line arguments
 - argv: Command line arguments
 ( - desc: Options description giving the information to look for )
 ( - vec: Container giving the information to look for )
 ( - vec_vec: Container giving the information to look for )
 
 Function:
 - Parse the options file specified in the command line using the third argument, and store the recorded values.
 
 Output:
 - Variables_map containing the recorded values
 */
variables_map pow_nms::get_file_opt_from_cmd(int argc, char ** argv, const options_description& desc)
{
    return get_file_opt(jm_nms::get_opts_fn(argc, argv), desc);
}

variables_map pow_nms::get_file_opt_from_cmd(int argc, char ** argv, const std::vector<pow_nms::mob_desc>& vec)
{
    options_description desc{};
    return get_file_opt_from_cmd(argc, argv, append_desc_vec(desc, vec));
}

variables_map pow_nms::get_file_opt_from_cmd(int argc, char ** argv, const std::vector<std::vector<pow_nms::mob_desc>>& vec_vec)
{
    options_description desc{};
    return get_file_opt_from_cmd(argc, argv, append_desc_vec(desc, vec_vec));
}

