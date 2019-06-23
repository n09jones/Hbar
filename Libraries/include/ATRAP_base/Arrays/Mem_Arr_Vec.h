/*
 */
#ifndef MEM_ARR_VEC_H
#define MEM_ARR_VEC_H

#include <string>
#include <array>
#include <boost/filesystem.hpp>
#include "Global_Info.h"
#include "File_IO.h"
#include "Partitions.h"
#include "Arrays/Basic_Vec.h"
#include "Arrays/Mem_Arr.h"
#include "Template_Impl/Template_Functions.hpp"
#include "Math/Basic_Arith.h"
#include "Math/Vec_Math.h"
#include "Template_Impl/Math/Cmp_Ops.hpp"

namespace mem_arr_vec_nms
{
    /* Exception for functions in this header */
    class mem_arr_vec_exc {
    public:
        mem_arr_vec_exc(std::string& msg): err_message{msg} {}
        mem_arr_vec_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class T = global_nms::default_data_type, class U = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    class mem_arr_vec:
    public bar_nms::basic_arith<mem_arr_vec<T>, mem_arr_vec<T>, false,
    bar_nms::basic_arith<mem_arr_vec<T>, bvec_nms::basic_vec<T>, false,
    bar_nms::basic_arith<mem_arr_vec<T>, mem_arr_nms::mem_arr<T>, true,
    bar_nms::basic_arith<mem_arr_vec<T>, T, true
    >>>>,
    public vm_nms::vec_math<mem_arr_vec<T>, mem_arr_vec<T>, mem_arr_nms::mem_arr<T>>,
    public vm_nms::vec_math<mem_arr_vec<T>, bvec_nms::basic_vec<T>, mem_arr_nms::mem_arr<T>>
    {
    private:
        /* Underlying data */
        std::array<mem_arr_nms::mem_arr<T>, 3> comp_arrs;
        
    public:
        typedef T fund_type;
        typedef mem_arr_nms::mem_arr<T> value_type;
        typedef T* cmp_type;
        
        /* Default constructor */
        mem_arr_vec() {}
        
        /* Constructors for allocating memory for a mem_arr_vec */
        explicit mem_arr_vec(size_t);
        mem_arr_vec(size_t, T);
        mem_arr_vec(size_t, const bvec_nms::basic_vec<T>&);
        
        /* Constructors for making a mem_arr_vec from mem_arrs. */
        explicit mem_arr_vec(const mem_arr_nms::mem_arr<T>&);
        mem_arr_vec(mem_arr_nms::mem_arr<T>, mem_arr_nms::mem_arr<T>, mem_arr_nms::mem_arr<T>);
        
        /* Constructor for associating files to an mem_arr_vec */
        mem_arr_vec(const boost::filesystem::path&, prt_nms::partition = prt_nms::partition{});
        
        /* Iterator interface */
        typedef decltype(comp_arrs.begin()) iterator;
        iterator begin() {return comp_arrs.begin();}
        iterator end() {return comp_arrs.end();}
        
        typedef decltype(comp_arrs.cbegin()) const_iterator;
        const_iterator begin() const {return comp_arrs.cbegin();}
        const_iterator end() const {return comp_arrs.cend();}
        const_iterator cbegin() const {return comp_arrs.cbegin();}
        const_iterator cend() const {return comp_arrs.cend();}
        
        /* Access data */
        value_type* data() {return comp_arrs.data();}
        const value_type* data() const {return comp_arrs.data();}
        
        constexpr size_t size() const {return comp_arrs.size();}
        size_t arr_len() const {return comp_arrs[0].size();}
        
        /* Subscripting */
        mem_arr_nms::mem_arr<T>& operator[](int comp) {return comp_arrs[comp];}
        const mem_arr_nms::mem_arr<T>& operator[](int comp) const {return comp_arrs[comp];}
        
        /* Slicing */
        bvec_nms::basic_vec<T> slice(size_t idx) const {return bvec_nms::basic_vec<T>{comp_arrs[0][idx], comp_arrs[1][idx], comp_arrs[2][idx]};}
    };
    
    /* Conversion */
    template <class T, class U>
    mem_arr_vec<T> convert(const mem_arr_vec<U>&);
    
    /* Print to File */
    template <class T>
    void print_to_file(const mem_arr_vec<T>&, const boost::filesystem::path&);
    
    /* Comparision */
    template <class T>
    bool differ_less_than(const mem_arr_vec<T>&, const mem_arr_vec<T>&, T);
}

#include "Template_Impl/Arrays/Mem_Arr_Vec.hpp"

#endif /* MEM_ARR_VEC_H */

