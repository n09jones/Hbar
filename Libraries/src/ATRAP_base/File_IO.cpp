#include <fstream>
#include <iomanip>
#include "File_IO.h"
#include "Global_Info.h"

using boost::filesystem::path;

namespace
{
    /* Ensure that the types parameterizing file sizes and array lengths are large enough. */
    const unsigned long long max_TB {10ULL};
    const unsigned long long bytes_in_TB {(1024ULL)*(1024ULL)*(1024ULL)*(1024ULL)};
    
    static_assert(std::numeric_limits<size_t>::max()/(max_TB*bytes_in_TB), "The type size_t cannot hold indices for max_TB terabytes.");
    static_assert(std::numeric_limits<uintmax_t>::max()/(max_TB*bytes_in_TB), "The type uintmax_t cannot hold indices for max_TB terabytes.");
    static_assert(std::numeric_limits<std::streamoff>::max()/(max_TB*bytes_in_TB), "The type streamoff cannot hold indices for max_TB terabytes.");
    static_assert(std::numeric_limits<std::streamsize>::max()/(max_TB*bytes_in_TB), "The type streamsize cannot hold indices for max_TB terabytes.");
    
    /*
     Inputs:
     - fn: Base path
     - app_str: String to append
     
     Function:
     - If fn is of the form dir/stem.ext, construct the path dir/stem_(app_str).ext.
     
     Output:
     - Constructed path
     */
    path app_before_ext(const path& fn, const std::string& app_str)
    {
        
        path par_dir {fn.parent_path()};
        path filename {fn.stem().string() + std::string{"_"} + app_str + fn.extension().string()};
        return (par_dir / filename);
    }
}

/******************************/
/*     DERIVED FILE NAMES     */
/******************************/

/*
 Inputs:
 - fn_pair.first: Path of a concatonated file
 - fn_pair.second: True if .first is a base path from which component paths can be derived
 ( - tot_parts: Number of partitions )
 - part(_idx): Partition (index) corresponding to the partition file of interest
 
 Function:
 - Derive the partition file path corresponding to fn_pair and part(_idx and tot_parts).
 
 Output:
 - .first: Constructed path
 - .second: fn_pair.second
 */
std::pair<path, bool> fio_nms::part_file_name(std::pair<path, bool> fn_pair, const prt_nms::partition& part)
{
    if(part.is_def_init()) {throw fio_exc{"Cannot use a default initialized partition"};}
    if(1 == part.get_tot_parts()) {return fn_pair;}
    path fn {fn_pair.first};
    path fn_par_dir {app_before_ext((fn.parent_path() / fn.stem()), "partition_files")};
    if(fn_pair.second) {fn_par_dir = app_before_ext(fn_par_dir, "vec");}
    path part_fn {app_before_ext((fn_par_dir / fn.filename()), std::to_string(part.get_part_idx()))};
    return std::pair<path, bool>{part_fn, fn_pair.second};
}

std::pair<path, bool> fio_nms::part_file_name(std::pair<path, bool> fn_pair, size_t tot_parts, size_t part_idx)
{
    return part_file_name(fn_pair, prt_nms::partition{tot_parts, tot_parts, part_idx});
}

/* Vector version of part_file_name(std::pair<path, bool>, const prt_nms::partition&) */
std::vector<std::pair<path, bool>> fio_nms::part_file_name(const std::vector<std::pair<path, bool>>& fn_pair_vec, const prt_nms::partition& part)
{
    std::vector<std::pair<path, bool>> part_fn_pair_vec;
    for(const std::pair<path, bool>& fn_pair: fn_pair_vec) {part_fn_pair_vec.push_back(part_file_name(fn_pair, part));}
    return part_fn_pair_vec;
}

std::vector<std::pair<path, bool>> fio_nms::part_file_name(const std::vector<std::pair<path, bool>>& fn_pair_vec, size_t tot_parts, size_t part_idx)
{
    return part_file_name(fn_pair_vec, prt_nms::partition{tot_parts, tot_parts, part_idx});
}


/*
 Inputs:
 - fn: Base file path
 - comp: Component of interest
 
 Function:
 - If fn is of the form dir/stem.ext, construct the component file path dir/stem_vec/stem_q.ext, where q is the letter representing the axis corresponding to comp.
 
 Output
 - Constructed path
 */
path fio_nms::comp_file_name(const path& fn, int comp)
{
    std::vector<std::string> comp_str {"x", "y", "z"};
    path fn_par_dir {app_before_ext((fn.parent_path() / fn.stem()), "vec")};
    return app_before_ext((fn_par_dir / fn.filename()), comp_str[comp]);
}


/*
 Inputs:
 - fn: Base file path
 
 Function:
 - Find the directory holding the component files corresponding to fn.
 
 Output
 - Directory path
 */
path fio_nms::vec_directory_name(const path& fn)
{
    return comp_file_name(fn, 0).parent_path();
}


/*
 Inputs:
 - fn_pair_vec: Vector of (path, is path a vector base path? boolean) pairs
 
 Function:
 - Extract the file paths from fn_pair_vec, expanding all vector base paths to their three component paths.
 
 Output:
 - Vector of extracted and expanded file paths
 */
std::vector<path> fio_nms::expand_fns(const std::vector<std::pair<path, bool>>& fn_pair_vec)
{
    std::vector<path> fn_vec;
    for(const std::pair<path, bool>& fn_pair: fn_pair_vec) {
        if(fn_pair.second) {
            for(int comp = 0; comp < 3; ++comp) {fn_vec.push_back(comp_file_name(fn_pair.first, comp));}
        }
        else {fn_vec.push_back(fn_pair.first);}
    }
    
    return fn_vec;
}




/*************************************/
/*     PARTITION FILE OPERATIONS     */
/*************************************/

