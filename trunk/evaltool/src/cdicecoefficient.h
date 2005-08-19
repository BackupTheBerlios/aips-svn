/************************************************************************
 * File: cdicecoefficient.h                                             *
 * Project: AIPS                                                        *
 * Description: Dice coefficient of two images                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 2004-06-07                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CDICECOEFFICIENT_H
#define CDICECOEFFICIENT_H

// AIPS includes
#include <cfilter.h>

// library includes
#include "libid.h"

using namespace aips;

/**
 * Computes the dice coefficient of two images. The images must
 * be of the same size and should only have one channel
 */
class CDiceCoefficient : public CFilter
{
private:
	/// Standard constructor
	CDiceCoefficient();
	/// Copy constructor
	CDiceCoefficient( CDiceCoefficient& );
	/// Assignment operator
	CDiceCoefficient& operator=( CDiceCoefficient& );
public: 
/* Structors */
	/// Constructor
  CDiceCoefficient( ulong ulID = 0 ) 
		throw();
	/// Destructor
  virtual ~CDiceCoefficient() 
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
