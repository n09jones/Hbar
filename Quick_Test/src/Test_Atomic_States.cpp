#include <iostream>
#include "ATRAP_base.h"
#include "ATRAP_Atomic_Physics.h"

using asts_nms::S_gr;
using asts_nms::S_ex;
using asts_nms::P_ex;
using boost::filesystem::path;

template <int N, const asts_nms::nm_rng_t * Nm_Rng>
void print_ket(const asts_nms::ket<N, Nm_Rng>& k)
{
    for(const auto& key_val : *Nm_Rng) {
        std::cout << key_val.first << " : " << k(key_val.first) << "     ";
    }
    std::cout << std::endl;
}

template <class U>
void print_basis(std::string str)
{
    std::cout << str << " :" << std::endl;
    for(const auto& k : U::basis()) {print_ket(k);}
    std::cout << std::endl;
}

int main(int argc, char ** argv)
{
    std::cout << "******* BASES *******" << std::endl;
    
    print_basis<asts_nms::S_gr>("S_gr");
    print_basis<asts_nms::S_ex>("S_ex");
    print_basis<asts_nms::P_ex>("P_ex");
    
    std::cout << std::endl << std::endl;
    
    
    std::cout << "******* LOADED KETS *******" << std::endl << std::endl;
    
    path opts_fn {global_nms::quick_test_dir / path{"Options_Files"} / path{"Atomic_States_Opts_File.txt"}};
    
    S_gr S_gr_ket {opts_fn};
    S_ex S_ex_ket {opts_fn};
    P_ex P_ex_ket {opts_fn};
    
    std::cout << "S_gr_ket --- ";
    print_ket(S_gr_ket);
    std::cout << "S_ex_ket --- ";
    print_ket(S_ex_ket);
    std::cout << "P_ex_ket --- ";
    print_ket(P_ex_ket);
    
    if(
       (S_gr_ket != S_gr{{{"F", 1}, {"mF", 0}}}) || (S_gr_ket == S_gr{{{"F", 1}, {"mF", -1}}}) ||
       (S_ex_ket != S_ex{{{"F", 0}, {"mF", 0}}}) || (S_ex_ket == S_ex{{{"F", 1}, {"mF", -1}}}) ||
       (P_ex_ket != P_ex{{{"2J", 3}, {"2mJ", -1}, {"2mI", -1}}}) || (P_ex_ket == P_ex{{{"2J", 1}, {"2mJ", -1}, {"2mI", 1}}})
       ) {throw asts_nms::asts_exc{"Comparison operators did not work as expected."};}
    
    return 0;
}
