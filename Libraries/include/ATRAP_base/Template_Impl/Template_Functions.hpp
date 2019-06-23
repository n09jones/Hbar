#ifndef TEMPLATE_FUNCTIONS_HPP
#define TEMPLATE_FUNCTIONS_HPP

namespace tf_nms
{
    template <bool B>
    using enable_if_tt = typename std::enable_if<B>::type;
    
    template <class T, class U, class = typename std::decay<T>::type::cmp_type, class = typename std::decay<U>::type::cmp_type>
    constexpr bool mut_cmp() {return std::is_same<typename std::decay<T>::type::cmp_type, typename std::decay<U>::type::cmp_type>::value;}
    
    template <class T>
    constexpr bool is_basic() {return (std::is_same<typename std::decay<T>::type, T>::value && std::is_arithmetic<T>::value);}
}

#endif /* TEMPLATE_FUNCTIONS_HPP */

