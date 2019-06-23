#include "Partitions.h"


/************************/
/*     CONSTRUCTORS     */
/************************/

/*
 Inputs:
 - tot_elems_in: Number of computational elements in the partition
 - tot_parts_in: Number of subdivisions in the partition
 - part_idx_in: Index of the subdivision of interest
 
 Function:
 - Initialize the partition object to describe tot_elems_in elements divided into tot_parts_in subsets. The subdivision of interest is part_idx_in.
 - Check that the inputs make sense.
 */
prt_nms::partition::partition(size_t tot_elems_in, size_t tot_parts_in, size_t part_idx_in, size_t chunk_size_in):
def_init {false}, tot_elems {tot_elems_in}, tot_parts {tot_parts_in}, chunk_size {chunk_size_in}
{
    if((tot_elems < tot_parts*chunk_size) || !tot_parts || !chunk_size || (tot_elems % chunk_size)) {
        throw prt_exc{"Invalid parameters"};
    }
    
    set_part_idx(part_idx_in);
}




/**********************************/
/*     CHANGE PARTITION INDEX     */
/**********************************/

/*
 Inputs:
 - part_idx_in: New partition index
 
 Function:
 - Make sure part_idx_in makes sense.
 - Change part_idx along with dependent data members.
 
 Output:
 - Reference to the modified partition
 */
prt_nms::partition& prt_nms::partition::set_part_idx(size_t part_idx_in)
{
    part_idx = part_idx_in;
    if(part_idx >= tot_parts) {throw prt_exc{"Partition index out-of-range"};}
    
    size_t tot_chunks = tot_elems/chunk_size;
    
    size_t base_chunks_per_part = tot_chunks/tot_parts;
    size_t rem_chunks = tot_chunks - (base_chunks_per_part*tot_parts);
    
    first_elem_idx = ((part_idx*base_chunks_per_part) + ((part_idx < rem_chunks) ? part_idx : rem_chunks))*chunk_size;
    elems_in_part = (base_chunks_per_part + ((part_idx < rem_chunks) ? 1 : 0))*chunk_size;
    
    return *this;
}

