/***********************************************************************
 * File: clowpassfilter.h                                              *
 * Project: AIPS neighbourhood filters plugin library                  *
 * Description: Uniform smoothing filter in image space                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.5                                                        *
 * Status : Beta                                                       *
 * Created: 2003-08-26                                                 *
 * Changed: 2003-09-09 Standardized variable names                     *
 *                     Exceptions added, attributes are now private    *
 *                     Constructor initializes constant parameters     *
 *          2003-09-23 Filter now works on "short" data                *
 *          2003-11-10 Filter now uses CKernelFilter and works also on *
 *                      volume data                                    *
 *          2004-01-29 Clarified and documented source code            *
 ***********************************************************************/

#ifndef CLOWPASSFILTER_H
#define CLOWPASSFILTER_H

// lib includes
#include <cglobalprogress.h>
#include "ckernelfilter.h"
#include "libid.h"

using namespace aips;

/** Uniform smoothing filter in image space */
class CLowPassFilter : public CKernelFilter
{
private:
  /// Standard constructor
  CLowPassFilter();
  /// Copy constructor
  CLowPassFilter( CLowPassFilter& );
  /// Assignment operator
  CLowPassFilter& operator=( CLowPassFilter& );
public:
/* Structors */
  /// Constructor
  CLowPassFilter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CLowPassFilter()
    throw();
  /// Reimplemented from CPipelineItem  
  virtual CPipelineItem* newInstance( ulong ulID ) const
    throw();
  /// Reimplemented from CPipelineItem  
  virtual void apply()
    throw();
};

#endif
