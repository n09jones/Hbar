#include <cstdlib>
#include <mutex>
#include "Tmp_File_Manager.h"
#include "File_IO.h"

using boost::filesystem::path;
using boost::interprocess::file_lock;
using tmp_file_nms::tmp_file_exc;


namespace
{
    /* Mutex ensuring only one thread at a time manipulates temporary files */
    std::mutex flock_map_access {};
    /* Map maintaining file locks at a per-process level */
    std::map<path, std::unique_ptr<file_lock>> flock_map {};
    
    /* Enum describing whether the count file will be incremented, decremented, or reset */
    enum class ct_chng {incr, decr, reset};
    
    /*
     Inputs:
     - fn: Path of the source file
     - chng: Enum determining how the count file's value will change
     
     Function:
     - Change the value in the count file corresponding to fn in a direction informed by chng.
     
     Output:
     - New count file value
     */
    size_t update_count_file(const path& fn, ct_chng chng)
    {
        path count_fn {fio_nms::gen_tmp_count_fn(fn)};
        
        size_t access_num {0};
        if(is_regular_file(count_fn)) {
            if(fio_nms::data_len<size_t>(count_fn) != 1) {
                throw tmp_file_nms::tmp_file_exc{"Malformed count file"};
            }
            fio_nms::read_file(count_fn, &access_num, 1, 0);
        }
        remove(count_fn);
        
        switch (chng) {
            case ct_chng::incr :
                access_num++;
                break;
                
            case ct_chng::decr :
                if(!access_num) {throw tmp_file_nms::tmp_file_exc{"Can't decrement below zero"};}
                access_num--;
                break;
                
            case ct_chng::reset :
                access_num = 1;
                break;
        }
        
        fio_nms::print_to_file(&access_num, count_fn, 1);
        
        return access_num;
    }
}


/************************/
/*     CONSTRUCTORS     */
/************************/

/*
 Inputs:
 - src_fn_in: Path of the file whose temporary copy will be managed by the initialized object
 
 Function:
 - Make a temporary copy of src_fn_in if no up-to-date temporary exists.
 */
tmp_file_nms::tmp_file_mngr::tmp_file_mngr(const path& src_fn_in):
managing_file {true}, src_fn {src_fn_in}, tmp_fn {fio_nms::gen_tmp_fn(src_fn_in)}
{
    flock_map_access.lock();
    
    path lock_fn {fio_nms::gen_tmp_lock_fn(src_fn)};
    
    if(!flock_map.count(src_fn)) {
        if(!exists(lock_fn)) {std::ofstream ofs {lock_fn.string(), std::ios_base::app};}
        flock_map.insert(std::pair<path, std::unique_ptr<file_lock>> {src_fn, new file_lock {lock_fn.string().c_str()}});
    }
    
    flock_map.at(src_fn)->lock();
    
    update_count_file(src_fn, ct_chng::incr);
    if(!is_regular_file(tmp_fn) || (difftime(last_write_time(tmp_fn), last_write_time(src_fn)) < 0.0) || (file_size(tmp_fn) != file_size(src_fn))) {
        remove(tmp_fn);

/*
In the next two lines, I replace boost's copy_file by a POSIX system function. This is not portable and is
generally discouraged, but this workaround is necessary until the regal filesystem is fixed.
*/
/*        copy_file(src_fn, tmp_fn);*/
std::system(("cp " + src_fn.string() + " " + tmp_fn.string()).c_str());
/* End of workaround */


        update_count_file(src_fn, ct_chng::reset);
    }
    
    flock_map.at(src_fn)->unlock();
    flock_map_access.unlock();
}




/**********************/
/*     DESTRUCTOR     */
/**********************/

/*
 Function:
 - If this object is managing a temporary file, and if no other thread is using this file, delete the file.
 */
tmp_file_nms::tmp_file_mngr::~tmp_file_mngr()
{
    if(!managing()) {return;}
    
    flock_map_access.lock();
    flock_map.at(src_fn)->lock();
    if(!update_count_file(src_fn, ct_chng::decr)) {
        remove(fio_nms::gen_tmp_fn(src_fn));
        remove(fio_nms::gen_tmp_count_fn(src_fn));
    }
    flock_map.at(src_fn)->unlock();
    flock_map_access.unlock();
}

