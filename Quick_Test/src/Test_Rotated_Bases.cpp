#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

template <class U>
void print_array(const U& arr)
{
    std::cout << "(";
    for(size_t idx = 0; idx < arr.size(); ++idx)
    {
        if(idx) {std::cout << ", ";}
        std::cout << arr[idx];
    }
    std::cout << ")";
}

void print_basis(std::string str, rotb_nms::rot_base base)
{
    std::cout << str << " : " << std::endl;
    for(auto elem : base.basis) {
        print_array(elem); std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    print_basis("z-hat = {1, 0, 0}", rotb_nms::gen_rot_base({3, 0, 0}));
    print_basis("z-hat = {-1, 0, 0}", rotb_nms::gen_rot_base({-M_PI, 0, 0}));
    
    print_basis("z-hat = {0, 1, 0}", rotb_nms::gen_rot_base({0, 17.3, 0}));
    print_basis("z-hat = {0, 0, 1}", rotb_nms::gen_rot_base({0, 0, 22}));
    
    print_basis("z-hat = {0, 0, 1}, x-hat = {0, 1, 0}", rotb_nms::gen_rot_base({0, 0, 22}, {0, 5, 0}));
    print_basis("z-hat = {0, 1, 1}, x-hat = {1, 1, 0}", rotb_nms::gen_rot_base({0, 22, 22}, {5, 5, 0}));
    
    std::cout << std::endl << std::endl;
    
    auto basis_1 {rotb_nms::gen_rot_base({0, 22, 22}, {5, 5, 0})};
    
    std::array<std::complex<global_nms::default_data_type>, 3> arr {{{1, 0}, {2, -1}, {0, 0}}};
    
    std::cout << "{{1, 0}, {2, -1}, {0, 0}} rot -> abs : ";
    print_array(basis_1.rot_to_abs_basis(arr));
    std::cout << std::endl << std::endl;
    
    std::cout << "{{1, 0}, {2, -1}, {0, 0}} abs -> rot : ";
    print_array(basis_1.abs_to_rot_basis(arr));
    std::cout << std::endl << std::endl;
    
    return 0;
}
