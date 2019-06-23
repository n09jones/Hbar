/*
 */
#ifndef PARTITIONS_H
#define PARTITIONS_H

#include <string>
#include <boost/filesystem.hpp>

namespace prt_nms
{
    /* Exception for functions in this header */
    class prt_exc {
    public:
        prt_exc(std::string& msg): err_message{msg} {}
        prt_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    class partition {
    private:
        /* Default initialized? */
        bool def_init;
        
        /* Global partition info */
        size_t tot_elems;
        size_t tot_parts;
        size_t chunk_size;
        
        /* Subdivision info */
        size_t part_idx;
        size_t first_elem_idx;
        size_t elems_in_part;
        
    public:
        /* Constructors */
        partition() : def_init {true} {}
        explicit partition(size_t, size_t = 1, size_t = 0, size_t = 1);
        
        /* Change partition index */
        partition& set_part_idx(size_t);
        
        /* Access underlying data */
        bool is_def_init() const {return def_init;}
        
        size_t get_tot_elems() const {return tot_elems;}
        size_t get_tot_parts() const {return tot_parts;}
        size_t get_chunk_size() const {return chunk_size;}
        
        size_t get_part_idx() const {return part_idx;}
        size_t get_first_elem_idx() const {return first_elem_idx;}
        size_t get_elems_in_part() const {return elems_in_part;}
    };
}

#endif /* PARTITIONS_H */

