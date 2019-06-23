#ifndef BASIC_ARITH_HPP
#define BASIC_ARITH_HPP


namespace bar_defs_nms
{
    /* Exception for functions in this header */
    class bar_exc {
    public:
        bar_exc(std::string& msg): err_message{msg} {}
        bar_exc(const char * msg): err_message{msg} {}
        std::string err_message;
        const char * give_err() {
            return err_message.c_str();
        }
    };
    
    struct arith_empty_base {};
    
    template <class T, class U, bool Broadcast, class V = arith_empty_base>
    struct unary_arith;
    
    template <class T, class U, class V>
    struct unary_arith<T, U, true, V> : V
    {
        T& operator+=(const U&);
        T& operator-=(const U&);
        T& operator*=(const U&);
        T& operator/=(const U&);
    };
    
    template <class T, class U, class V>
    struct unary_arith<T, U, false, V> : V
    {
        T& operator+=(const U&);
        T& operator-=(const U&);
        T& operator*=(const U&);
        T& operator/=(const U&);
    };
    
    
    template <class T, class U, class V = arith_empty_base>
    struct asym_ops : V
    {
        friend T operator-(const U& x, const T& y) {
            T ret {y};
            ret *= 0;
            ret += x;
            ret -= y;
            return ret;
        }
        friend T operator/(const U& x, const T& y) {
            T ret {y};
            ret *= 0;
            ret += x;
            ret /= y;
            return ret;
        }
    };
    
    template <class T, class U, bool Broadcast, class V = arith_empty_base>
    struct basic_arith_raw :
    unary_arith<T, U, Broadcast,
    asym_ops<T, U,
    boost::arithmetic2<T, U, V
    >>>
    {};
    
    template <class T, class V>
    struct basic_arith_raw<T, T, false, V> :
    unary_arith<T, T, false,
    boost::arithmetic1<T, V
    >>
    {};
}

/* Addition */
template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, true, V>::operator+=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] += x;}
    return derived;
}

template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, false, V>::operator+=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    if(derived.size() != x.size()) {throw bar_exc{"Length mismatch"};}
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] += x[idx];}
    return derived;
}


/* Subtraction */
template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, true, V>::operator-=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] -= x;}
    return derived;
}

template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, false, V>::operator-=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    if(derived.size() != x.size()) {throw bar_exc{"Length mismatch"};}
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] -= x[idx];}
    return derived;
}


/* Multiplication */
template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, true, V>::operator*=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] *= x;}
    return derived;
}

template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, false, V>::operator*=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    if(derived.size() != x.size()) {throw bar_exc{"Length mismatch"};}
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] *= x[idx];}
    return derived;
}


/* Division */
template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, true, V>::operator/=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] /= x;}
    return derived;
}

template <class T, class U, class V>
T& bar_defs_nms::unary_arith<T, U, false, V>::operator/=(const U& x)
{
    T& derived = static_cast<T&>(*this);
    if(derived.size() != x.size()) {throw bar_exc{"Length mismatch"};}
    for(size_t idx = 0; idx < derived.size(); ++idx) {derived[idx] /= x[idx];}
    return derived;
}


#endif /* BASIC_ARITH_HPP */
