#ifndef VEC_MATH_H
#define VEC_MATH_H

#include <string>
#include "Template_Impl/Math/Vec_Math.hpp"

namespace vm_nms
{
    template <class T, class U, class t>
    struct vec_math : vm_defs_nms::gen_vec_ops<T, U, t, T>, vm_defs_nms::gen_vec_ops<U, T, t, T>
    {};
    
    template <class T, class t>
    struct vec_math<T, T, t> : vm_defs_nms::gen_vec_ops<T, T, t, T>, vm_defs_nms::self_vec_ops<T, t>
    {};
}

#endif /* VEC_MATH_H */

