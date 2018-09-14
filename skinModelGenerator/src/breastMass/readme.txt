This software re-implements the three-dimensional breast mass model developed 
by L. de Sisternes, et al as described in the paper:

de Sisternes, L., Brankov, J. G., Zysk, A. M., Schmidt, R. A., Nishikawa, 
R. M. and Wernick, M. N. (2015), 
A computational model to generate simulated three-dimensional breast masses. 
Med. Phys., 42: 1098–1118. doi:10.1118/1.4905232

Software implementation written by:
Christian G. Graff (christian.graff@fda.hhs.gov)

See source code for software disclaimer

This is software written in C++ and tested on Linux systems

Required libraries: boost, blas, lapack, vtk

Compilation using cmake.  From a suitable build directory issue commands:
cmake [path to source code]
make

Editing of CMakeLists.txt may be necessary based on your operating system configuration

Usage:
breastMass -c [configuration file]

input:
a configuration file listing mass parameters.  For non-obvious paramter meanings see paper cited above.  All lengths given in millimetres.  See provided example configuration files

outputs:

mass_[random number seed]_[number of voxels per dimension].raw
a 3D raw voxelized cube of unsigned 8-bit integers, 0 = background, 1 = mass

mass_[random number seed].vti
a .vti file of the same, viewable in Paraview (www.paraview.org)
 
mass_[random number seed].cfg
a copy of the configuration file used

mass_[random number seed]_proj.png
a PNG image of a mass projection for visualization
