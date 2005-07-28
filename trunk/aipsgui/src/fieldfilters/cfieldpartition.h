/************************************************************************
 * File: cfieldpartition.h                                              *
 * Project: AIPS                                                        *
 * Description: Tries to determine a useful partition of a vector field *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 03.02.04                                                    *
 * Changed: 04.02.04 Corrected an error in method angle()               *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CFIELDPARTITION_H
#define CFIELDPARTITION_H

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// library includes
#include "libid.h"

using namespace aips;

/**
 * Tries to determine a useful partition of a given vector field
 */
class CFieldPartition : public CFilter
{
private:
  /// Standard constructor
  CFieldPartition();
  /// Copy constructor
  CFieldPartition( CFieldPartition& );
  /// Assignment operator
  CFieldPartition& operator=( CFieldPartition& );
public: 
/* Structors */
  /// Constructor
  CFieldPartition( ulong ulID )
    throw();
  /// Destructor
  ~CFieldPartition()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
private:
	void part3D();
  /// Determine the angle of a 2D vector
  double angle( TVector2D v )
    throw();
};

#endif
