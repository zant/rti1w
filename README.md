# rti1w
work in progress :) implementation from "ray tracing in one weekend" book series (not at all in one weelend lol)


### compile

the book uses some c++17 features. this command compiles and dumps the generated stdout into an image callend `image.ppm`

```
clang++ -std=c++17 -stdlib=libc++ main.cpp && ./a.out > image.ppm
```

