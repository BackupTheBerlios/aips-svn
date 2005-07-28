/***********************************************************************
 * File: cmedianfilter.h                                               * 
 * Project: AIPS neighbourhood filters plugin library                  * 
 * Description: Median filter with variable window size                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.4                                                        *
 * Status : Beta                                                       *
 * Created: 2003-08-26                                                 *
 * Changed: 2003-09-02 Corrected allocation error                      *
 *          2003-09-09 Standardized variable names                     *
 *                     Exceptions added, attributes are now private    *
 *                     Constructor initializes constant parameters     *
 *          2003-09-23 Filter now works on "short" data                *
 *          2004-01-16 Filter now works on multichannel data           *
 *          2004-01-29 Clarified and documented source code            *
 *                     Changed internal implementation of median       *
 *                      pivoting                                       *
 *          2004-02-03 Removed a bug on 3D images                      *
 *          2004-05-06 Added progress bar and event handler            *
 ***********************************************************************/

#ifndef CMEDIANFILTER_H
#define CMEDIANFILTER_H

// Standard includes
#include <algorithm>

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;

/** A median filter */
class CMedianFilter : public CFilter
{
private:
  /// Standard constructor
  CMedianFilter();
  /// Copy constructor
  CMedianFilter( CMedianFilter& );
  /// Assignment operator
  CMedianFilter& operator=( CMedianFilter& );
public:
/* Structors */
  /// Constructor
  CMedianFilter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CMedianFilter()
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
