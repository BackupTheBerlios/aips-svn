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
#ifndef CPARTSNAKESTART_H
#define CPARTSNAKESTART_H

// AIPS includes
#include <cfilter.h>
#include <cglobalprogress.h>
// lib includes
#include "libid.h"

using namespace aips;

/** Extracts local maxima from the given image */
class CPartSnakeStart : public CFilter
{
private:
  /// Standard constructor
  CPartSnakeStart();
  /// Copy constructor
  CPartSnakeStart( CPartSnakeStart& );
  /// Assignment operator
  CPartSnakeStart& operator=( CPartSnakeStart& );
public:
/* Structors */
  /// Constructor
  CPartSnakeStart( ulong ulID )
    throw();
  /// Destructor
  virtual ~CPartSnakeStart()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem  
  virtual void apply()
    throw();
private:		
	bool checkUpper( const TVector2D& p0, const TVector2D& p1, const TVector2D& s )
const throw();
};

#endif
