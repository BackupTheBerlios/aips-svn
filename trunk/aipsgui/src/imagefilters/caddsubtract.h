/************************************************************************
 * File: caddsubtract.h                                                 *
 * Project: AIPS basic filters plugin                                   *
 * Description: Adds or subtracts the intensities of the given inputs   *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 2004-02-04                                                  *
 * Changed: 2004-02-27 Added benchmarking and global progress bar       *
 *                     Made a better internal implementation which      *
 *                     now also handles images of different sizes       *
 *                     and dimensions                                   *
 *          2004-07-08 Added a weighting parameter for the first image  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CADDSUBTRACT_H
#define CADDSUBTRACT_H

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
class CAddSubtract : public CFilter
{
private:
	/// Standard constructor
	CAddSubtract();
	/// Copy constructor
	CAddSubtract( CAddSubtract& );
	/// Assignment operator
	CAddSubtract& operator=( CAddSubtract& );
public: 
/* Structors */
	/// Constructor
  CAddSubtract( ulong ulID = 0 ) 
		throw();
	/// Destructor
  virtual ~CAddSubtract() 
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
