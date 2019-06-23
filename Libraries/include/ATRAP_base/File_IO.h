#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "Partitions.h"
#include "Template_Impl/Template_Functions.hpp"

namespace fio_nms
{
    /* Exception for functions in this header */
    class fio_exc {
    public:
        fio_exc(std::string& msg): err_message{msg} {}
        fio_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    /* File length */
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    size_t data_len(const boost::filesystem::path&);
    
    /* Print to file */
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    void print_to_file(const T *, const boost::filesystem::path&, size_t);
    
    /* Read file */
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    void read_file(const boost::filesystem::path&, T *, size_t, size_t);
    
    /* Map file */
    template <class T, class = tf_nms::enable_if_tt<tf_nms::is_basic<T>()>>
    const T * map_file(const boost::filesystem::path&, boost::iostreams::mapped_file_source&, size_t, size_t);
    
    /* Derived file names */
    std::pair<boost::filesystem::path, bool> part_file_name(std::pair<boost::filesystem::path, bool>, const prt_nms::partition&);
    std::pair<boost::filesystem::path, bool> part_file_name(std::pair<boost::filesystem::path, bool>, size_t, size_t);
    std::vector<std::pair<boost::filesystem::path, bool>> part_file_name(const std::vector<std::pair<boost::filesystem::path, bool>>&, const prt_nms::partition&);
    std::vector<std::pair<boost::filesystem::path, bool>> part_file_name(const std::vector<std::pair<boost::filesystem::path, bool>>&, size_t, size_t);
    boost::filesystem::path comp_file_name(const boost::filesystem::path&, int);
    boost::filesystem::path vec_directory_name(const boost::filesystem::path&);
    std::vector<boost::filesystem::path> expand_fns(const std::vector<std::pair<boost::filesystem::path, bool>>&);
    
    /* Partition file operations */
    bool clear_incomplete_file_set(const std::vector<std::pair<boost::filesystem::path, bool>>&);
    std::list<size_t> missing_part_files(const std::vector<std::pair<boost::filesystem::path, bool>>&, size_t);
    void cat_part_files(const std::vector<std::pair<boost::filesystem::path, bool>>&, size_t);
    
    /* Generate file names */
    boost::filesystem::path gen_new_idxed_fn(const boost::filesystem::path&);
    boost::filesystem::path gen_tmp_fn(boost::filesystem::path);
    boost::filesystem::path gen_tmp_count_fn(const boost::filesystem::path&);
    boost::filesystem::path gen_tmp_lock_fn(const boost::filesystem::path&);
}

#include "Template_Impl/File_IO.hpp"

#endif /* FILE_IO_H */
