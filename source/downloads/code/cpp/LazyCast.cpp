#include <Rcpp.h>

template <int RTYPE, bool NA, typename VEC_T>
class LazyCastImpl 
  : public Rcpp::VectorBase<
    INTSXP, true, 
    LazyCastImpl<RTYPE, NA, VEC_T> 
> {
public:
  typedef Rcpp::VectorBase<RTYPE, NA, VEC_T> VEC_TYPE;
  typedef Rcpp::Vector<INTSXP> VECTOR;
  typedef typename Rcpp::traits::storage_type<INTSXP>::type result_type;
  
private:
  const VEC_TYPE& vec;
  
public:
  LazyCastImpl(const VEC_TYPE& vec_) 
    : vec(vec_.get_ref()) {}
  
  inline R_xlen_t size() const {
    return vec.size();
  }
  
  inline result_type operator[](R_xlen_t i) const {
    return static_cast<const result_type&>(vec[i]);
  }
  
  inline const VECTOR& get_ref() const {
    return static_cast<const VECTOR&>(*this);
  }
};

template <int RTYPE, bool NA, typename VEC_T>
inline LazyCastImpl<RTYPE, NA, VEC_T>
LazyCast(const Rcpp::VectorBase<RTYPE, NA, VEC_T>& x) {
  return LazyCastImpl<RTYPE, NA, VEC_T>(x);
}

// [[Rcpp::export]]
Rcpp::IntegerVector lazy_cast(Rcpp::NumericVector x) {
  return LazyCast(x);
}