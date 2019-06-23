#ifndef ROTATED_BASES_H
#define ROTATED_BASES_H

#include "ATRAP_base.h"

namespace rotb_nms
{
    /* Exception for functions in this header */
    class rotb_exc {
    public:
        rotb_exc(const std::string& msg): err_message{msg} {}
        rotb_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    struct rot_base
    {
        rot_base(const std::array<bvec_nms::basic_vec<>, 3>&);
        
        /* Rotated frame basis vectors in absolute frame coordinates */
        std::array<bvec_nms::basic_vec<>, 3> basis;
        
        template <class U>
        U rot_to_abs_basis(const U&);
        template <class U>
        U abs_to_rot_basis(const U&);
    };
    
    rot_base gen_rot_base(bvec_nms::basic_vec<>, bvec_nms::basic_vec<> = {1, 0, 0});
}

#include "Template_Impl/Rotated_Bases.hpp"

#endif /* ROTATED_BASES_H */

