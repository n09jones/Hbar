#ifndef SLURM_IMPL_HPP
#define SLURM_IMPL_HPP

#include <string>
#include "Job_Manager.h"
#include "Global_Info.h"
#include "File_IO.h"

namespace SLURM_jm_nms {
    
    /* Maximum characters in the job array...cuts off additions once it is exceeded */
    const size_t SLURM_soft_max_arr_char {2900};
    
    /* Default SLURM resource requests */
    const size_t SLURM_mem_MB_def {1000};
    const size_t SLURM_time_min_def {60};
    const std::string SLURM_part_def {"shared"};
    const std::string SLURM_job_name_def {"SLURM_job"};
    
    /*
     Inputs:
     - job_ints: Reference to a list of job intervals
     
     Function:
     - Build a string of the job intervals in job_ints.
     - Stop if SLURM_soft_max_arr_char is exceeded.
     - Remove added intervals from job_ints.
     
     Output:
     - Constructed string
     */
    std::string SLURM_idx_str(std::list<std::pair<size_t, size_t>>& job_ints)
    {
        auto it = job_ints.begin();
        std::string arr_str {};
        
        for(const auto& intvl: job_ints) {
            arr_str += std::to_string(intvl.first % global_nms::max_arr_idx);
            if(intvl.first != intvl.second) {
                arr_str += std::string{"-"} + std::to_string(intvl.second % global_nms::max_arr_idx);
            }
            arr_str += ",";
            it++;
            if(arr_str.length() > SLURM_soft_max_arr_char) {break;}
        }
        
        arr_str.pop_back();
        job_ints.erase(job_ints.begin(), it);
        
        return arr_str;
    }
    
    /*
     Inputs:
     - argc: Number of command line arguments
     - argv: Command line arguments
     - single_job: True if a single job is submitted, false if a job array is submitted
     - job_ints_str: String listing intervals of member indices of jobs to submit
     - part_group_idx: Partition group index for the submitted jobs
     
     Function:
     - Write and submit a SLURM script.
     
     Output:
     - None
     */
    void submit_SLURM_script(int argc, char ** argv, bool single_job, std::string job_ints_str = "", size_t part_group_idx = 0)
    {
        boost::program_options::variables_map vm {pow_nms::get_file_opt_from_cmd(argc, argv, jm_nms::Impl_opts)};
        boost::filesystem::path out_fn {vm["Job Name"].as<std::string>() + std::string{"_%A_%a.out"}};
        boost::filesystem::path err_fn {vm["Job Name"].as<std::string>() + std::string{"_%A_%a.err"}};
        
        boost::filesystem::path fn {fio_nms::gen_new_idxed_fn(global_nms::batch_output_dir / boost::filesystem::path{"SLURM_script.txt"})};
        
        std::ofstream ofs {fn.string()};
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        
        ofs << "#!/bin/bash" << std::endl;
        ofs << "#SBATCH -n 1" << std::endl;
        ofs << "#SBATCH -N 1" << std::endl;
        ofs << "#SBATCH -t " << vm["Time (in mins)"].as<size_t>() << std::endl;
        ofs << "#SBATCH --mem=" << vm["Memory (in MB)"].as<size_t>() << std::endl;
        ofs << "#SBATCH -p " << vm["SLURM partitions"].as<std::string>() << std::endl;
        if(!single_job) {ofs << "#SBATCH --array=" << job_ints_str << std::endl;}
        ofs << "#SBATCH -o " << (global_nms::batch_output_dir / out_fn).string() << std::endl;
        ofs << "#SBATCH -e " << (global_nms::batch_output_dir / err_fn).string() << std::endl;
        ofs << std::endl;
        
        for(int k = 0; k < argc; ++k) {
            if((std::string{argv[k]} != "--cluster") && (std::string{argv[k]} != "--cluster_single")) {
                ofs << argv[k];
                if(k+1 != argc) {ofs << " ";}
            }
        }
        
        if(!single_job) {
            ofs << " --part_group_idx=" << part_group_idx;
            ofs << " --part_member_idx=${SLURM_ARRAY_TASK_ID}";
        }
        
        ofs << std::endl << std::endl;
        
        ofs.close();
        std::system((std::string{"sbatch "} + fn.string()).c_str());
    }
}


/* File options for SLURM resource requests */
const std::vector<pow_nms::mob_desc> jm_nms::Impl_opts
{
    {"Memory (in MB)", [](){return (boost::program_options::value<size_t>()->default_value(SLURM_jm_nms::SLURM_mem_MB_def));}, "Memory per node in MB"},
    {"Time (in mins)", [](){return (boost::program_options::value<size_t>()->default_value(SLURM_jm_nms::SLURM_time_min_def));}, "Time per job in minutes"},
    {"SLURM partitions", [](){return (boost::program_options::value<std::string>()->default_value(SLURM_jm_nms::SLURM_part_def));}, "Comma-separated list (no spaces) of SLURM partitions to use"},
    {"Job Name", [](){return (boost::program_options::value<std::string>()->default_value(SLURM_jm_nms::SLURM_job_name_def));}, "Job name (no spaces)"}
};


/*
 Inputs:
 - argc: Number of command line arguments
 - argv: Command line arguments
 - job_ints_lst: List of lists of intervals of partition indices corresponding to unfinished jobs
 
 Function:
 - Submit the jobs described by job_ints_lst.
 
 Output:
 - None
 */
void jm_nms::submit_batch_job(int argc, char ** argv, std::list<std::list<std::pair<size_t, size_t>>> job_ints_lst)
{
    while (job_ints_lst.size() && job_ints_lst.front().empty()) {job_ints_lst.pop_front();}
    if(job_ints_lst.empty()) {return;}
    
    size_t part_group_idx {job_ints_lst.front().front().first / global_nms::max_arr_idx};
    std::string job_ints_str {SLURM_jm_nms::SLURM_idx_str(job_ints_lst.front())};
    SLURM_jm_nms::submit_SLURM_script(argc, argv, false, job_ints_str, part_group_idx);
    
    submit_batch_job(argc, argv, job_ints_lst);
}


/*
 Inputs:
 - argc: Number of command line arguments
 - argv: Command line arguments
 
 Function:
 - Submit the job described by the command line arguments.
 
 Output:
 - None
 */
void jm_nms::submit_single_job(int argc, char ** argv)
{
    SLURM_jm_nms::submit_SLURM_script(argc, argv, true);
}

#endif /* SLURM_IMPL_HPP */
