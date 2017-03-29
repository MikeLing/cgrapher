# cgrapher
a clang plug for generater a call graph
## useage
Try
> clang++-3.9 -fplugin=./cgrapher.so test.cpp

And the result will be:

```
 --- Call graph Dump --- 
  Function: < root > calls: D Y X C B S P O N M G A main 
  Function: main calls: A 
  Function: A calls: B G 
  Function: G calls: M 
  Function: M calls: N 
  Function: N calls: O 
  Function: O calls: P 
  Function: P calls: S 
  Function: S calls: D 
  Function: B calls: C 
  Function: C calls: D X 
  Function: X calls: Y 
  Function: Y calls: D 
  Function: D calls: 
```
