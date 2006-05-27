/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: $DATE                                                       *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CHYSTERESIS_H
#define CHYSTERESIS_H
#define CHYSTERESIS_VERSION "0.2"

// AIPS includes
#include <aipsnumbertraits.h>
#include <cfilter.h>
// library includes
#include "libid.h"

using namespace aips;
  
/** 
 * Hysteris thresholding on a given 2D or 3D scalar edge image.
 * Inputs:
 * 1. Scalar edge image
 * Outputs:
 * 1. Threholded edge image (Of same type as input)
 * Parameters:
 * 1. Low - Low threshold
 * 2. High - High threshold
 */
class CHysteresis : public CFilter
{
private:
  /// Standard constructor
  CHysteresis();
  /// Copy constructor
  CHysteresis( CHysteresis& );
  /// Assignment operator
  CHysteresis& operator=( CHysteresis& );
public:
/** \name Structors */
	//@{
  /// Constructor
  CHysteresis( ulong ulID ) 
		throw();
  /// Destructor
  virtual ~CHysteresis() 
		throw();
	//@}
/** \name Other methods */
	//@{
  /// Reimplemented from CPipelineItem  
  virtual void apply() 
		throw();
	NEW_INSTANCE( CHysteresis );
	//@}
private:
	DECLARE_CALL_MACRO( call );
	template<typename ImageType> bool hysteresis() 
  	throw();
};

#endif
