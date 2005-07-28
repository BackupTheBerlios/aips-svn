/************************************************************************
 * File: cfieldtoimage.h                                                *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Converts a float image into an integer image            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-13-01                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CFIELDTOIMAGE_H
#define CFIELDTOIMAGE_H

// AIPS includes
#include <cconverter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

/**
 * Reduces a double vector field to a short scalar field.
 */
using namespace aips;
 
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
/* Structors */
  /// Constructor
  CFieldToImage( ulong ulID )
    throw();
  /// Destructor
  ~CFieldToImage()
    throw();
/* Other member functions */
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
};

#endif
