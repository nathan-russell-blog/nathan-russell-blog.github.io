#include <Rcpp.h>

template <int RTYPE, bool NA, typename VEC_T>
class LazyCastImpl
  : public Rcpp::sugar::Lazy<
      Rcpp::Vector<INTSXP>,
      LazyCastImpl<RTYPE, NA, VEC_T>
> {
public:
  typedef Rcpp::Vector<INTSXP> VECTOR;
  typedef Rcpp::VectorBase<RTYPE, NA, VEC_T> VEC_BASE;
  
private:
  const VEC_BASE& vec;
  
public:
  LazyCastImpl(const VEC_BASE& vec_)
    : vec(vec_.get_ref()) {}
  
  inline VECTOR get() const {
    R_xlen_t i = 0, n = vec.size();
    VECTOR result(n);
    
    for ( ; i < n; i++) {
      result[i] = static_cast<const int&>(vec[i]);
    }
    
    return result;
  }
};

template <int RTYPE, bool NA, typename VEC_T>
inline LazyCastImpl<RTYPE, NA, VEC_T>
LazyCast(const Rcpp::VectorBase<RTYPE, NA, VEC_T>& x) {
  return LazyCastImpl<RTYPE, NA, VEC_T>(x);
}

// [[Rcpp::export]]
Rcpp::IntegerVector lazy_cast2(Rcpp::NumericVector x) {
  return LazyCast(x);
}