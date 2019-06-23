#include "TQ_Array_Manager.h"

template <>
global_nms::default_data_type TQam_nms::extract_from_it<global_nms::default_data_type>(TQam_nms::it_t& it)
{
    return *(it++);
}

template <>
std::array<std::complex<global_nms::default_data_type>, 1> TQam_nms::extract_from_it<std::array<std::complex<global_nms::default_data_type>, 1>>(TQam_nms::it_t& it)
{
    std::array<std::complex<global_nms::default_data_type>, 1> tmp {{{it[0], it[1]}}};
    it += 2;
    return tmp;
}

template <>
std::array<std::complex<global_nms::default_data_type>, 3> TQam_nms::extract_from_it<std::array<std::complex<global_nms::default_data_type>, 3>>(TQam_nms::it_t& it)
{
    std::array<std::complex<global_nms::default_data_type>, 3> tmp {{
        {it[0], it[3]}, {it[1], it[4]}, {it[2], it[5]}
    }};
    it += 6;
    return tmp;
}

template <>
std::array<global_nms::default_data_type, 6> TQam_nms::extract_from_it<std::array<global_nms::default_data_type, 6>>(TQam_nms::it_t& it)
{
    std::array<global_nms::default_data_type, 6> tmp {{it[0], it[1], it[2], it[3], it[4], it[5]}};
    it += 6;
    return tmp;
}

