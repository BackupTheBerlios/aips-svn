/************************************************************************
 * File: cgaussderivative.h                                             *
 * Project: AIPS - Edge detectors plugin library                        *
 * Description: Edge detector using the derivative of the gaussian      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status:  Beta                                                        *
 * Created: 2004-06-23                                                  *
 * Changed: 2004-07-02 Added 3D version of filter                       *
 *                     Updated documentation                            *
 *          2006-04-04 Added documentation. Overwork of source code.    *
 *                     Added type list scheme to handle all image data. *
 *          2006-05-24 Added output origin and spacing                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CGAUSSDERIVATIVE_H
#define CGAUSSDERIVATIVE_H
#define CGAUSSDERIVATIVE_VERSION "0.4"

// AIPS includes
#include <cfilter.h>


using namespace aips;
  
/** 
 * Gaussian derivative operator. Calculates the derivative of a Gaussian of a given scalar
 * input image.
 * Inputs: 
 * 1. A scalar image (2D or 3D)
 * 2. Optional: A unsigned 8 bit (TSmallImage) binary image defining a region of interest (2D or 3D)
 * Outputs: 
 * 1. A vector field (2D or 3D) normalized to a maximum vector length of 1.0
 * Parameters: 
 * 1. Sigma - Width of the Gaussian, defaults to 1.0
 */
class CGaussDerivative : public CFilter
{
private:
  /// Standard constructor
  CGaussDerivative();
  /// Copy constructor
  CGaussDerivative( CGaussDerivative& );
  /// Assignment operator
  CGaussDerivative& operator=( CGaussDerivative& );
public:
/** \name Structors */
	//{@
  /// Constructor
  CGaussDerivative( ulong ulID ) 
		throw();
  /// Destructor
  virtual ~CGaussDerivative() 
		throw();
	//}@
/** \name Other member functions */
	//{@
  /// Reimplemented from CPipelineItem  
  virtual void apply() 
		throw();
  NEW_INSTANCE( CGaussDerivative );
  //}@
private:
	DECLARE_CALL_MACRO( call2D );
	DECLARE_CALL_MACRO( call3D );
	TImagePtr roiSPtr; ///< Region of interest
	bool bRoiSelf;      ///< Is aoi generated by the filter?
	/// 2D Gaussian derivative operator
	template<typename ImageType>
	bool gauss2D() 
		throw();
	/// 3D Gaussian derivative operator
	template<typename ImageType>
	bool gauss3D()
		throw();
	/// Compute gaussian derivative mask
	std::vector<double> computeMask( int& iMaskSize )
		throw();
};

#endif
