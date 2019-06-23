#ifndef MAPPED_ARR_HPP
#define MAPPED_ARR_HPP

#include "Global_Info.h"
#include "File_IO.h"


/************************/
/*     CONSTRUCTORS     */
/************************/

/*
 Inputs:
 - fn: Path of the file to be mapped
 - tmp_copy: True if a temporary copy of fn should be made before mapping
 - part: Partition corresponding to the part of fn that should be mapped
 
 Function:
 - Map the selected subset of fn.
 */
template <class T, class U>
mapped_arr_nms::mapped_arr<T, U>::mapped_arr(const boost::filesystem::path& fn, bool tmp_copy, prt_nms::partition part) :
mapped_src_ptr {new boost::iostreams::mapped_file_source {}}, tmp_info_ptr {nullptr}
{
    size_t full_len = fio_nms::data_len<T>(fn);
    if(part.is_def_init()) {part = prt_nms::partition{full_len};}
    
    len = part.get_elems_in_part();
    size_t offset = part.get_first_elem_idx();
    if(full_len != part.get_tot_elems()) {throw mapped_arr_exc{"Wrong number of elements in file"};}
    
    if(tmp_copy && global_nms::enable_mapped_temp_files) {
        tmp_info_ptr = std::unique_ptr<tmp_file_nms::tmp_file_mngr>{new tmp_file_nms::tmp_file_mngr{fn}};
        ptr = fio_nms::map_file<T>(tmp_info_ptr->get_tmp_fn(), *mapped_src_ptr, len, offset);
    }
    else {
        ptr = fio_nms::map_file<T>(fn, *mapped_src_ptr, len, offset);
    }
}

#endif /* MAPPED_ARR_HPP */
