#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include <cfloat>
#include <fstream>
#include <iomanip>
#include "Global_Info.h"

#ifndef TXT_PREC
#define TXT_PREC LDBL_DIG
#endif


namespace fio_impl_nms
{
    /* Format check */
    bool is_bin_file(const boost::filesystem::path&);
    
    /* Print to file */
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    void print_to_file_bin(const T *, const boost::filesystem::path&, size_t);
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    void print_to_file_txt(const T *, const boost::filesystem::path&, size_t);
    
    /* Read file */
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    void read_binary_file(const boost::filesystem::path&, T *, size_t, size_t);
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    void read_text_file(const boost::filesystem::path&, T *, size_t, size_t);
}



/***********************/
/*     FILE LENGTH     */
/***********************/

/*
 Inputs:
 - fn: Path of the file to address
 
 Function:
 - Determine the number of numbers in fn.
 
 Output:
 - Number of numbers in fn
 */
template <class T, class>
size_t fio_nms::data_len(const boost::filesystem::path& fn)
{
    if(fio_impl_nms::is_bin_file(fn))
    {
        uintmax_t sz = file_size(fn);
        if((static_cast<uintmax_t>(-1) == sz) || (sz % sizeof(T))) {throw fio_nms::fio_exc{"Invalid file size"};}
        return static_cast<size_t>(sz/sizeof(T));
    }
    
    else
    {
        std::ifstream ifs {fn.string()};
        size_t elems {0};
        for(T dummy; (ifs >> dummy); ++elems);
        if(!(ifs.eof())) {throw fio_nms::fio_exc{"Read error"};}
        return elems;
    }
}




/*************************/
/*     PRINT TO FILE     */
/*************************/


/*
 Inputs:
 - ptr: Pointer to the data to print
 - fn: Path to the destination file
 - len: Number of numbers to print
 
 Function:
 - Create any missing parent directories and make sure fn doesn't already exist.
 - Write len numbers from the memory block starting at ptr to the file fn.
 
 Output:
 - None
 */
template <class T, class>
void fio_nms::print_to_file(const T * ptr, const boost::filesystem::path& fn, size_t len)
{
    if(fio_impl_nms::is_bin_file(fn)) {fio_impl_nms::print_to_file_bin(ptr, fn, len);}
    else {fio_impl_nms::print_to_file_txt(ptr, fn, len);}
}

template <class T, class>
void fio_impl_nms::print_to_file_bin(const T * ptr, const boost::filesystem::path& fn, size_t len)
{
    if(!is_bin_file(fn)) {throw fio_nms::fio_exc{"Wrong extension"};}
    if(exists(fn)) {throw fio_nms::fio_exc{"File already exists"};}
    create_directories(fn.parent_path());
    
    std::ofstream ofs {fn.string(), std::ios_base::binary};
    ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    ofs.write(reinterpret_cast<const char*>(ptr), static_cast<std::streamsize>(sizeof(T)*len));
}

template <class T, class>
void fio_impl_nms::print_to_file_txt(const T * ptr, const boost::filesystem::path& fn, size_t len)
{
    if(is_bin_file(fn)) {throw fio_nms::fio_exc{"Wrong extension"};}
    if(exists(fn)) {throw fio_nms::fio_exc{"File already exists"};}
    create_directories(fn.parent_path());
    
    std::ofstream ofs {fn.string()};
    ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    std::streamsize prec = (TXT_PREC);
    ofs << std::scientific << std::setprecision(prec);
    for(size_t idx = 0; idx < len; ++idx) {ofs << ptr[idx] << std::endl;}
}




/**************************/
/*     READ FROM FILE     */
/**************************/

/*
 Input:
 - fn: Path of the file to read
 - ptr: Pointer to the beginning of the memory block which will store the file's contents
 - len: Number of numbers to read
 - offset: Number of numbers at the beginning of the file to ignore
 
 Function:
 - Read len numbers from fn into the memory block starting at ptr, after ignoring the first offset numbers.
 
 Output:
 - ptr
 */
template <class T, class>
void fio_nms::read_file(const boost::filesystem::path& fn, T * ptr, size_t len, size_t offset)
{
    if(fio_impl_nms::is_bin_file(fn)) {fio_impl_nms::read_binary_file(fn, ptr, len, offset);}
    else {fio_impl_nms::read_text_file(fn, ptr, len, offset);}
}

template <class T, class>
void fio_impl_nms::read_binary_file(const boost::filesystem::path& fn, T * ptr, size_t len, size_t offset)
{
    if(!is_bin_file(fn)) {throw fio_nms::fio_exc{"Wrong extension"};}
    std::ifstream ifs {fn.string(), std::ios_base::binary};
    ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    
    ifs.seekg(static_cast<std::streamoff>(offset*sizeof(T)), ifs.beg);
    ifs.read(reinterpret_cast<char*>(ptr), static_cast<std::streamsize>(len*sizeof(T)));
}

template <class T, class>
void fio_impl_nms::read_text_file(const boost::filesystem::path& fn, T * ptr, size_t len, size_t offset)
{
    if(is_bin_file(fn)) {throw fio_nms::fio_exc{"Wrong extension"};}
    std::ifstream ifs {fn.string()};
    ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    
    T dummy;
    for(size_t ct = 0; ct < offset; ++ct) {ifs >> dummy;}
    for (size_t ct = 0; ct < len; ++ct) {ifs >> ptr[ct];}
}




/********************/
/*     MAP FILE     */
/********************/

/*
 Inputs:
 - fn: Path of the file to map
 - mapped_src: Reference to the mapped_file_source object that will hold the details of the mapping
 - len: Number of numbers of interest in the mapping
 - offset: Number of numbers at the beginning of the mapping to ignore
 
 Function:
 - Map len+offset numbers in fn, storing the details in mapped_src.
 
 Output:
 - Pointer to the first number of interest in the mapping
 */
template <class T, class>
const T * fio_nms::map_file(const boost::filesystem::path& fn, boost::iostreams::mapped_file_source& mapped_src, size_t len, size_t offset)
{
    if(!fio_impl_nms::is_bin_file(fn)) {throw fio_exc{"Wrong extension"};}
    mapped_src.open(fn.string(), (len+offset)*sizeof(T));
    return (reinterpret_cast<const T *>(mapped_src.data()) + offset);
}

#endif /* FILE_IO_HPP */
