/************************************************************************
 * File: csimpledatheader.h                                             *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: A file header for the dat file format                   *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-04-19                                                  *
 * Changed: 2004-06-16 Updated documentation                            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CSIMPLEDATHEADER_H
#define CSIMPLEDATHEADER_H

#include <cimageheader.h>

using namespace aips;

/** A file header for the dat file format */
class CSimpleDatHeader : public CImageHeader
{
private:
	/// Copy constructor
	CSimpleDatHeader( CSimpleDatHeader& );
	/// Assignment operator
	CSimpleDatHeader& operator=( CSimpleDatHeader& );
public:
/* Structors */
	/// Constructor
  CSimpleDatHeader() 
		throw();
	/// Destructor
  ~CSimpleDatHeader() 
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
