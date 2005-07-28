/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 2004-08-05                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CMULTIPLY_H
#define CMULTIPLY_H

// AIPS includes
#include <cfilter.h>
#include <cglobalprogress.h>
// library includes
#include "libid.h"

using namespace aips;

/**
 * Adds or subtracts the intensities of the given input images.
 * Both images can be weighted before the operation.
 */
class CMultiply : public CFilter
{
private:
	/// Standard constructor
	CMultiply();
	/// Copy constructor
	CMultiply( CMultiply& );
	/// Assignment operator
	CMultiply& operator=( CMultiply& );
public: 
/* Structors */
	/// Constructor
  CMultiply( ulong ulID = 0 ) 
		throw();
	/// Destructor
  virtual ~CMultiply() 
		throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
};

#endif
