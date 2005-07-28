/************************************************************************
 * File: cforwarddifference                                             *
 * Project: AIPS                                                        *
 * Description: Central difference gradient operator                    *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 2004-05-10                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CCENTRALDIFFERENCE_H
#define CCENTRALDIFFERENCE_H

#include <cfilter.h>
#include "libid.h"
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

using namespace aips;
/**
@author Hendrik Belitz
*/
class CCentralDifference : public CFilter
{
public:
  CCentralDifference( ulong ulID_ ) throw();
  ~CCentralDifference()throw();
	void apply() throw();
	virtual CPipelineItem* newInstance( ulong ulID = 0 ) const throw();  
};

#endif
