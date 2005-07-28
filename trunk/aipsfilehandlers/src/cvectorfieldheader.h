/************************************************************************
 * File: cvectorfieldheader                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz <hendrik@darkon.info>                                           *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Pre-Alpha                                                   *
 * Created: Do Jun 2 2005                                                      *
 * Based on:                                                            *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CVECTORFIELDHEADER_H
#define CVECTORFIELDHEADER_H

#include <cimageheader.h>

using namespace aips;

/** A file header for the dat file format */
class CVectorFieldHeader : public CImageHeader
{
private:
	/// Copy constructor
	CVectorFieldHeader( CVectorFieldHeader& );
	/// Assignment operator
	CVectorFieldHeader& operator=( CVectorFieldHeader& );
public:
/* Structors */
	/// Constructor
  CVectorFieldHeader()
		throw();
	/// Destructor
  ~CVectorFieldHeader()
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
