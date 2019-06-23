/*
 */
#ifndef MAPPED_ARR_H
#define MAPPED_ARR_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "Global_Info.h"
#include "Tmp_File_Manager.h"
#include "Partitions.h"
#include "Template_Impl/Template_Functions.hpp"
#include "Template_Impl/Math/Cmp_Ops.hpp"

namespace mapped_arr_nms
{
    /* Exception for functions in this header */
    class mapped_arr_exc {
    public:
        mapped_arr_exc(std::string& msg): err_message{msg} {}
        mapped_arr_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class T = global_nms::default_data_type, class U = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    class mapped_arr
    {
    private:
        /* Underlying data */
        const T * ptr;
        size_t len;
        
        std::unique_ptr<boost::iostreams::mapped_file_source> mapped_src_ptr;
        std::unique_ptr<tmp_file_nms::tmp_file_mngr> tmp_info_ptr;
        
    public:
        typedef T fund_type;
        typedef T value_type;
        typedef T cmp_type;
        
        /* Default constructor */
        mapped_arr() : ptr {nullptr}, len {0}, mapped_src_ptr {nullptr}, tmp_info_ptr {nullptr} {}
        
        /* Constructor for associating a file to a mapped_arr */
        mapped_arr(const boost::filesystem::path&, bool, prt_nms::partition = prt_nms::partition{});
        
        /* Iterator interface */
        typedef const T * const_iterator;
        const_iterator cbegin() const {return ptr;}
        const_iterator cend() const {return (ptr+len);}
        
        /* Access data */
        size_t size() const {return len;}
        const value_type* data() const {return ptr;}
        
        /* Subscripting */
        const T& operator[] (size_t idx) const {return ptr[idx];}
        
        /* Slicing (for compatibility) */
        const T& slice(size_t idx) const {return ptr[idx];}
    };
}

#include "Template_Impl/Arrays/Mapped_Arr.hpp"

#endif /* MAPPED_ARR_H */

