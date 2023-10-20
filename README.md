# Ascotherian Spelaeology v3

![sample execution output](/examples/cave.gif)
*Several caves generated from the same initial parametrization.*



## Demonstration compilation

Requires the [cairo](https://www.cairographics.org/) graphics library for rendering tilemap images. 

Run `-I/opt/local/include/cairo -L/opt/local/lib -lcairo -lm -std=c11 cairoRenderer/cairoRenderWithID.c *.c` to compile the test driver found in `test.c`.
