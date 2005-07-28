/************************************************************************
 * File: cdatamirror.h                                                  *
 * Project: AIPS image filters plugin                                   *
 * Description: Mirrors the given dataset on the x,y,z axes             *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-05-11                                                  *
 * Changed: 2004-07-08 Minor documentation changes                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CDATAMIRROR_H
#define CDATAMIRROR_H

// AIPS includes
#include <cfilter.h>
#include <cglobalprogress.h>

// library includes
#include "libid.h"

using namespace aips;

/**
 * Mirrors a data set on any or all of the three axes
 */
class CDataMirror : public CFilter
{
private:
	/// Standard constructor
	CDataMirror();
	/// Copy constructor
	CDataMirror( CDataMirror& );
	/// Assignment operator
	CDataMirror& operator=( CDataMirror& );
public:
/* Structors */
	/// Constructor
  CDataMirror( ulong ulID = 0 ) 
		throw();
	/// Destructor
  virtual ~CDataMirror() 
		throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
private:
	/// Mirror an image
	void mirrorImage() 
		throw();
	/// Mirror a field of 2D vectors
	void mirrorField2D() 
		throw();
	/// Mirror a field of 3D vectors
	void mirrorField3D() 
		throw();
};

#endif
