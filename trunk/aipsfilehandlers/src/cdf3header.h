/************************************************************************
 * File: cdf3header.h                                                   *
 * Project: AIPS                                                        *
 * Description: Header for the POVRay volume format df3                 *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-06-03                                                  *
 * Changed: 2004-12-23 Updated and documented source code               *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CDF3HEADER_H
#define CDF3HEADER_H

#include <cimageheader.h>

using namespace aips;

/** A file header for the dat file format */
class CDF3Header : public CImageHeader
{
private:
	/// Copy constructor
	CDF3Header( CDF3Header& );
	/// Assignment operator
	CDF3Header& operator=( CDF3Header& );
public:
/* Structors */
	/// Constructor
  CDF3Header() 
		throw();
	/// Destructor
  ~CDF3Header() 
		throw();
/* Other methods */		
	/// Loads the header from an open stream
	virtual void loadHeader( std::istream& theFile ) 
		throw( FileException );
	/// Saves the header to an open stream
	virtual void saveHeader( std::ostream& theFile ) 
		throw( FileException );  		
};

#endif
