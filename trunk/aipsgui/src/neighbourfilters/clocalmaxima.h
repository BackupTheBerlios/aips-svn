/************************************************************************
 * File: clocalmaxima.h                                                 *
 * Project: AIPS                                                        *
 * Description: Extracts local maxima from the given image              *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: 06-07-2004                                                  *
 * Changed: 09-07-2004 Added documentation                              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef CLOCALMAXIMA_H
#define CLOCALMAXIMA_H

// AIPS includes
#include <cfilter.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;

/** Extracts local maxima from the given image */
class CLocalMaxima : public CFilter
{
private:
  /// Standard constructor
  CLocalMaxima();
  /// Copy constructor
  CLocalMaxima( CLocalMaxima& );
  /// Assignment operator
  CLocalMaxima& operator=( CLocalMaxima& );
public:
/* Structors */
  /// Constructor
  CLocalMaxima( ulong ulID )
    throw();
  /// Destructor
  virtual ~CLocalMaxima()
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
