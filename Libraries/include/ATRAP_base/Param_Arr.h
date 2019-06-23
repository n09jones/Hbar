/*
 */
#ifndef PARAM_ARR_H
#define PARAM_ARR_H

#include "Global_Info.h"
#include "Arrays/Mem_Arr.h"
#include "Arrays/Mem_Arr_Vec.h"
#include "Arrays/Mapped_Arr.h"
#include "Arrays/Mapped_Arr_Vec.h"

namespace par_arr_nms
{
    class par_arr_exc {
    public:
        par_arr_exc(std::string& msg): err_message{msg} {}
        par_arr_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    enum class storage {mem, map};
    enum class arr_type {scalar, vector};
    
    template <storage STORAGE, arr_type ARR_TYPE, class T = global_nms::default_data_type>
    using par_arr =
    typename std::conditional<STORAGE == storage::mem,
    typename std::conditional<ARR_TYPE == arr_type::scalar,
    mem_arr_nms::mem_arr<T>,
    mem_arr_vec_nms::mem_arr_vec<T>
    >::type,
    typename std::conditional<ARR_TYPE == arr_type::scalar,
    mapped_arr_nms::mapped_arr<T>,
    mapped_arr_vec_nms::mapped_arr_vec<T>
    >::type
    >::type;
    
    template <storage STORAGE, arr_type ARR_TYPE, class T = global_nms::default_data_type, class = tf_nms::enable_if_tt<STORAGE == storage::mem>>
    par_arr<STORAGE, ARR_TYPE, T> init_par_arr(const boost::filesystem::path&, bool, bool, prt_nms::partition = prt_nms::partition{});
    
    template <storage STORAGE, arr_type ARR_TYPE, class T = global_nms::default_data_type, class = tf_nms::enable_if_tt<STORAGE == storage::map>, class = int>
    par_arr<STORAGE, ARR_TYPE, T> init_par_arr(const boost::filesystem::path&, bool, bool, prt_nms::partition = prt_nms::partition{});
}

#include "Template_Impl/Param_Arr.hpp"

#endif /* PARAM_ARR_H */

