#ifndef GRID_QUANTITY_TEMPLATE_H
#define GRID_QUANTITY_TEMPLATE_H

#include "ATRAP_base.h"

namespace gqt_nms
{
    /* Exception for functions in this header */
    class gqt_exc {
    public:
        gqt_exc(std::string& msg): err_message{msg} {}
        gqt_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    extern const std::function<bool()> fnc_true;
    extern const std::function<bool()> fnc_false;
    template <class U>
    void str_to_data(std::string, U*);
    
    /*
     The following members must be present for the classes used.
     
     In_Params/Coord_Grid/Out_Params:
     - header (std::string)
     - std::vector<param_info> get_param_info()
     - void post_read_ops(boost::program_options::variables_map)
     
     Out_Params:
     - opts_file_path (boost::filesystem::path)
     
     Data:
     - void import_data(Out_Params)
     - void print_data(Out_Params)
     */
    template <class In_Params, class Coord_Grid, class Data, class Out_Params, bool Constructing = false>
    class grid_quantity_template
    {
    private:
        std::vector<std::string> in_val_str;
        std::vector<std::string> cg_val_str;
        std::vector<std::string> out_val_str;
        
    public:
        In_Params in_params;
        Coord_Grid coord_grid;
        Data data;
        Out_Params out_params;
        
        grid_quantity_template() {}
        grid_quantity_template(const boost::filesystem::path&, size_t = 1, size_t = 0);
        
        void fill_in_params(const boost::filesystem::path&);
        void fill_coord_grid(const boost::filesystem::path&);
        void fill_out_params(const boost::filesystem::path&);
        void fill_contents(const boost::filesystem::path&, size_t = 1, size_t = 0);
        
        template <class U>
        void app_opt_file(const boost::filesystem::path&, U&, std::vector<std::string> = {});
        
        void write_opt_file(const boost::filesystem::path&);
        static void write_opt_file_template(const boost::filesystem::path&);
        
        template <bool Dummy = false, class = typename tf_nms::enable_if_tt<Dummy || Constructing>>
        void print_contents(bool = false);
        
        static void print_out_mem(const boost::filesystem::path&);
        static std::vector<std::pair<boost::filesystem::path, bool>> output_files(const boost::filesystem::path&);
    };
    
    template <class U>
    void fill_params(const boost::filesystem::path&, U&, std::vector<std::string>&);
    
    void print_memory(double);
}

#include "Template_Impl/Grid_Quantity_Template.hpp"

#endif /* GRID_QUANTITY_TEMPLATE_H */
