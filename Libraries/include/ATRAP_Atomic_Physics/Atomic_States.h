#ifndef ATOMIC_STATES_H
#define ATOMIC_STATES_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"

namespace asts_nms
{
    /* Exception for functions in this header */
    class asts_exc {
    public:
        asts_exc(const std::string& msg): err_message{msg} {}
        asts_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    using nm_rng_t = std::vector<std::pair<std::string, std::function<std::vector<int>(const std::map<std::string, int>&)>>>;
    
    extern const nm_rng_t S_nm_rng;
    extern const nm_rng_t P_nm_rng;
    
    template <int N, const nm_rng_t * Nm_Rng>
    struct ket
    {
        std::map<std::string, int> val_map;
        
        void consistency_check();
        std::string header() {return ("ATOMIC STATE : n = " + std::to_string(N));}
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map) {consistency_check();}
        
        ket() {}
        
        ket(const boost::filesystem::path& opts_fn) {fill(opts_fn);}
        void fill(const boost::filesystem::path&);
        
        ket(const std::map<std::string, int>& mp) {fill(mp);}
        void fill(const std::map<std::string, int>& mp) {val_map = mp; consistency_check();}
        
        friend bool operator==(const ket& lhs, const ket& rhs) {
            return (std::equal(lhs.val_map.begin(), lhs.val_map.end(), rhs.val_map.begin()) && (lhs.val_map.size() == rhs.val_map.size()));
        }
        friend bool operator!=(const ket& lhs, const ket& rhs) {return !(lhs == rhs);}
        
        friend bool operator<(const ket& lhs, const ket& rhs)
        {
            auto basis_vec {ket::basis()};
            auto it_lhs {std::find(basis_vec.begin(), basis_vec.end(), lhs)};
            
            return (std::find(++it_lhs, basis_vec.end(), rhs) != basis_vec.end());
        }
        
        friend bool operator>(const ket& lhs, const ket& rhs) {return !(lhs == rhs || lhs < rhs);}
        
        int operator() (const std::string& key) const {return val_map.at(key);}
        
        static std::vector<ket> basis(std::map<std::string, int> = {});
    };
    
    using S_gr = ket<1, &S_nm_rng>;
    using S_ex = ket<2, &S_nm_rng>;
    using P_ex = ket<2, &P_nm_rng>;
}

#endif /* ATOMIC_STATES_H */

