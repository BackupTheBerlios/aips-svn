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
#ifndef CFILLCENTRALREGION_H
#define CFILLCENTRALREGION_H

#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

#include "libid.h"

using namespace aips;
 
/**
 * A class for simple morphological operations
 */
class CFillCentralRegion : public CFilter
{
private:
  /// Standard constructor
  CFillCentralRegion();
  /// Copy constructor
  CFillCentralRegion( CFillCentralRegion& );
  /// Assignment operator
  CFillCentralRegion& operator=( CFillCentralRegion& );
public:
  /// Constructor
  CFillCentralRegion( ulong ulID )
    throw();
  /// Destructor
  virtual ~CFillCentralRegion()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply ()
    throw();
};

#endif
