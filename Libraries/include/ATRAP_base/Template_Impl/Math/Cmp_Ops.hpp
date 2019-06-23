#ifndef CMP_OPS_HPP
#define CMP_OPS_HPP

#include <cmath>
#include <string>
#include "Template_Impl/Template_Functions.hpp"

namespace cmp_nms
{
    /* Exception for functions in this header */
    class mops_exc {
    public:
        mops_exc(std::string& msg): err_message{msg} {}
        mops_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
}

/* Define comparison operators. */
template <class T, class U, class = tf_nms::enable_if_tt<tf_nms::mut_cmp<T, U>()>>
bool operator==(const T& x, const U& y)
{
    if(x.size() != y.size()) {throw cmp_nms::mops_exc{"Size mismatch"};}
    auto pred = [](const decltype(x[0])& xx, const decltype(y[0])& yy){return (xx == yy);};
    return std::equal(x.cbegin(), x.cend(), y.cbegin(), pred);
}

template <class T, class U, class = tf_nms::enable_if_tt<tf_nms::mut_cmp<T, U>()>>
bool operator!=(const T& x, const U& y)
{
    return !(x == y);
}

#endif /* CMP_OPS_HPP */

