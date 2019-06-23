#ifndef PROGRAM_OPTIONS_WRAPPER_H
#define PROGRAM_OPTIONS_WRAPPER_H

#include <string>
#include <tuple>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace pow_nms {
    
    /* Exception for functions in this header */
    class pow_exc {
    public:
        pow_exc(std::string& msg): err_message{msg} {}
        pow_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    /* Type allowing separate specification and initialization of options_description objects */
    typedef std::tuple<const std::string, std::function<boost::program_options::value_semantic*()>, const std::string> mob_desc;
    
    /* Inter-option dependence choices */
    enum class opt_dep {excl, bound};
    
    /* Check inter-option dependences */
    void dep_opts(const std::vector<std::pair<boost::program_options::variables_map, std::string>>&, opt_dep);
    
    /* Build options_description objects */
    boost::program_options::options_description& append_desc_vec(boost::program_options::options_description&, const std::vector<mob_desc>&);
    boost::program_options::options_description& append_desc_vec(boost::program_options::options_description&, const std::vector<std::vector<mob_desc>>&);
    
    /* Get command line options */
    boost::program_options::variables_map get_cmd_opt(int, char **, const boost::program_options::options_description&);
    boost::program_options::variables_map get_cmd_opt(int, char **, const std::vector<mob_desc>&);
    boost::program_options::variables_map get_cmd_opt(int, char **, const std::vector<std::vector<mob_desc>>&);
    
    /* Get file options */
    boost::program_options::variables_map get_file_opt(const boost::filesystem::path&, const boost::program_options::options_description&);
    boost::program_options::variables_map get_file_opt(const boost::filesystem::path&, const std::vector<mob_desc>&);
    boost::program_options::variables_map get_file_opt(const boost::filesystem::path&, const std::vector<std::vector<mob_desc>>&);
    
    boost::program_options::variables_map get_file_opt_from_cmd(int, char **, const boost::program_options::options_description&);
    boost::program_options::variables_map get_file_opt_from_cmd(int, char **, const std::vector<mob_desc>&);
    boost::program_options::variables_map get_file_opt_from_cmd(int, char **, const std::vector<std::vector<mob_desc>>&);
}

#endif /* PROGRAM_OPTIONS_WRAPPER_H */
