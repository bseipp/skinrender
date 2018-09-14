/* Software for generating random spiculated and non-spiculated breast masses
 *
 * A re-implementation of the model developed by L. de Sisternes, et al
 * as described in the paper cited below.  This software was developed
 * independently and is not guaranteed to be identical to the original.
 *
 * de Sisternes, L., Brankov, J. G., Zysk, A. M., Schmidt, R. A., Nishikawa,
 * R. M. and Wernick, M. N. (2015), A computational model to generate simulated
 * three-dimensional breast masses. Med. Phys., 42: 1098–1118. doi:10.1118/1.4905232
 *
 * Software written by:
 * Christian G. Graff (christian.graff@fda.hhs.gov)
 * February 27, 2017
 *
 */

/*
 *  ***Disclaimer****
 *
 *  This software and documentation (the "Software") were developed at the
 *  Food and Drug Administration (FDA) by employees of the Federal Government in
 *  the course of their official duties. Pursuant to Title 17, Section 105 of
 *  the United States Code, this work is not subject to copyright protection
 *  and is in the public domain. Permission is hereby granted, free of charge,
 *  to any person obtaining a copy of the Software, to deal in the Software
 *  without restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, or sell copies of the software
 *  or derivatives, and to permit persons to whom the Software is furnished to do so.
 *  FDA assumes no responsibility whatsoever for use by other parties of the
 *  Software, its source code, documentation or compiled executables, and makes
 *  no guarantees, expressed or implied, about its quality, reliability, or any
 *  other characteristic. Further, use of this code in no way implies endorsement
 *  by the FDA or confers any advantage in regulatory decisions. Although this
 *  software can be redistributed and/or modified freely, we ask that any derivative
 *  works bear some notice that they are derived from it, and any modified versions
 *  bear some notice that they have been modified.
 *
 */

#ifndef BREASTMASS_HXX_
#define BREASTMASS_HXX_

// boost
#include <boost/math/special_functions/spherical_harmonic.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>

#include <omp.h>

// vtk
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkMath.h>
#include <vtkPNGWriter.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkBoxMuellerRandomSequence.h>
#include <vtkImageInterpolator.h>

// create spiculation segments
void createBranch(double, double, double, double, double, double, double, double, 
	double, vtkImageData*, vtkMinimalStandardRandomSequence*, vtkBoxMuellerRandomSequence*, boost::program_options::variables_map);


#endif /* BREASTMASS_HXX_ */
