#ifndef BASIC_ARITH_H
#define BASIC_ARITH_H

#include <string>
#include <boost/operators.hpp>
#include "Template_Impl/Math/Basic_Arith.hpp"

namespace bar_nms
{
    template <class T, class U, bool Broadcast, class V = bar_defs_nms::arith_empty_base>
    struct basic_arith : bar_defs_nms::basic_arith_raw<T, U, Broadcast, V>
    {};
    
    template <class T1, class U1, bool Broadcast1, class T2, class U2, bool Broadcast2, class V>
    struct basic_arith<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>> :
    bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1,
    basic_arith<T2, U2, Broadcast2, V
    >>
    {
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::operator+=;
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::template basic_arith<T2, U2, Broadcast2, V>::operator+=;
        
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::operator-=;
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::template basic_arith<T2, U2, Broadcast2, V>::operator-=;
        
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::operator*=;
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::template basic_arith<T2, U2, Broadcast2, V>::operator*=;
        
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::operator/=;
        using bar_defs_nms::basic_arith_raw<T1, U1, Broadcast1, basic_arith<T2, U2, Broadcast2, V>>::template basic_arith<T2, U2, Broadcast2, V>::operator/=;
    };
}

#endif /* BASIC_ARITH_H */
