#ifndef BASIC_VEC_HPP
#define BASIC_VEC_HPP

/**********************/
/*     CONVERSION     */
/**********************/

/*
 Inputs:
 - bv_in: basic_vec to convert
 
 Function:
 - Convert a basic_vec of type U to one of type T.
 
 Output:
 - Converted basic_vec
 */
template <class T, class U>
bvec_nms::basic_vec<T> bvec_nms::convert(const bvec_nms::basic_vec<U>& bv_in)
{
    return basic_vec<T>(bv_in[0], bv_in[1], bv_in[2]);
}



/***************************/
/*     SPECIAL VECTORS     */
/***************************/

/*
 Inputs:
 - comp: Component representing the unit vector's direction
 
 Function:
 - Construct a basic_vec with 1 at component comp and 0's elsewhere.
 
 Output:
 - Constructed basic_vec
 */
template <class T>
bvec_nms::basic_vec<T> bvec_nms::unit_vec(unsigned comp)
{
    basic_vec<T> bv {0, 0, 0};
    bv[comp] = 1;
    return bv;
}

#endif /* BASIC_VEC_HPP */
