/************************************************************************
 * File: cedgethinner.h                                                 *
 * Project: AIPS                                                        *
 * Description: Thins out and homogenieses the given vector field       *
 *              to produce a better edge field                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 04-05-18                                                    *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CEDGETHINNER_H
#define CEDGETHINNER_H

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

class CEdgeThinner : public CFilter
{
private:
  /// Standard constructor
  CEdgeThinner();
  /// Copy constructor
  CEdgeThinner( CEdgeThinner& );
  /// Assignment operator
  CEdgeThinner& operator=( CEdgeThinner& );
public: 
/* Structors */
  /// Constructor
  CEdgeThinner( ulong ulID )
    throw();
  /// Destructor
  ~CEdgeThinner()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
private:
	/// Return the sector ( 0..7 ) of the given vector
	/// Each sector has an angle width of PI/4
	ushort sector( TVector2D aVector ) throw();
};

#endif
