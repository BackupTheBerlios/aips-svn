/************************************************************************
 * File: cdataheader.h                                                  *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: File header for the data file format                    *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-04-19                                                  *
 * Changed: 2004-06-16 Updated documentation                            *
 *          2004-11-13 Added new datatype framework                     *
 *          2004-12-23 Added exception handling                         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CDATAHEADER_H
#define CDATAHEADER_H

#include <cimageheader.h>

using namespace aips;

/** File header for the data file format */
class CDataHeader : public CImageHeader
{
private:
	/// Copy constructor
	CDataHeader( CDataHeader& );
	/// Assignment operator
	CDataHeader& operator=( CDataHeader& );
public:
/* Structors */
	/// Constructor
	CDataHeader() 
		throw();
	/// Destructor
  ~CDataHeader()
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
