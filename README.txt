To compile the program, please see makefile.

To run cpu_curver, supply object file and number of iterations of curving:
./cpu_curver objects/iconorm.obj 2

To run shader_curver, supply object file and inner and outer levels of tessellation:
./shader_curver objects/iconorm.obj 4 4

When shader_curver is running, the following commands are available:
i, k	increase or decrease inner tessellation level
o, l 	increase or decrease outer tessellation level
r 		pause or resume rotation
t, g 	increase or decrease multiplicator of tangent
n 		randomize direction of first normal of object
wasd 	move camera

Written by Jakob Ambeck Vase and Jannek Egeskov Kristensen
2015-05-10