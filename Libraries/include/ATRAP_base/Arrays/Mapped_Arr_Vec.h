/*
 */
#ifndef MAPPED_ARR_VEC_H
#define MAPPED_ARR_VEC_H

#include <string>
#include <array>
#include <boost/filesystem.hpp>
#include "Global_Info.h"
#include "Basic_Vec.h"
#include "Arrays/Mapped_Arr.h"
#include "Partitions.h"
#include "Template_Impl/Template_Functions.hpp"
#include "Template_Impl/Math/Cmp_Ops.hpp"

namespace mapped_arr_vec_nms
{
    /* Exception for functions in this header */
    class mapped_arr_vec_exc {
    public:
        mapped_arr_vec_exc(std::string& msg): err_message{msg} {}
        mapped_arr_vec_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class T = global_nms::default_data_type, class U = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    class mapped_arr_vec
    {
    private:
        /* Underlying data */
        std::array<mapped_arr_nms::mapped_arr<T>, 3> comp_arrs;
        
    public:
        typedef T fund_type;
        typedef mapped_arr_nms::mapped_arr<T> value_type;
        typedef T* cmp_type;
        
        /* Default constructor */
        mapped_arr_vec() {}
        
        /* Constructor for associating files to a mapped_arr_vec */
        mapped_arr_vec(const boost::filesystem::path&, bool, prt_nms::partition = prt_nms::partition{});
        
        /* Iterator interface */
        typedef decltype(comp_arrs.cbegin()) const_iterator;
        const_iterator cbegin() const {return comp_arrs.cbegin();}
        const_iterator cend() const {return comp_arrs.cend();}
        
        /* Access data */
        const value_type* data() const {return comp_arrs.data();}
        
        constexpr size_t size() const {return comp_arrs.size();}
        size_t arr_len() const {return comp_arrs[0].size();}
        
        /* Subscripting */
        const mapped_arr_nms::mapped_arr<T>& operator[] (int comp) const {return comp_arrs[comp];}
        
        /* Slicing */
        bvec_nms::basic_vec<T> slice(size_t idx) const {return bvec_nms::basic_vec<T>{comp_arrs[0][idx], comp_arrs[1][idx], comp_arrs[2][idx]};}
    };
}

#include "Template_Impl/Arrays/Mapped_Arr_Vec.hpp"

#endif /* MAPPED_ARR_VEC_H */

