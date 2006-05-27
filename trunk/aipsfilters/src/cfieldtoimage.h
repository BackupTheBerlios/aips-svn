/************************************************************************
 * File: cfieldtoimage.h                                                *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Converts a float image into an integer image            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status : Beta                                                        *
 * Created: 2005-01-13                                                  *
 * Changed: 2005-11-28 Added support for integer input. Added some      *
 *                     documentation.                                   *
 *          2006-04-04 Support for more data types. Added documentation.*
 *          2006-05-24 Added output origin and spacing                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CFIELDTOIMAGE_H
#define CFIELDTOIMAGE_H
#define CFIELDTOIMAGE_VERSION "0.4"

// AIPS includes
#include <cconverter.h>
#include <aipsnumeric.h>

// lib includes
#include "libid.h"

using namespace aips;
 
/**
 * Converts the scalar input field to a scalar integer field. Minimum and 
 * maximum values can be adjusted.
 * Inputs:
 *  1. A 2D or 3D scalar integer or floating point field (TField)
 * Outputs:
 *  1. Scalar integer point field (TImage) 
 *  2. Scalar small integer point field (TSmallImage) (not set if 
 *     minimum values are out of uint8 range)
 * Parameters:
 *  1. MinimumValue - Desired minimum value for output field (default 0)
 *  2. MaximumValue - Desired maximum value for output field (default 255)
 */
class CFieldToImage : public CConverter
{
private:
  /// Standard constructor
  CFieldToImage();
  /// Copy constructor
  CFieldToImage( CFieldToImage& );
  /// Assignment operator
  CFieldToImage& operator=( CFieldToImage& );
public:
/** \name Structors */
	//{@
  /// Constructor
  CFieldToImage( ulong ulID )
    throw();
  /// Destructor
  ~CFieldToImage()
    throw();
  //}@
/** \name Other member functions */
	//{@
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  NEW_INSTANCE( CFieldToImage );
  //}@
private:
	DECLARE_CALL_MACRO( call );
	template<typename ImageType> bool reduce() 
  	throw();
};

#endif
