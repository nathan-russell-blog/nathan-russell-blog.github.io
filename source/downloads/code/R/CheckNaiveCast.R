x <- 1.5:10.5
R> class(x)
# [1] "numeric"

(y <- naive_cast(x))
# [1]  1  2  3  4  5  6  7  8  9 10
R> class(y)
# [1] "integer"