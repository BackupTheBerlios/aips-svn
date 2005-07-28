/************************************************************************
 * File: canalyzeheader.h                                               *
 * Project: AIPS                                                        *
 * Description: Header for the analyze file format                      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 2004-05-11                                                  *
 * Changed:                                                             *
 * TODO: Support SPM2 extented headers (header size != 348)             *
 *       Support irregular files (regular != r)                         *
 *       Support 4D files                                               *
 *       Support SPM files of nonstandard orientation                   *
 *       Support for setting vox_units, cal_units, pix_dim, vox_offset  *
 *        cal_max, cal_min, aux_file, orient                            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CANALYZEHEADER_H
#define CANALYZEHEADER_H

#include <cimageheader.h>
#include <aipsnumeric.h>

using namespace aips;

/** A header for the analyze file format, SPM99/SPM2 variations of the header are also supported */
class CAnalyzeHeader : public CImageHeader
{
private:
	/// Copy constructor
	CAnalyzeHeader( CAnalyzeHeader& );
	/// Assignment operator
	CAnalyzeHeader& operator=( CAnalyzeHeader& );
public:
/* Structors */
	/// Constructor
	CAnalyzeHeader() 
		throw();
	/// Destructor
  ~CAnalyzeHeader()
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
