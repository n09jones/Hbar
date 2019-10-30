#include <iostream>
#include "ATRAP_base.h"

int main(int argc, char ** argv)
{
    mem_arr_nms::mem_arr<> in_arr {global_nms::large_storage_dir / argv[1]};
    mem_arr_nms::mem_arr<> out_arr_2 {756*176*176};
    mem_arr_nms::mem_arr<> out_arr_5 {303*71*71};
    
    size_t off_0 = 0;
    size_t off_2 = 0;
    size_t off_5 = 0;

    for(size_t z = 0; z < 1511; ++z) {
        for(size_t y = 0; y < 351; ++y) {
            for(size_t x = 0; x < 351; ++x)
            {
                if(((x % 2) == 0) && ((y % 2) == 0) && ((z % 2) == 0)) {
                    out_arr_2[off_2++] = in_arr[off_0];
                }

                if(((x % 5) == 0) && ((y % 5) == 0) && ((z % 5) == 0)) {
                    out_arr_5[off_5++] = in_arr[off_0];
                }

                ++off_0;
            }
        }
    }

    bool print_q {true};
    if(off_0 != in_arr.size()) {print_q = false; std::cout << "In problem" << std::endl;}
    if(off_2 != out_arr_2.size()) {print_q = false; std::cout << "Out 2 problem" << std::endl;}
    if(off_5 != out_arr_5.size()) {print_q = false; std::cout << "Out 5 problem" << std::endl;}

    if(print_q)
    {
        print_to_file(out_arr_2, global_nms::large_storage_dir / argv[2]);
        print_to_file(out_arr_5, global_nms::large_storage_dir / argv[3]);
    }
}
