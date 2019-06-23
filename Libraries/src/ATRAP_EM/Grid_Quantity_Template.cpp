#include "Grid_Quantity_Template.h"

const std::function<bool()> gqt_nms::fnc_true {[](){return true;}};
const std::function<bool()> gqt_nms::fnc_false {[](){return false;}};

void gqt_nms::print_memory(double mem_in_bytes)
{
    std::cout << std::setprecision(4);
    if(mem_in_bytes < 1024) {std::cout << mem_in_bytes << " B" << std::endl;}
    else if(mem_in_bytes < 1024*1024) {std::cout << mem_in_bytes/1024 << " kB" << std::endl;}
    else if(mem_in_bytes < 1024*1024*1024) {std::cout << mem_in_bytes/(1024*1024) << " MB" << std::endl;}
    else {std::cout << mem_in_bytes/(1024*1024*1024) << " GB" << std::endl;}
}

