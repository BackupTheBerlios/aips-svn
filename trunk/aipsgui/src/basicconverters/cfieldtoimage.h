/************************************************************************
 * File: cfieldtoimage.h                                                *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Converts a float image into an integer image            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-01-13                                                  *
 * Changed: 2005-11-28 Added support for integer input. Added some      *
 *                     documentation.                                   *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CFIELDTOIMAGE_H
#define CFIELDTOIMAGE_H
#define CFIELDTOIMAGE_VERSION "0.2"

// AIPS includes
#include <cconverter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;
 
/**
 * \brief Reduces a double vector field to a short scalar field.
 *
 * The only input is a scalar image. If this is of integer type, it
 * is send directly to the output after adjusting the minimum and maximum
 * values. Floating point images are converted to integer images using 
 * the parameters MinimumValue and MaximumValue.
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
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  //}@
};

#endif
