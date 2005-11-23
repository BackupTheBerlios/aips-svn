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
#ifndef CPOINTLISTFROMMASK_H
#define CPOINTLISTFROMMASK_H

#include <vector>

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>

// lib includes
#include "libid.h"

using namespace aips;

class CPointListFromMask : public CFilter
{
private:
  /// Standard constructor
  CPointListFromMask();
  /// Copy constructor
  CPointListFromMask( CPointListFromMask& );
  /// Assignment operator
  CPointListFromMask& operator=( CPointListFromMask& );
public:
/* Structors */
  /// Constructor
  CPointListFromMask( ulong ulID )
    throw();
  /// Destructor
  virtual ~CPointListFromMask()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID_ ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
private:
	std::vector<TPoint2D> work;
};

#endif
