/************************************************************************
 * File: csynergeticmodel.h                                             *
 * Project: AIPS basic plugins                                          *
 * Description: A region growing algorithm with a randomized component  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Created: 05.11.03                                                    *
 * Changed: 12.02.04 Module now produces correct output                 *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CSYNERGETICMODEL_H
#define CSYNERGETICMODEL_H

// Standard includes
#include <ctime>
#include <cstdlib>

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>
// lib includes
#include "libid.h"
#include "csynergeticmodeldialog.h"

// Qt includes
#include <qlabel.h>
#include <qimage.h>

using namespace aips;

/** A region growing algorithm with a randomized component */
class CSynergeticModel : public CFilter
{
private:
  /// Standard constructor
  CSynergeticModel();
  /// Copy constructor
  CSynergeticModel( CSynergeticModel& );
  /// Assignment operator
  CSynergeticModel& operator=( CSynergeticModel& );
public:
/* Structors */
  /// Constructor
  CSynergeticModel( ulong ulID )
    throw();
  /// Destructor
  virtual ~CSynergeticModel()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID_ ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
private:
	boost::shared_ptr<CSynergeticModelDialog> theDialog;
  ulong ulChangedCells;
  ulong ulChaoticCells;
	ulong ulRegionThreshold;	
	long lTTL;
  double dGrowChance;
	double dInitialGrowChance;
	double dReactivationStability;
  bool isInRange( short intensity, short target, short range ) const throw();
  bool staticGrowth( short src, short tgt, short &tgtnext, short &act ) throw();
  void chaoticGrowth( short source, short target, short &tgtnext, short amount ) throw();
  ushort validNeigh( short x, short y, short radius, TImagePtr tmp, ushort value );
  short min( short a, short b ) { if (a<b) return a; else return b; }
  short max( short a, short b ) { if (a>b) return a; else return b; }
  void addHomogenousNeighbors( ushort x, ushort y, TImagePtr theOutput,
    TImagePtr tmp, TImage* next );
  void addChaoticNeighbors( ushort x, ushort y, TImagePtr theOutput,
    TImagePtr tmp, TImage* next );
};

#endif
