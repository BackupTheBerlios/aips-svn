/************************************************************************
 * File: cviewhistogram.h                                               *
 * Project: AIPS - Basic targets plugin library                         *
 * Description: A viewer for image histograms                           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 2003-12-19                                                  *
 * Changed: 2004-01-28 Moved inline members to cpp file                 *
 *          2004-01-29 Documented and clarified source code             *
 *          2004-02-06 Removed uneccassary constructor parameter sName  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CVIEWHISTOGRAM_H
#define CVIEWHISTOGRAM_H

// AIPS includes
#include <ctarget.h>
#include <chistogram.h>

#include <cviewhistogramdialog.h>
using namespace aips;

/**
 * A viewer for image histograms
 */ 
class CViewHistogram : public CTarget, private CHistogram
{
private:
  CViewHistogram();
  CViewHistogram( CViewHistogram& );
  CViewHistogram& operator=( CViewHistogram& );  
public:
/* Structors */
  /// Constructor
  CViewHistogram( ulong ulID )
    throw();
  /// Destructor
  virtual ~CViewHistogram()
    throw();
/* Other methods */    
  /// Reimplemented from CTarget
  virtual void apply() throw();
  /// Reimplemented from CTarget
  virtual const std::string dump() const
    throw();
  /// Reimplemented from CTarget
  CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
private:
	boost::shared_ptr<CViewHistogramDialog> theDialog;
};

#endif
