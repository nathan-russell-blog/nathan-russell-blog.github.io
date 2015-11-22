---
layout: post
title: "Rcpp and Expression Templates"
date: 2015-11-21 19:58:22 -0500
comments: true
categories: [Rcpp, C++, TMP] 
---
*Disclaimer: the topic of this post is briefly touched upon in the linked vignette; my aim here is merely to provide yet another
example use case.*

One of the more notable features of the [Rcpp package](https://cran.r-project.org/web/packages/Rcpp/vignettes/Rcpp-introduction.pdf) 
are the so called "sugar expressions", syntactic sugar for common R functions / expressions that can be called from 
within C++ code. More information in the [dedicated vignette](https://cran.r-project.org/web/packages/Rcpp/vignettes/Rcpp-sugar.pdf). 
If you've every wondered why these sugar expressions are so delightfully performant, the magic lies in the use of a neat C++ 
template metaprogramming (TMP) technique known as [Expression Templates](https://en.wikipedia.org/wiki/Expression_templates). If 
you are interested in learning more about expression templates, there is a fair bit of literature available online, but for now, 
let's jump right into an example. To keep things light, we will use expression templates to implement the R's `as.integer` 
function - for simplicity we only consider converting `numeric` vectors to `integer` vectors. 

First, a naïve C++ implementation:

{% include_code cpp/NaiveCast.cpp %}

And a quick sanity check:

{% include_code R/CheckNaiveCast.R %}

Alright, so far so good. Now the sugarized version, which is a bit more involved: 

{% include_code cpp/LazyCast.cpp %}

If you are initially put off by this, try to reserve judgement for the time being. Despite the somewhat complex appearance, many 
Rcpp sugar functions / expression template operations follow a very similar pattern, so if are willing to put a 
little extra effort into familiarizing
yourself will the basic paradigm, you will have added a *very* powerful tool to your C++ / Rcpp arsenal. 

The above source file essentially consists of three parts: 

+ the implementation of the action in the form of a template class (`LazyCastImpl`) 

+ the templated function (`LazyCast`) which provides a callable interface 
to the implementation class 

+ the exported function (`lazy_cast`) which will be called from R 

This last part has nothing to do with expression templates; it's just an implementation detail 
specific to this example.

Anyways, let's start by focusing on the first part, as this is mainly where the magic happens. For the benefit of 
those who may not have spent countless hours reading through [Rcpp source code](https://github.com/RcppCore/Rcpp/tree/master/inst/include/Rcpp), 
I'll briefly touch on a few Rcpp-isms in the above code (not necessarily related to the topic at hand). 

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

 



   
