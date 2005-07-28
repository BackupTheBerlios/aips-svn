/***********************************************************************
 * File: ctukeyfilter.h                                                *
 * Project: AIPS - Diffusion filter plugin library                     *
 * Description: Smooting Filter based on the Tukey's Biweight function *
 *              with variable number of iterations, diffusion factor   *
 *              and Integration constant (lambda)                      *
 *                                                                     *
 * Author: Joaquin Castellanos                                         *                                                                     *
 * Version: 0.1                                                        *
 * Status : Beta                                                       *
 * Created: 2004-04-26                                                 *
 * Changed: 2004-06-16 Incorporated some corrections and optimizations *
 ***********************************************************************/

#ifndef CTUKEYFILTER_H
#define CTUKEYFILTER_H

// Standard includes
#include <algorithm>

// AIPS includes
#include <cfilter.h>
#include <aipsdatatraits.h>
#include <diffusiondefs.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// lib includes
#include "libid.h"

using namespace aips;

/** A Anisotropic Diffusion filter */
class CTukeyFilter : public CFilter
{
private:
  /// Standard constructor
  CTukeyFilter();
  /// Copy constructor
  CTukeyFilter( CTukeyFilter& );
  /// Assignment operator
  CTukeyFilter& operator=( CTukeyFilter& );
public:
/* Structors */
  /// Constructor
  CTukeyFilter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CTukeyFilter()
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
