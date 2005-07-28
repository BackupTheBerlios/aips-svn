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
#ifndef CGENERATESTARTCONTOUR_H
#define CGENERATESTARTCONTOUR_H

// AIPS includes
#include <cfilter.h>

// lib includes
#include "libid.h"

using namespace aips;

/** Extracts local maxima from the given image */
class CGenerateStartContour : public CFilter
{
private:
  /// Standard constructor
  CGenerateStartContour();
  /// Copy constructor
  CGenerateStartContour( CGenerateStartContour& );
  /// Assignment operator
  CGenerateStartContour& operator=( CGenerateStartContour& );
public:
/* Structors */
  /// Constructor
  CGenerateStartContour( ulong ulID )
    throw();
  /// Destructor
  virtual ~CGenerateStartContour()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem  
  virtual void apply()
    throw();
private:
	bool isInNeighbourhood( TImagePtr imagePtr, ushort usX, ushort usY, ushort usIntensity );
	ushort countNeighbours( TImagePtr imagePtr, ushort usX, ushort usY, 
		ushort usRadius, ushort usIntensity );
};

#endif
