#ifndef MEM_ARR_HPP
#define MEM_ARR_HPP

/************************/
/*     CONSTRUCTORS     */
/************************/

/*
 Inputs:
 - len_in: Memory block length (in numbers) to be allocated
 ( - init_val: Value with which to fill the allocated memory block )
 
 Function:
 - Set len to len_in.
 - Allocate a memory block which can hold len_in numbers.
 ( - Initialize every number in the block to init_val. )
 */
template <class T, class U>
mem_arr_nms::mem_arr<T, U>::mem_arr(size_t len_in) :
ptr{new T[len_in]}, len {len_in}
{
    if(!len) {throw mem_arr_exc{"Number of elements must be positive"};}
}

template <class T, class U>
mem_arr_nms::mem_arr<T, U>::mem_arr(size_t len_in, T init_val) :
mem_arr {len_in}
{
    std::fill(begin(), end(), init_val);
}


/*
 Inputs:
 - fn: Path of the file to be read
 - part: Partition corresponding to the part of fn that should be read
 
 Function:
 - Read the selected subset of fn.
 */
template <class T, class U>
mem_arr_nms::mem_arr<T, U>::mem_arr(const boost::filesystem::path& fn, prt_nms::partition part)
{
    size_t full_len = fio_nms::data_len<T>(fn);
    if(part.is_def_init()) {part = prt_nms::partition{full_len};}
    
    size_t offset = part.get_first_elem_idx();
    if(full_len != part.get_tot_elems()) {throw mem_arr_exc{"Wrong number of elements in file"};}
    
    *this = std::move(mem_arr{part.get_elems_in_part()});
    fio_nms::read_file(fn, ptr.get(), len, offset);
}


/***************************/
/*     COPY OPERATIONS     */
/***************************/

/*
 Inputs:
 - marr: mem_arr object to copy
 
 Function:
 - Allocate memory to hold the contents of marr, and copy the contents of marr into the newly allocated memory.
 
 Output:
 ( - Reference to *this )
 */
template <class T, class U>
mem_arr_nms::mem_arr<T, U>::mem_arr(const mem_arr<T>& marr) :
mem_arr {marr.size()}
{
    std::copy(marr.cbegin(), marr.cend(), begin());
}

template <class T, class U>
mem_arr_nms::mem_arr<T>& mem_arr_nms::mem_arr<T, U>::operator=(const mem_arr<T>& marr)
{
    *this = std::move(mem_arr<T>{marr});
    return *this;
}


/***************************/
/*     MOVE OPERATIONS     */
/***************************/

/*
 Inputs:
 - marr: mem_arr object to copy
 
 Function:
 - Allocate memory to hold the contents of marr, and copy the contents of marr into the newly allocated memory.
 
 Output:
 ( - Reference to *this )
 */
template <class T, class U>
mem_arr_nms::mem_arr<T, U>::mem_arr(mem_arr<T>&& marr) : ptr {std::move(marr.ptr)}, len {marr.len} {}

template <class T, class U>
mem_arr_nms::mem_arr<T>& mem_arr_nms::mem_arr<T, U>::operator=(mem_arr<T>&& marr)
{
    ptr = std::move(marr.ptr);
    len = std::move(marr.len);
    return *this;
}



/**********************/
/*     CONVERSION     */
/**********************/

/*
 Inputs:
 - marr_in: mem_arr to convert
 
 Function:
 - Convert a mem_arr of type U to one of type T.
 
 Output:
 - Converted mem_arr
 */
template <class T, class U>
mem_arr_nms::mem_arr<T> mem_arr_nms::convert(const mem_arr_nms::mem_arr<U>& marr_in)
{
    mem_arr<T> marr_out {marr_in.size()};
    std::copy(marr_in.cbegin(), marr_in.cend(), marr_out.begin());
    return marr_out;
}



/***********************/
/*     SQUARE ROOT     */
/***********************/

/*
 Inputs:
 - marr: mem_arr of interest
 
 Function:
 - Find the component-wise square root of marr.
 
 Output:
 - mem_arr holding the component-wise square root of marr
 */
template <class T>
mem_arr_nms::mem_arr<T> mem_arr_nms::sqrt(mem_arr<T> marr)
{
    mem_arr<T> sqrt_arr {std::move(marr)};
    std::transform(sqrt_arr.begin(), sqrt_arr.end(), sqrt_arr.begin(), [](T x){return std::sqrt(x);});
    return sqrt_arr;
}


/*************************/
/*     PRINT TO FILE     */
/*************************/

/*
 Inputs:
 - marr: mem_arr to write
 - fn: Path to the destination file
 
 Function:
 - Write the contents of marr to fn.
 
 Output:
 - None
 */
template <class T>
void mem_arr_nms::print_to_file(const mem_arr<T>& marr, const boost::filesystem::path& fn)
{
    fio_nms::print_to_file(marr.data(), fn, marr.size());
}



/**********************/
/*     COMPARISON     */
/**********************/

/*
 Inputs:
 - marr_1: mem_arr to compare
 - marr_2: mem_arr to compare
 - diff_lim: Maximum difference allowed
 
 Function:
 - Determine whether marr_1 and marr_2 differ by more than diff_lim at any index.
 
 Output:
 - true: |marr_1[idx] - marr_2[idx]| < diff_lim for all valid idx
 - false: otherwise
 */
template <class T>
bool mem_arr_nms::differ_less_than(const mem_arr<T>& marr_1, const mem_arr<T>& marr_2, T diff_lim)
{
    if(marr_1.size() != marr_2.size()) {throw mem_arr_exc{std::string{"Different lengths "} + std::to_string(marr_1.size()) + std::string{" "} + std::to_string(marr_2.size())};}
    return std::equal(marr_1.cbegin(), marr_1.cend(), marr_2.cbegin(), [&diff_lim](T x, T y){return (fabs(x-y) < diff_lim);});
}

#endif /* MEM_ARR_HPP */
