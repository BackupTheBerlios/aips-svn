/***********************************************************************
 * File: cpmad2filter.h                                                *
 * Project: AIPS - Diffusion filter plugin library                     *
 * Description: Smooting Filter based on the 2nd Perona-Malik function *
 *              with variable number of iterations, diffusion factor   *
 *              and Integration constant (lambda)                      *
 *                                                                     *
 * Author: Joaquin Castellanos                                         *                                                                     *
 * Version: 0.2                                                        *
 * Status : Beta                                                       *
 * Created: 2004-04-16                                                 *
 * Changed: 2004-04-22 New parameter definition to determine the number*
 *                      of neighbours to be considered in the operation*
 *          2004-06-16 Incorporated some corrections and optimizations *
 ***********************************************************************/

#ifndef CPMAD2FILTER_H
#define CPMAD2FILTER_H

// Standard includes
#include <algorithm>

// AIPS includes
#include <cfilter.h>
#include <aipsnumbertraits.h>
#include <diffusiondefs.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// lib includes
#include "libid.h"

using namespace aips;

/** A Anisotropic Diffusion filter */
class CPMAD2Filter : public CFilter
{
private:
  /// Standard constructor
  CPMAD2Filter();
  /// Copy constructor
  CPMAD2Filter( CPMAD2Filter& );
  /// Assignment operator
  CPMAD2Filter& operator=( CPMAD2Filter& );
public:
/* Structors */
  /// Constructor
  CPMAD2Filter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CPMAD2Filter()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem  
  virtual void apply()
    throw();
private:
	template<typename T> void filter() throw();	
};

#endif
