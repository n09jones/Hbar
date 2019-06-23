#ifndef MEM_ARR_VEC_HPP
#define MEM_ARR_VEC_HPP

/************************/
/*     CONSTRUCTORS     */
/************************/


/*
 Inputs:
 - len_in: Desired length of the component mem_arrs
 ( - init_val: Value with which to fill the component mem_arrs )
 ( - init_val_arr: Array of values with which to fill the component mem_arrs )
 
 Functions:
 - Allocate memory for component mem_arrs of lengths len_in.
 ( - Initialize the component mem_arrs. )
 */
template <class T, class U>
mem_arr_vec_nms::mem_arr_vec<T, U>::mem_arr_vec(size_t len_in) : comp_arrs {{mem_arr_nms::mem_arr<T>{len_in}, mem_arr_nms::mem_arr<T>{len_in}, mem_arr_nms::mem_arr<T>{len_in}}} {}

template <class T, class U>
mem_arr_vec_nms::mem_arr_vec<T, U>::mem_arr_vec(size_t len_in, T init_val) : mem_arr_vec<T>{len_in, {init_val, init_val, init_val}} {}

template <class T, class U>
mem_arr_vec_nms::mem_arr_vec<T, U>::mem_arr_vec(size_t len_in, const bvec_nms::basic_vec<T>& init_val_arr)
{
    for(int idx = 0; idx < 3; ++idx) {
        comp_arrs[idx] = std::move(mem_arr_nms::mem_arr<T>{len_in, init_val_arr[idx]});
    }
}


/*
 Inputs:
 - marr: Desired first, second, and third component of *this
 ( - marr_1: Desired first component of *this )
 ( - marr_2: Desired second component of *this )
 ( - marr_3: Desired third component of *this )
 
 Function:
 - Insert the input mem_arr(s) into *this and check that the lengths are compatible.
 */
template <class T, class U>
mem_arr_vec_nms::mem_arr_vec<T, U>::mem_arr_vec(const mem_arr_nms::mem_arr<T>& marr) :
mem_arr_vec {marr, marr, marr} {}

template <class T, class U>
mem_arr_vec_nms::mem_arr_vec<T, U>::mem_arr_vec(mem_arr_nms::mem_arr<T> marr_1, mem_arr_nms::mem_arr<T> marr_2, mem_arr_nms::mem_arr<T> marr_3) : comp_arrs {{std::move(marr_1), std::move(marr_2), std::move(marr_3)}}
{
    for(int comp = 0; comp < 3; ++comp) {
        if((comp_arrs[comp].size() != arr_len()) || !arr_len()) {
            throw mem_arr_vec_exc{"Component file lengths must match and be nonzero"};
        }
    }
}


/*
 Inputs:
 - fn: File path for generating the component paths to be read
 - part: Partition corresponding to the parts of the component files that should be read
 
 Function:
 - Read the appropriate parts of the component files.
 */
template <class T, class U>
mem_arr_vec_nms::mem_arr_vec<T, U>::mem_arr_vec(const boost::filesystem::path& fn, prt_nms::partition part)
{
    for(int comp = 0; comp < 3; ++comp) {
        comp_arrs[comp] = std::move(mem_arr_nms::mem_arr<T>{fio_nms::comp_file_name(fn, comp), part});
        if((comp_arrs[comp].size() != arr_len()) || !arr_len()) {
            throw mem_arr_vec_exc{"Component file lengths must match and be nonzero"};
        }
    }
}



/**********************/
/*     CONVERSION     */
/**********************/

/*
 Inputs:
 - mvec_in: mem_arr_vec to convert
 
 Function:
 - Convert a mem_arr_vec of type U to one of type T.
 
 Output:
 - Converted mem_arr_vec
 */
template <class T, class U>
mem_arr_vec_nms::mem_arr_vec<T> mem_arr_vec_nms::convert(const mem_arr_vec_nms::mem_arr_vec<U>& mvec_in)
{
    return mem_arr_vec<T>{convert<T>(mvec_in[0]), convert<T>(mvec_in[1]), convert<T>(mvec_in[2])};
}



/*************************/
/*     PRINT TO FILE     */
/*************************/

/*
 Inputs:
 - mvec: mem_arr_vec to write
 - fn: File path generating the paths of the component files to write
 
 Function:
 - Write the components of mvec to the corresponding component files.
 
 Output:
 - None
 */
template <class T>
void mem_arr_vec_nms::print_to_file(const mem_arr_vec<T>& mvec, const boost::filesystem::path& fn)
{
    for(int comp = 0; comp < 3; ++comp) {
        mem_arr_nms::print_to_file(mvec[comp], fio_nms::comp_file_name(fn, comp));
    }
}


/**********************/
/*     COMPARISON     */
/**********************/

/*
 Inputs:
 - mvec_1: mem_arr_vec to compare
 - mvec_2: mem_arr_vec to compare
 - diff_lim: Maximum difference allowed
 
 Function:
 - Determine whether corresponding components of mvec_1 and mvec_2 differ by more than diff_lim at any index.
 
 Output:
 - true: |marr_1[comp][idx] - marr_2[comp][idx]| < diff_lim for all valid comp and idx
 - false: otherwise
 */
template <class T>
bool mem_arr_vec_nms::differ_less_than(const mem_arr_vec<T>& mvec_1, const mem_arr_vec<T>& mvec_2, T diff_lim)
{
    auto pred {std::bind(mem_arr_nms::differ_less_than<T>, std::placeholders::_1, std::placeholders::_2, diff_lim)};
    return std::equal(mvec_1.cbegin(), mvec_1.cend(), mvec_2.cbegin(), pred);
}

#endif /* MEM_ARR_VEC_HPP */
