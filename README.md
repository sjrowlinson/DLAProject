# DLAProject
C++/CLI with C# project to create Diffusion Limited Aggregates (DLA) using fast C++ code with an interface managed by C# WPF.

Diffusion Limited Aggregrates are structures which are generated via random walking particles sticking to an initial seed building up a
system of sticky particles with interesting properties such as fractal behaviour. This project involves creating arbitrary DLA systems
on varying lattice types with different types of initial attractor seed using unmanaged (native) C++ behind the scenes wrapped with 
managed C++/CLI containers for use by C# code for producing a rich interface to interact with for creating aggregates.
