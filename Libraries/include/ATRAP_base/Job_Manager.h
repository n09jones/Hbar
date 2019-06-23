#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <string>
#include <boost/filesystem.hpp>
#include "Program_Options_Wrapper.h"

namespace jm_nms {
    
    /* Exception for functions in this header */
    class jm_exc {
    public:
        jm_exc(std::string& msg): err_message{msg} {}
        jm_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    /* Options for how to execute a program */
    enum class exec_loc {loc, loc_part_calc, cluster, cluster_single};
    
    /* Options for getting the options file name and for printing a template options file */
    extern const std::vector<pow_nms::mob_desc> opts_fn_opts;
    
    /* File option determining the number of partitions to use */
    extern const std::vector<pow_nms::mob_desc> tot_parts_opt;
    
    /* Command line options determining how and where to execute a program */
    extern const std::vector<pow_nms::mob_desc> vis_run_opts;
    extern const std::vector<pow_nms::mob_desc> hid_run_opts;
    
    /* Execute program */
    int pseudo_main(int, char **, std::function<void(int, char**, size_t, size_t)>, std::function<std::vector<std::pair<boost::filesystem::path, bool>>(const boost::filesystem::path&)>, std::function<void(const boost::filesystem::path&)>, std::function<void(const boost::filesystem::path&)>, std::function<void()> = [](){});
    void exec_prgrm(int, char **, std::function<void(int, char**, size_t, size_t)>, const std::vector<std::pair<boost::filesystem::path, bool>>&);
    
    /* Get execution options */
    exec_loc get_exec_loc(int, char **);
    size_t get_part_idx(int, char **);
    boost::filesystem::path get_opts_fn(int, char **);
    
    /* Get next jobs list */
    std::list<std::pair<size_t, size_t>> extract_intervals(std::list<size_t>);
    std::list<std::list<std::pair<size_t, size_t>>> miss_ints(const std::vector<std::pair<boost::filesystem::path, bool>>&, size_t);
    
    /* Print job options */
    void print_job_opts();
    
    /* Cluster-dependent definitions, provided by a header in "Cluster_Implementations" */
    extern const std::vector<pow_nms::mob_desc> Impl_opts;
    void submit_batch_job(int, char **, std::list<std::list<std::pair<size_t, size_t>>>);
    void submit_single_job(int, char **);
}

#endif /* JOB_MANAGER_H */
