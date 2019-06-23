#ifndef PARAM_ARR_HPP
#define PARAM_ARR_HPP

template <par_arr_nms::storage STORAGE, par_arr_nms::arr_type ARR_TYPE, class T, class>
par_arr_nms::par_arr<STORAGE, ARR_TYPE, T> par_arr_nms::init_par_arr(const boost::filesystem::path& fn, bool tmp_copy, bool constructing, prt_nms::partition part)
{
    if(constructing) {return par_arr_nms::par_arr<STORAGE, ARR_TYPE, T>{part.get_elems_in_part(), 0};}
    return par_arr_nms::par_arr<STORAGE, ARR_TYPE, T>{fn, part};
}

template <par_arr_nms::storage STORAGE, par_arr_nms::arr_type ARR_TYPE, class T, class, class>
par_arr_nms::par_arr<STORAGE, ARR_TYPE, T> par_arr_nms::init_par_arr(const boost::filesystem::path& fn, bool tmp_copy, bool constructing, prt_nms::partition part)
{
    if(constructing) {throw par_arr_exc{"You cannot allocate memory in mapped mode"};}
    return par_arr_nms::par_arr<STORAGE, ARR_TYPE, T>{fn, tmp_copy, part};
}

#endif /* PARAM_ARR_HPP */

