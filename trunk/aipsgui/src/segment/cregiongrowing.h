/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Belitz                                  *
 *   h.belitz@fz-juelich.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 /* Build 2005-02-01 */
#ifndef CREGIONGROWING_H
#define CREGIONGROWING_H

#include <queue>

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>

// lib includes
#include "libid.h"

using namespace aips;

/** A region growing algorithm with a randomized component */
class CRegionGrowing : public CFilter
{
private:
  /// Standard constructor
  CRegionGrowing();
  /// Copy constructor
  CRegionGrowing( CRegionGrowing& );
  /// Assignment operator
  CRegionGrowing& operator=( CRegionGrowing& );
public:
/* Structors */
  /// Constructor
  CRegionGrowing( ulong ulID )
    throw();
  /// Destructor
  virtual ~CRegionGrowing()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID_ ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
private:
	ulong ulRegionThreshold;	
	std::queue<TPoint3D> work;
};

#endif
