#include <Rcpp.h>

// [[Rcpp::export]]
Rcpp::IntegerVector naive_cast(Rcpp::NumericVector x) {
  R_xlen_t i = 0, n = x.size();
  Rcpp::IntegerVector result(n);
  
  for ( ; i < n; i++) {
    result[i] = static_cast<const int&>(x[i]);
  }
  
  return result;
}
