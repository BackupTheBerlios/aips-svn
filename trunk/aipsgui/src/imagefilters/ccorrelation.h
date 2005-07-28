/************************************************************************
 * File: ccorrelation.h                                                 *
 * Project: AIPS image filters plugin                                   *
 * Description: Computes the voxelwise correlation of two images        *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-05-07                                                  *
 * Changed: 2004-07-08 Added documentation                              *
 *                     Module now works on arbitrary images             *
 *                     Images do not need to have the same size         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CCORRELATION_H
#define CCORRELATION_H

// AIPS includes
#include <cfilter.h>
#include <cglobalprogress.h>

// library includes
#include "libid.h"

using namespace aips;

/**
 * Adds or subtracts the intensities of the given input images
 */
class CCorrelation : public CFilter
{
private:
	/// Standard constructor
	CCorrelation();
	/// Copy constructor
	CCorrelation( CCorrelation& );
	/// Assignment operator
	CCorrelation& operator=( CCorrelation& );
public: 
/* Structors */
	/// Constructor
  CCorrelation( ulong ulID = 0 ) 
		throw();
	/// Destructor
  virtual ~CCorrelation() 
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
