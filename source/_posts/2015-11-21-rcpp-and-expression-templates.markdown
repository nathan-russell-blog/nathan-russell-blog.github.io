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

If you are initially put off by this, try to reserve judgement for a bit. Despite the somewhat complex appearance, many 
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





   
