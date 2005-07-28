/************************************************************************
 * File: cfoerstner.h                                                   *
 * Project: AIPS - Edge operators library                               *
 * Description: The Foerstner-Corner-Detector                           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-05-10                                                  *
 * Changed: 2004-06-18 Added support for 3D fields                      *
 *          2004-07-02 Updated documentation                            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CFOERSTNER_H
#define CFOERSTNER_H

#include <cfilter.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif
#include "libid.h"

using namespace aips;

/** 
 * Computes the Foerstner corner measurement for 2D and 3D fields. 
 * 3D computation is done via the generalization of the operartor by Rohr
 */
class CFoerstner : public CFilter
{
private:
	/// Standard constructor
	CFoerstner();
	/// Copy constructor
	CFoerstner( const CFoerstner& );
	/// Assignment operator
	CFoerstner& operator= ( const CFoerstner& );
public:
/* Structors */
	/// Constructor
  CFoerstner( ulong ulID_ ) throw();
	/// Destructor
  ~CFoerstner() throw();
/* Other methods */
	/// Reimplemented from CFilter
	void apply() throw();
	/// Reimplemented from CFilter
	virtual CPipelineItem* newInstance( ulong ulID = 0 ) const throw();  
private:
	/// Apply filter to a 2D field
	void apply2D( TField2D* inputPtr ) throw();
	/// Apply filter to a 3D field
	void apply3D( TField3D* inputPtr ) throw();
};
	
#endif
