#ifndef EM_QUANTS_H
#define EM_QUANTS_H

#include "ATRAP_base.h"
#include "ATRAP_Trap_Quantities.h"

namespace emq_nms
{
    /* Exception for functions in this header */
    class emq_exc {
    public:
        emq_exc(std::string& msg): err_message{msg} {}
        emq_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <ig_nms::interp_type IT = interp_nms::default_interp_type>
    class grid_EM_quants
    {
    private:
        static size_t users;
        static interp_nms::interpolator<IT> interp;
        
        bool is_user;
        
    public:        
        static global_nms::default_data_type Bnorm_min;

        grid_EM_quants() : is_user {false} {}
        
        static void write_opt_file_template(const boost::filesystem::path&);
        
        void initialize(const boost::filesystem::path&);
        void reset_out_of_bnds();
        
        template <ig_nms::interp_quant IQ>
        void EM_quant(std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)>& output, bvec_nms::basic_vec<> pos, const double = 0.0) {
            interp.template calc_trap_quant<IQ, true>(output, pos);
        }
        
        bool out_of_bounds() {return interp.out_of_bounds;}
	bvec_nms::basic_vec<> out_of_bounds_position() {return interp.out_of_bounds_position;}
        
        ~grid_EM_quants();
    };
    
    template <ig_nms::interp_type IT = interp_nms::default_interp_type>
    class grid_EM_quants_changing_currents
    {
    private:
        std::map<std::string, interp_nms::interpolator<IT>> interp_by_coil;
        std::map<std::string, mem_arr_nms::mem_arr<>> curr_by_coil;
        
    public:
        std::vector<std::string> val_str;
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map) {}
        
        static global_nms::default_data_type Bnorm_min;
        
        static void write_opt_file_template(const boost::filesystem::path&);
        
        void initialize(const boost::filesystem::path& opts_fn) {gqt_nms::fill_params(opts_fn, *this, val_str);}
        void reset_out_of_bnds();
        
        template <ig_nms::interp_quant IQ>
        void EM_quant(std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)>&, bvec_nms::basic_vec<>, const double = 0.0);
        
        bool out_of_bounds() {return interp_by_coil.begin()->second.out_of_bounds;}
        bvec_nms::basic_vec<> out_of_bounds_position() {return interp_by_coil.begin()->second.out_of_bounds_position;}
    };
    
    struct prompt_EM_quants
    {
        static global_nms::default_data_type Bnorm_min;

        static void write_opt_file_template(const boost::filesystem::path&) {}
        
        template <class... Args>
        void initialize(Args...) {}
        
        void reset_out_of_bnds() {}
        
        template <ig_nms::interp_quant IQ>
        void EM_quant(std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)>& output, bvec_nms::basic_vec<>, const double = 0.0) {
            std::cout << "Need " << ig_nms::enum_val(IQ) << " components :" << std::endl;
            for(size_t idx = 0; idx < ig_nms::enum_val(IQ); ++idx) {
                std::cout << "index " << idx << " : ";
                std::cin >> output[idx];
            }
            std::cout << std::endl;
        }
        
        bool out_of_bounds() {return false;}
    };
    
    struct one_EM_quants
    {
        static global_nms::default_data_type Bnorm_min;

        static void write_opt_file_template(const boost::filesystem::path&) {}
        
        template <class... Args>
        void initialize(Args...) {}
        
        void reset_out_of_bnds() {}
        
        template <ig_nms::interp_quant IQ>
        void EM_quant(std::array<global_nms::default_data_type, ig_nms::enum_val(IQ)>& output, bvec_nms::basic_vec<>, const double = 0.0) {
            for(size_t idx = 0; idx < ig_nms::enum_val(IQ); ++idx) {
                output[idx] = 1;
            }
        }
        
        bool out_of_bounds() {return false;}
    };
}

#include "Template_Impl/EM_Quants.hpp"

#endif /* EM_QUANTS_H */

