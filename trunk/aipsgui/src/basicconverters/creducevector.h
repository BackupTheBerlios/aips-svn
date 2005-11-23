/************************************************************************
 * File: creducevector.h                                                *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Generates a scalar image from the magnitude of a        *
 *              vector field                                            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 02.12.03                                                    *
 * Changed: 2004-01-28 Clarified and documented source code             *
 *                     Apply now uses CTypedData::iterator              *
 *          2004-02-06 Now using ++it for the iterators. Standardized   *
 *                      constructor and method apply()                  *
 *          2004-02-26 Implemented 3D case. Added benchmarking and      *
 *                      global progress bar.                            *
 *          2004-06-16 Filter now also supports multichannel fields     *
 *                     Updated documentation and error output           *
 *          2005-12-01 Output changed from TImage to TField             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CREDUCEVECTOR_H
#define CREDUCEVECTOR_H

// AIPS includes
#include <cconverter.h>

// lib includes
#include "libid.h"

/**
 * Reduces a double vector field to a short scalar field.
 */
using namespace aips;
 
class CReduceVector : public CConverter
{
private:
  /// Standard constructor
  CReduceVector();
  /// Copy constructor
  CReduceVector( CReduceVector& );
  /// Assignment operator
  CReduceVector& operator=( CReduceVector& );
public:
/* Structors */
  /// Constructor
  CReduceVector( ulong ulID )
    throw();
  /// Destructor
  ~CReduceVector()
    throw();
/* Other member functions */
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
private:
	template<typename T> void reduce() throw();
};
#endif
