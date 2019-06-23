/*
 This module provides a variant of std::unique_ptr which interface naturally with file IO and arithmetic/logical operators.
 */
#ifndef MEM_ARR_H
#define MEM_ARR_H

#include <string>
#include <boost/filesystem.hpp>
#include "Global_Info.h"
#include "File_IO.h"
#include "Partitions.h"
#include "Template_Impl/Template_Functions.hpp"
#include "Math/Basic_Arith.h"
#include "Template_Impl/Math/Cmp_Ops.hpp"

namespace mem_arr_nms
{
    /* Exception for functions in this header */
    class mem_arr_exc {
    public:
        mem_arr_exc(const std::string& msg): err_message{msg} {}
        mem_arr_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    template <class T = global_nms::default_data_type, class U = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    class mem_arr:
    public bar_nms::basic_arith<mem_arr<T>, mem_arr<T>, false,
    bar_nms::basic_arith<mem_arr<T>, T, true
    >>
    {
    private:
        /* Underlying data */
        std::unique_ptr<T []> ptr;
        size_t len;
        
    public:
        typedef T fund_type;
        typedef T value_type;
        typedef T cmp_type;
        
        /* Default constructor */
        mem_arr() : ptr {nullptr}, len {0} {}
        
        /* Constructors for allocating memory for a mem_arr */
        explicit mem_arr(size_t);
        mem_arr(size_t, T);
        
        /* Constructor for associating a file to a mem_arr */
        mem_arr(const boost::filesystem::path&, prt_nms::partition = prt_nms::partition{});
        
        /* Copy operations */
        mem_arr(const mem_arr<T>&);
        mem_arr<T>& operator=(const mem_arr<T>&);
        
        /* Move operations: default */
        mem_arr(mem_arr<T>&&);
        mem_arr<T>& operator=(mem_arr<T>&&);
        
        /* Destructor */
        ~mem_arr() {}
        
        /* Iterator interface */
        typedef T * iterator;
        iterator begin() {return ptr.get();}
        iterator end() {return (ptr.get()+len);}
        
        typedef const T * const_iterator;
        const_iterator begin() const {return ptr.get();}
        const_iterator end() const {return (ptr.get()+len);}
        const_iterator cbegin() const {return ptr.get();}
        const_iterator cend() const {return (ptr.get()+len);}
        
        /* Access data */
        size_t size() const {return len;}
        value_type* data() {return ptr.get();}
        const value_type* data() const {return ptr.get();}
        
        /* Subscripting */
        T& operator[] (size_t idx) {return ptr[idx];}
        const T& operator[] (size_t idx) const {return ptr[idx];}
        
        /* Slicing (for compatibility) */
        T& slice(size_t idx) {return ptr[idx];}
        const T& slice(size_t idx) const {return ptr[idx];}
    };
    
    /* Conversion */
    template <class T, class U>
    mem_arr<T> convert(const mem_arr<U>&);
    
    /* Square root */
    template <class T>
    mem_arr<T> sqrt(mem_arr<T>);
    
    /* Print to File */
    template <class T>
    void print_to_file(const mem_arr<T>&, const boost::filesystem::path&);
    
    /* Comparision */
    template <class T> bool differ_less_than(const mem_arr<T>&, const mem_arr<T>&, T);
}

#include "Template_Impl/Arrays/Mem_Arr.hpp"

#endif /* MEM_ARR_H */

