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
#ifndef CWAGENKNECHTHOMOGENITY_H
#define CWAGENKNECHTHOMOGENITY_H

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;

class CWagenknechtHomogenity : public CFilter
{
private:
  /// Standard constructor
  CWagenknechtHomogenity();
  /// Copy constructor
  CWagenknechtHomogenity( CWagenknechtHomogenity& );
  /// Assignment operator
  CWagenknechtHomogenity& operator=( CWagenknechtHomogenity& );
public:
/* Structors */
  /// Constructor
  CWagenknechtHomogenity( ulong ulID )
    throw();
  /// Destructor
  virtual ~CWagenknechtHomogenity()
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
