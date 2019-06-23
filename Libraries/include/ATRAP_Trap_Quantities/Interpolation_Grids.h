#ifndef INTERPOLATION_GRIDS_H
#define INTERPOLATION_GRIDS_H

#include "ATRAP_base.h"
#include "ATRAP_EM.h"

namespace ig_nms
{
    /* Exception for functions in this header */
    class ig_exc {
    public:
        ig_exc(std::string& msg): err_message{msg} {}
        ig_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    constexpr size_t enum_incr {1000};
    
    template<size_t enum_no>
    constexpr size_t enum_assgn(size_t enum_val)
    {
        return ((enum_incr*enum_no) + enum_val);
    }
    
    template <class ENUM_T>
    constexpr size_t enum_val(ENUM_T enum_nm)
    {
        return (static_cast<size_t>(enum_nm) % enum_incr);
    }
    
    enum class interp_type : size_t {zeroth = enum_assgn<1>(1), lin = enum_assgn<2>(4), quad = enum_assgn<3>(10), trilin = enum_assgn<4>(8)};
    enum class interp_quant : size_t {BdB = enum_assgn<1>(12), B = enum_assgn<2>(3), Bnorm = enum_assgn<3>(1), acc_1Sc = enum_assgn<4>(3), acc_1Sd = enum_assgn<5>(3), acc_2Sc = enum_assgn<6>(3), acc_2Sd = enum_assgn<7>(3)};
    
    std::vector<boost::filesystem::path> interp_source_files(const boost::filesystem::path&, interp_quant);
    
    /* interp_grid components */
    struct ig_in_params
    {
        static const bool coord_grid_deduced {true};
        boost::filesystem::path coord_grid_src();
        
        std::string header();
        std::vector<pid_nms::param_info> get_param_info();
        void post_read_ops(boost::program_options::variables_map);
        
        bool grads;
        interp_type it;
        boost::filesystem::path input_dir;
    };
    
    extern const std::string iop_header;
    extern const std::string iop_prompt;
    using interp_out_params = fp_nms::out_params<&iop_header, &iop_prompt>;
    
    template <interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy = true>
    struct interp_data
    {
        typedef par_arr_nms::par_arr<Storage, par_arr_nms::arr_type::scalar> stored_type;
        
        stored_type arr;
        prt_nms::partition part;
        
        void import_data(const boost::filesystem::path&, bool, bool, prt_nms::partition = prt_nms::partition{});
        
        template <par_arr_nms::storage Ret = Storage>
        tf_nms::enable_if_tt<Ret == par_arr_nms::storage::mem> print_data(const boost::filesystem::path&);
        
        static std::vector<std::pair<boost::filesystem::path, bool>> output_files(const boost::filesystem::path&, bool = false);
    };
    
    /* interp_grid class */
    template <interp_quant IQ, par_arr_nms::storage Storage, bool Tmp_Copy = true, bool Constructing = false>
    struct interp_grid :
    gqt_nms::grid_quantity_template<ig_in_params, cg_nms::cart_coord_grid, interp_data<IQ, Storage, Tmp_Copy>, interp_out_params, Constructing>
    {
        interp_grid() {}
        interp_grid(const boost::filesystem::path&, size_t = 1, size_t = 0);
        
        void fill_contents(const boost::filesystem::path&, size_t = 1, size_t = 0);
        static void print_out_mem(const boost::filesystem::path&);
    };
}

#include "Template_Impl/Interpolation_Grids.hpp"

#endif /* INTERPOLATION_GRIDS_H */
