#ifndef GRID_DATA_H
#define GRID_DATA_H

#include "ATRAP_base.h"

namespace gd_nms
{
    /* Exception for functions in this header */
    class gd_exc {
    public:
        gd_exc(std::string& msg): err_message{msg} {}
        gd_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    
    enum class quant {B, Bnorm, acc_1Sc, acc_1Sd, acc_2Sc, acc_2Sd};
    constexpr bool quant_is_scalar(quant quantity) {return (quant::Bnorm == quantity);}
    
    std::vector<std::string> base_strs(quant, bool);
    boost::filesystem::path base_str_to_fn(const boost::filesystem::path&, const std::string&);
    
    template <par_arr_nms::storage STORAGE, quant QUANT, class T = global_nms::default_data_type>
    using grid_quant = par_arr_nms::par_arr<STORAGE, (quant_is_scalar(QUANT) ? par_arr_nms::arr_type::scalar : par_arr_nms::arr_type::vector), T>;
    
    template <par_arr_nms::storage STORAGE, quant QUANT, class T = global_nms::default_data_type>
    class grid_data
    {
    private:
        std::map<std::string, grid_quant<STORAGE, QUANT, T>> data_map;
        
    public:
        typedef grid_quant<STORAGE, QUANT, T> stored_type;
        
        prt_nms::partition part;
        
        void import_data(const boost::filesystem::path&, bool, bool, prt_nms::partition = prt_nms::partition{});
        
        template <par_arr_nms::storage Ret = STORAGE>
        tf_nms::enable_if_tt<Ret == par_arr_nms::storage::mem> print_data(const boost::filesystem::path&);
        
        grid_quant<STORAGE, QUANT, T>& operator[](const std::string& str) {return data_map.at(str);}
        
        static std::vector<std::pair<boost::filesystem::path, bool>> output_files(const boost::filesystem::path&, bool);
    };
}

#include "Template_Impl/Grid_Data.hpp"

#endif /* GRID_DATA_H */


