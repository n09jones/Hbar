#ifndef GLOBAL_INFO_H
#define GLOBAL_INFO_H

#include <boost/filesystem.hpp>

namespace global_nms
{
    typedef double default_data_type;
    const bool enable_mapped_temp_files {true};
    
    const boost::filesystem::path base_dir { (boost::filesystem::path{__FILE__}).parent_path().parent_path().parent_path().parent_path() };
#include "Global_Info_Loc.h"
    const boost::filesystem::path batch_output_dir { large_storage_dir / boost::filesystem::path{"SLURM_IO"} };
    const boost::filesystem::path coil_dir { base_dir / boost::filesystem::path{"Coil_Defs"} / boost::filesystem::path{"Coil_Def_Data"}};
    const boost::filesystem::path default_opts_fn {"Options_File.txt"};
    const boost::filesystem::path trans_quant_dir {base_dir / boost::filesystem::path{"Transition_Quantity_Generator"} / boost::filesystem::path{"Trans_Quants"}};
    const boost::filesystem::path quick_test_dir { base_dir / "Quick_Test" };
    
    const size_t max_jobs {7500};
    const size_t max_arr_idx {10000};
}

#endif /* GLOBAL_INFO_H */

