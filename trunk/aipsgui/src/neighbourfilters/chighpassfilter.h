/***********************************************************************
 * File: chighpassfilter.h                                             *
 * Project: AIPS                                                       *
 * Description: An unsharp mask high pass filter in image space.       *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.6                                                        *
 * Status : Beta                                                       *
 * Created: 26.08.03                                                   *
 * Changed: 09.09.03 Standardized variable names                       *
 *                   Exceptions added, attributes are now private      *
 *                   Constructor initializes constant parameters       *
 *          23.09.03 Filter now works on "short" data                  *
 *          29.09.03 Adapted filter for arbitrary intensity ranges     *
 *          12.11.03 Filter now inherits from CKernelFilter            *
 *          18.12.03 Class now holds Blitz and Standalone versions     *
 *          29.01.04 Clarified and documented source code              *
 ***********************************************************************/

#ifndef CHIGHPASSFILTER_H
#define CHIGHPASSFILTER_H

// lib includes
#include <cglobalprogress.h>
#include "ckernelfilter.h"
#include "libid.h"

using namespace aips;

/**
 * An unsharp mask high pass filter in image space.
 */
class CHighPassFilter : public CKernelFilter
{
private:
  /// Standard constructor
  CHighPassFilter();
  /// Copy constructor
  CHighPassFilter( CHighPassFilter& );
  /// Assignment operator
  CHighPassFilter& operator=( CHighPassFilter& );
public:
/* Structors */
  /// Constructor
  CHighPassFilter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CHighPassFilter()
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
