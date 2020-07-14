# rti1w
from [ray tracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)


### compile

the book uses some c++17 features. this command compiles and dumps stdout into an image

```
clang++ -std=c++17 -stdlib=libc++ main.cpp && ./a.out > image.ppm
```