/*
 Inputs:
 - fn_pair_vec: Vector of (path, is path a vector base path? boolean) pairs
 
 Function:
 - Check whether all paths in or expanded from fn_pair_vec exist as regular files.
 - If not, delete all considered paths.
 
 Output
 - true: At least one path considered does not exist as a regular file.
 - false: All paths considered exist as regular files.
 */
bool fio_nms::clear_incomplete_file_set(const std::vector<std::pair<path, bool>>& fn_pair_vec)
{
    bool missing_file = false;
    for(const path& fn: expand_fns(fn_pair_vec)) {missing_file |= !is_regular_file(fn);}
    
    if(missing_file) {
        for(const std::pair<path, bool>& fn_pair: fn_pair_vec) {
            if(fn_pair.second) {remove_all(vec_directory_name(fn_pair.first));}
            else {remove(fn_pair.first);}
        }
    }
    
    return missing_file;
}


/*
 Inputs:
 - fn_pair_vec: Vector of (path, is path a vector base path? boolean) pairs for concatonated file paths
 - tot_parts: Number of partitions
 
 Function:
 - For each valid partition index, check whether the set of partition file paths corresponding to fn_pair_vec all exist as regular files.
 - If they do not, delete any of the paths that exist.
 
 Output:
 - List of partition indices with missing partition files
 */
std::list<size_t> fio_nms::missing_part_files(const std::vector<std::pair<path, bool>>& fn_pair_vec, size_t tot_parts)
{
    std::list<size_t> missing_idxes {};
    for(size_t idx = 0; idx < tot_parts; ++idx) {
        if(clear_incomplete_file_set(part_file_name(fn_pair_vec, tot_parts, idx))) {
            missing_idxes.push_back(idx);
        }
    }
    
    return missing_idxes;
}


/*
 Inputs:
 - fn_pair_vec: Vector of (path, is path a vector base path? boolean) pairs for concatonated file paths
 - tot_parts: Number of partitions
 
 Function:
 - Return if all files corresponding to fn_pair_vec exist; otherwise, delete those that do exist.
 - Ensure all partition files corresponding to the paths in fn_pair_vec are present.
 - Concatonate the partition files.
 - Remove the partition files.
 
 Output:
 - None
 */
void fio_nms::cat_part_files(const std::vector<std::pair<path, bool>>& fn_pair_vec, size_t tot_parts)
{
    if(!clear_incomplete_file_set(fn_pair_vec)) {return;}
    if(missing_part_files(fn_pair_vec, tot_parts).size()) {throw fio_exc{"All files must be present"};}
    
    for (const std::pair<path, bool>& fn_pair: fn_pair_vec) {
        for(int comp = 0; comp < (fn_pair.second ? 3 : 1); ++comp) {
            path out_fn {expand_fns(std::vector<std::pair<path, bool>>{fn_pair}).at(comp)};
            if(exists(out_fn)) {throw fio_exc{"File already exists"};}
            create_directories(out_fn.parent_path());
            std::ofstream ofs {out_fn.string(), std::ios_base::binary | std::ios_base::app};
            ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            
            for(size_t idx = 0; idx < tot_parts; ++idx) {
                path in_fn {expand_fns(part_file_name(std::vector<std::pair<path, bool>>{fn_pair}, tot_parts, idx)).at(comp)};
                std::ifstream ifs {in_fn.string(), std::ios_base::binary};
                ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
                ofs << ifs.rdbuf();
            }
        }
    }
    
    for(const std::pair<path, bool>& fn_pair: fn_pair_vec) {
        remove_all(part_file_name(fn_pair, tot_parts, 0).first.parent_path());
    }
}




/*******************************/
/*     GENERATE FILE NAMES     */
/*******************************/

/*
 Inputs:
 - fn: Base path
 
 Function:
 - Let fn be of the form dir/stem.ext.
 - Starting at 0, increase idx until dir/stem_idx.ext doesn't yet exist.
 
 Output:
 - First dir/stem_idx.ext that doesn't yet exist
 */
path fio_nms::gen_new_idxed_fn(const path& fn)
{
    size_t idx {0};
    while (exists(app_before_ext(fn, std::to_string(idx)))) {++idx;}
    return app_before_ext(fn, std::to_string(idx));
}


/*
 Inputs:
 - src_fn: Path of the temporary's source file
 
 Function:
 - Construct the desired path.
 
 Output:
 - Constructed path
 */
path fio_nms::gen_tmp_fn(path src_fn)
{
    std::string src_ext {src_fn.extension().string()};
    std::string src_str {src_fn.replace_extension().string()};

    for(char& c : src_str) {
	if(!std::isalnum(c)) {c = '_';}
    }
    src_str = "tmp_" + src_str + src_ext;

    return (global_nms::tmp_dir / path{src_str});
}

path fio_nms::gen_tmp_count_fn(const path& src_fn)
{
    return app_before_ext(gen_tmp_fn(src_fn), "count");
}

path fio_nms::gen_tmp_lock_fn(const path& src_fn)
{
    return app_before_ext(gen_tmp_fn(src_fn).replace_extension(".txt"), "lock");
}




/************************/
/*     FORMAT CHECK     */
/************************/

/*
 Inputs:
 - fn: Path to file of interest
 
 Function:
 - Determine fn's format from its extension.
 
 Output:
 - true: fn's extension is ".bin" (so it should be a binary file)
 - false: fn's extension is ".txt" (so it should be a text file)
 */
bool fio_impl_nms::is_bin_file(const boost::filesystem::path& fn)
{
    if(fn.extension().string() == ".bin") {return true;}
    else if(fn.extension().string() == ".txt") {return false;}
    else {throw fio_nms::fio_exc{"Invalid extension"};}
}

