/*
 */
#ifndef TMP_FILE_MANAGER_H
#define TMP_FILE_MANAGER_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

namespace tmp_file_nms
{
    /* Exception for functions in this header */
    class tmp_file_exc {
    public:
        tmp_file_exc(std::string& msg): err_message{msg} {}
        tmp_file_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    class tmp_file_mngr {
    private:
        /* Managed file info */
        bool managing_file;
        boost::filesystem::path src_fn;
        boost::filesystem::path tmp_fn;
        
    public:
        /* Constructors */
        tmp_file_mngr() : managing_file {false} {};
        explicit tmp_file_mngr(const boost::filesystem::path&);
        
        /* Copy operations: deleted */
        tmp_file_mngr(const tmp_file_mngr&) = delete;
        tmp_file_mngr& operator=(const tmp_file_mngr&) = delete;
        
        /* Move operations: deleted */
        tmp_file_mngr(tmp_file_mngr&&) = delete;
        tmp_file_mngr& operator=(tmp_file_mngr&&) = delete;
        
        /* Destructor */
        ~tmp_file_mngr();
        
        /* Access private elements */
        bool managing() const {return managing_file;}
        boost::filesystem::path get_src_fn() const {return src_fn;}
        boost::filesystem::path get_tmp_fn() const {return tmp_fn;}
    };
}

#endif /* TMP_FILE_MANAGER_H */

