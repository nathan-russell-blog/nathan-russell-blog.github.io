---
layout: post
title: "Code Markdown Test"
date: 2015-08-09 20:43:51 -0400
comments: true
categories: [test, code-sample] 
---

``` bash Bash Markdown 

#!/bin/bash

SOMEDIRECTORY=$(echo $PWD | sed -re 's/.+(include\/)(.+)/\1/')
echo $SOMEDIRECTORY

echo $(date +'%F %T')

```

``` c++ C++ Markdown

#include <tuple>
#include <type_traits>
#include <iostream>

template <typename... Ts>
struct whatever {
public:
  whatever(Ts... ts)
  : data(std::forward<Ts>(ts)...)
  {}

  static constexpr std::size_t arg_count = sizeof...(Ts);
  std::tuple<Ts...> data;

  template <typename... Args>
  inline void print() {
    print_data<Args...>(data);
    std::cout << std::endl;
  }

private:
  template <std::size_t I = 0, typename... Args>
  inline typename std::enable_if<I == arg_count, void>::type
  print_data(std::tuple<Args...>& tpl) {}

  template <std::size_t I = 0, typename... Args>
  inline typename std::enable_if<I < arg_count, void>::type
  print_data(std::tuple<Args...>& tpl) {
    typedef typename std::tuple_element<I, decltype(tpl)>::type type;
    std::cout <<
      std::get<I>(tpl) << "\t";
    print_data<I + 1, Args...>(tpl);
  }

};

```

``` r R Markdown

foo <- function(x, y) {
  x ** y
}

(data.table::data.table(
  x = rnorm(20),
  y = rpois(20, 3))[,
  z := foo(x, y)])

```

``` sql SQL Markdown

SELECT TOP 1 w.*
  ,x.[foo]
  ,x.[bar]
  ,ISNULL(x.[baz], 0.) AS [baz]
FROM [Whatever] w
  OUTER APPLY (
    SELECT TOP 1 [foo]
      ,[bar]
      ,[baz]
    FROM [Something Else]
    WHERE [Timestamp] BETWEEN GETDATE()-5 AND GETDATE()-1
    ORDER BY [Value Field] DESC
  ) x
ORDER BY [Something];

```

