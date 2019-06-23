#ifndef VEC_MATH_HPP
#define VEC_MATH_HPP

#include <cmath>

namespace vm_defs_nms
{
    template <class T, class U, class IP, class CP>
    struct gen_vec_ops
    {
        friend IP inner_pr(const T& A, const U& B)
        {
            return (((A[0]*B[0]) + (A[1]*B[1])) + (A[2]*B[2]));
        }
        
        friend CP cross_pr(const T& A, const U& B)
        {
            CP ret;
            for(int comp = 0; comp < 3; ++comp) {
                ret[comp] = (A[(comp+1)%3]*B[(comp+2)%3]) - (A[(comp+2)%3]*B[(comp+1)%3]);
            }
            return ret;
        }
    };
    
    template <class T, class t>
    struct self_vec_ops
    {
        friend t norm_sq(const T& A)
        {
            return inner_pr(A, A);
        }
        
        friend t norm(const T& A)
        {
            return sqrt(norm_sq(A));
        }
        
        T& normalize()
        {
            T& derived = static_cast<T&>(*this);
            derived /= norm(derived);
            return derived;
        }
        
        friend T normalize(const T& A)
        {
            T ret {A};
            ret.normalize();
            return ret;
        }
    };
}

#endif /* VEC_MATH_HPP */

