*Thank you to tilkinsc for writing this DDS loader: https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673#file-load_dds-c-L19

*And to stefan.gustavson@liu.se for his noise1234.c library

This uses the CUBES Engine along with Stefan Gustavson's noise1234.c library to generate some 3d perlin voxel terrain. There are some improvements to the CUBES engine in this that you might like, particularly that it actually runs. I also made a function that generates UI_Element structs from input strings, which can then be transformed and rendered however you want. UI_Elements are just text boxes, generated from strings using element_init(<args>).
