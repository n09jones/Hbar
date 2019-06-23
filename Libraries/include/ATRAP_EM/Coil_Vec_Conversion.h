#ifndef COIL_VEC_CONVERSION_H
#define COIL_VEC_CONVERSION_H

#include "ATRAP_base.h"

namespace cvc_nms
{
    /* Exception for functions in this header */
    class cvc_exc {
    public:
        cvc_exc(std::string& msg): err_message{msg} {}
        cvc_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    mem_arr_nms::mem_arr<> vec_to_coil(const mem_arr_vec_nms::mem_arr_vec<>&);
    mem_arr_vec_nms::mem_arr_vec<> coil_to_vec(const mem_arr_nms::mem_arr<>&);
}

#endif /* COIL_VEC_CONVERSION_H */
