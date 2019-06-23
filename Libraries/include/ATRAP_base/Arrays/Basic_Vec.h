#ifndef BASIC_VEC_H
#define BASIC_VEC_H

#include <array>
#include "Template_Impl/Template_Functions.hpp"
#include "Math/Basic_Arith.h"
#include "Math/Vec_Math.h"
#include "Template_Impl/Math/Cmp_Ops.hpp"

namespace bvec_nms
{
    /* Exception for functions in this header */
    class bvec_exc {
    public:
        bvec_exc(std::string& msg): err_message{msg} {}
        bvec_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class T = global_nms::default_data_type, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    struct basic_vec:
    std::array<T, 3>,
    bar_nms::basic_arith<basic_vec<T>, basic_vec<T>, false,
    bar_nms::basic_arith<basic_vec<T>, T, true
    >>,
    vm_nms::vec_math<basic_vec<T>, basic_vec<T>, T>
    {
        typedef T fund_type;
        typedef T cmp_type;
        
        /* Default constructor */
        basic_vec() : basic_vec{0, 0, 0} {}
        
        /* Construction from components */
        basic_vec(T x, T y, T z) : std::array<T, 3> {{x, y, z}} {}
    };
    
    /* Conversion */
    template <class T, class U>
    basic_vec<T> convert(const basic_vec<U>&);

    /* Special Vectors */
    template <class T = global_nms::default_data_type>
    basic_vec<T> unit_vec(unsigned);
}

#include "Template_Impl/Arrays/Basic_Vec.hpp"

#endif /* BASIC_VEC_H */

