/***********************************************************************
 * File: cpmad1filter.h                                                *
 * Project: AIPS - Diffusion filter plugin library                     *
 * Description: Smooting Filter based on the 1st Perona-Malik function *
 *              with variable number of iterations, diffusion factor   *
 *              and Integration constant (lambda)                      *
 *                                                                     *
 * Author: Joaquin Castellanos                                         *                                                                     *
 * Version: 0.1                                                        *
 * Status : Beta                                                       *
 * Created: 2004-04-23                                                 *
 * Changed: 2004-06-16 Incorporated some corrections and optimizations *
 ***********************************************************************/

#ifndef CPMAD1FILTER_H
#define CPMAD1FILTER_H

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
class CPMAD1Filter : public CFilter
{
private:
  /// Standard constructor
  CPMAD1Filter();
  /// Copy constructor
  CPMAD1Filter( CPMAD1Filter& );
  /// Assignment operator
  CPMAD1Filter& operator=( CPMAD1Filter& );
public:
/* Structors */
  /// Constructor
  CPMAD1Filter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CPMAD1Filter()
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
