---
layout: post
title: "Rcpp and Expression Templates"
date: 2015-11-21 19:58:22 -0500
comments: true
categories: [Rcpp, C++, TMP, R] 
---
*Disclaimer: the topic of this post is briefly touched upon in the linked vignette; my aim here is merely to provide yet another
example use case.*

One of the more notable features of the [Rcpp package](https://cran.r-project.org/web/packages/Rcpp/vignettes/Rcpp-introduction.pdf) 
is the family of so called "sugar expressions", syntactic sugar for common R functions / expressions that can be called from 
within C++ code. More information in the [dedicated vignette](https://cran.r-project.org/web/packages/Rcpp/vignettes/Rcpp-sugar.pdf). 
If you've every wondered why these sugar expressions are so delightfully performant, the magic lies in the use of a neat C++ 
template metaprogramming (TMP) technique known as [Expression Templates](https://en.wikipedia.org/wiki/Expression_templates). If 
you are interested in learning more about expression templates, there is a fair bit of literature available online, but for now, 
let's jump right into an example. To keep things light, we will this technique to implement the R's `as.integer` 
function - for simplicity we only consider converting `numeric` vectors to `integer` vectors. 

___

First, a naïve C++ implementation:

{% include_code cpp/NaiveCast.cpp %}

And a quick sanity check:

{% include_code R/CheckNaiveCast.R %}

Alright, so far so good. Now the sugarized version, which is a bit more involved: 

{% include_code cpp/LazyCast.cpp %}

If you are initially put off by this, try to reserve judgement for the time being. Despite the somewhat complex appearance, many 
Rcpp sugar functions / expression template operations follow a very similar pattern, so if you are willing to put a 
little extra effort into familiarizing
yourself with the basic paradigm, you will have added a *very* powerful tool to your C++ / Rcpp arsenal. 

The above source file essentially consists of three parts: 

+ the implementation of the action in the form of a template class (`LazyCastImpl`) 

+ the templated function (`LazyCast`) which provides a callable interface 
to the implementation class 

+ the exported function (`lazy_cast`) which will be called from R 

This last part has nothing to do with expression templates; it's just an implementation detail 
specific to this example.

___

Anyways, let's start by focusing on the first part, as this is mainly where the magic happens. For the benefit of 
those who may not have spent countless hours reading through [Rcpp source code](https://github.com/RcppCore/Rcpp/tree/master/inst/include/Rcpp), 
I'll briefly touch on a few Rcpp-isms in the example above (not necessarily related to the topic at hand). 

+ Line 3, `template <int RTYPE, bool NA, typename VEC_T>`

You will see this for pretty much every `struct` / `class` deriving from `Rcpp::VectorBase` because (not surprisingly) 
that `int / bool / typename` combo corresponds to the template parameters of the 
[VectorBase class](https://github.com/RcppCore/Rcpp/blob/master/inst/include/Rcpp/vector/VectorBase.h#L28).
`RTYPE` is the integer code representing the `SEXPTYPE`, e.g. `REALSXP` (`numeric`) has an `RTYPE` of 14,
etc... A complete list can be found [here](http://www.biosino.org/R/R-doc/R-ints/SEXPTYPEs.html). The other 
non-type template parameter, `NA`, signifies whether or not the base class can contain missing (`NA`) values. 
Finally, the type parameter `VEC_T`, which is essential to the 
[CRTP](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Curiously_Recurring_Template_Pattern) 
idiom, represents the (non-base) vector type; a templated class itself. 

+ Line 12; `typedef typename Rcpp::traits::storage_type<INTSXP>::type result_type` 

Note that this is unnecessary in our example; I could have just as well written 
`typedef int result_type`. However, typically in such templates, `::storage_type< >::type` is used to get the equivalent C++ 
type of an (unknown) integer template parameter (e.g. `RTYPE`).  

___

Moving on, the first thing to notice is the inheritence pattern used - also a manifestation of CRTP - on lines 5 through 8: 

``` c++
// ... 
public Rcpp::VectorBase< INTSXP, true, LazyCastImpl<RTYPE, NA, VEC_T> >
```
Unlike the `typedef` on line 10 (`typedef Rcpp::VectorBase<RTYPE, NA, VEC_T> VEC_TYPE`), we inherit from a 
base vector with the following composition: 

1. `SEXPTYPE` is `INTSXP`: we already know that ultimately our result will be an integer vector.

2. the `NA` non-type parameter is set to `true`: we need to account for the possibility that our input may 
contain missing values, and if so, they should be preserved in the output vector.

3. The type parameter is the class itself - `LazyCastImpl<RTYPE, NA, VEC_T>` - since a key idea of CRTP is 
to provide the base class with access to / knowledge of whatever class is deriving from it. 

I'll summarize this with a general (but not definitive) rule-of-thumb regarding the expression template pattern in the 
context of Rcpp: *when inheriting from the `VectorBase` class, we set the non-type parameters based on 
the desired output object, and we set the type parameter to the derived class itself.*

___

Next, we consider the `typedef`s on lines 10 and 11 (line 12 was previously touched discussed): 

``` c++
typedef Rcpp::VectorBase<RTYPE, NA, VEC_T> VEC_TYPE;
typedef Rcpp::Vector<INTSXP> VECTOR;
``` 
Here, 

+ `VEC_TYPE` is a `typedef` for the `VectorBase` object that will be passed to our `LazyCastImpl` constructor, and for which 
we will also store a constant-reference-to-member to, e.g. `vec`. 

+ `VECTOR` is the output object of our expression (more on this to come). We use a `Rcpp::Vector<INTSXP>` and not 
a `VectorBase<...>` because the latter 
is just a base class and not really suitable for direct manipulation within code. 

___

Now we consider line 15, `const VEC_TYPE& vec`, in conjunction with our constructor on lines 18 and 19: 

``` c++
LazyCastImpl(const VEC_TYPE& vec_)
  : vec(vec_.get_ref()) {}

```

Since our member `vec` is a constant reference object, it **must be initialized in the constructor's member initializer list**. 
Attempting to initialize such a data member in the body of the constructor or at some later point (e.g. in a member function) 
will result in a compiler error. Also, note the manner in which we are initializing `vec`: `vec_.get_ref()`. Instead of calling 
the `VectorBase` copy constructor (which is not explicitly defined, but presumably the compiler has generated one to the effect 
of `VectorBase(const VectorBase& other)`), we initialize our member `vec` based on the `get_ref()` method - a method which we 
also implement in our own class. 

___


Finally, we examine lines 21 - 31, the methods of our `LazyCastImpl` class. First, the `size()` method on line 21: 

``` c++ 
inline R_xlen_t size() const {
  return vec.size();
}
```
Although we are not directly using this method to accomplish anything, in accordance with CRTP, we must implement it becuase the parent 
class defines its `size()` method as: 

``` c++ 
inline R_xlen_t size() const {
  return static_cast<const VECTOR*>(this)->size();
}
```





   
