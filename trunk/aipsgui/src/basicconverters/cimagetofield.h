/************************************************************************
 * File: cimagetofield.h                                                *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Converts an integer image into a float image            *
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
#ifndef CIMAGETOFIELD_H
#define CIMAGETOFIELD_H

// AIPS includes
#include <cconverter.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

/**
 * Reduces a double vector field to a short scalar field.
 */
using namespace aips;
 
class CImageToField : public CConverter
{
private:
  /// Standard constructor
  CImageToField();
  /// Copy constructor
  CImageToField( CImageToField& );
  /// Assignment operator
  CImageToField& operator=( CImageToField& );
public:
/* Structors */
  /// Constructor
  CImageToField( ulong ulID )
    throw();
  /// Destructor
  ~CImageToField()
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
