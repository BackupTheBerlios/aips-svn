/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: $DATE                                                       *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CSWAPAXES_H
#define CSWAPAXES_H

#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;

/**
@author Hendrik Belitz
*/
class CSwapAxes : public CFilter
{
private:
	/// Standard constructor 
	CSwapAxes();
	/// Copy constructor
	CSwapAxes( CSwapAxes& );
	/// Assignment operator
	CSwapAxes& operator= ( CSwapAxes& );
public:
/* Structors */
	/// Constructor
  CSwapAxes( ulong ulID )
    throw();
	/// Destructor
  virtual ~CSwapAxes()
    throw();
/* Other methods */	
	/// Reimplemented from CPipelineItem 
  virtual CPipelineItem* newInstance( ulong ulID ) const
    throw();    
	/// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
};

#endif
