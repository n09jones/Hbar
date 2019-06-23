#ifndef MAPPED_ARR_VEC_HPP
#define MAPPED_ARR_VEC_HPP

/************************/
/*     CONSTRUCTORS     */
/************************/

/*
 Inputs:
 - fn: File path for generating the component paths to be mapped
 - tmp_copy: True if temporary copies should be made before mapping
 - part: Partition corresponding to the parts of the component files that should be mapped
 
 Function:
 - Map the appropriate parts of the component files.
 */
template <class T, class U>
mapped_arr_vec_nms::mapped_arr_vec<T, U>::mapped_arr_vec(const boost::filesystem::path& fn, bool tmp_copy, prt_nms::partition part)
{
    for(int comp = 0; comp < 3; ++comp) {
        comp_arrs[comp] = mapped_arr_nms::mapped_arr<T>{fio_nms::comp_file_name(fn, comp), tmp_copy, part};
        if((comp_arrs[comp].size() != arr_len()) || !arr_len()) {
            throw mapped_arr_vec_exc{"Component file lengths must match and be nonzero"};
        }
    }
}

#endif /* MAPPED_ARR_VEC_HPP */
