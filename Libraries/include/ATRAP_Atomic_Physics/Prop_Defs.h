#ifndef PROP_DEFS_H
#define PROP_DEFS_H

#include "ATRAP_base.h"

namespace pdef_nms
{
    struct state_t :
    std::pair<bvec_nms::basic_vec<>, bvec_nms::basic_vec<>>
    {
        state_t() {}
        state_t(const bvec_nms::basic_vec<>& pos, const bvec_nms::basic_vec<>& vel) {
            this->first = pos;
            this->second = vel;
        }
        
        state_t& operator+=(const state_t& rhs) {
            this->first += rhs.first;
            this->second += rhs.second;
            return *this;
        }
        
        friend state_t operator+(state_t lhs, const state_t& rhs) {
            lhs += rhs;
            return lhs;
        }
        
        template <class Sc_T>
        state_t& operator*=(Sc_T a) {
            this->first *= a;
            this->second *= a;
            return *this;
        }
        
        template <class Sc_T>
        friend state_t operator*(Sc_T a, state_t pos_vel) {
            pos_vel *= a;
            return pos_vel;
        }
    };
    
    enum class illum_type {LyAlph, TwoPh};
    
    template <illum_type IT>
    constexpr bool is_LyAlph {(IT == illum_type::LyAlph)};
    
    template <illum_type IT>
    constexpr bool is_TwoPh {(IT == illum_type::TwoPh)};
}

#endif /* PROP_DEFS_H */

