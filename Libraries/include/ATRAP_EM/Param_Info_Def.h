#ifndef PARAM_INFO_DEF_H
#define PARAM_INFO_DEF_H

#include "ATRAP_base.h"

namespace pid_nms
{
    /* (prompt, optional notes, bool-valued function determining whether the value should be set,
     function taking the input string and setting the internal values) */
    using param_info =  std::tuple<std::string, std::string, std::function<bool()>, std::function<void(std::string)>>;
}

#endif /* PARAM_INFO_DEF_H */
