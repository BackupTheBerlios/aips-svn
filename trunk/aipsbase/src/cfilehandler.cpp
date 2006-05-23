/************************************************************************
 * File: cfilehandler.cpp                                               *
 * Project: AIPS                                                        *
 * Description: A file handler base class                               *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 24.10.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cfilehandler.h"

using namespace std;
using namespace aips;

/************* 
 * Structors *
 *************/
 
/**
 * Standard constructor
 */
CFileHandler::CFileHandler( const std::string &sClassName_, const std::string &sClassVersion_,
    const std::string &sDerivedFrom_ ) throw()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ )
{
}

/**
 * Destructor. Erases the vector of supported file types
 */
CFileHandler::~CFileHandler() throw()
{
  supportedFileTypesVec.clear();
}

/*****************
 * Other methods *
 *****************/
 
/**
 * \param sFilename filename to check
 * \returns true if the filehandler supports the format of the given extension
 */
bool CFileHandler::supports( const string& sFilename ) const throw()
{
  for ( std::vector<string>::const_iterator it = supportedFileTypesVec.begin();
    it != supportedFileTypesVec.end(); ++it )
  {
		string::size_type index = sFilename.rfind( ".", sFilename.length() );
		if ( index != string::npos )
		{
			string sSuffix = sFilename.substr( index + 1 );
    	if ( (*it) == sSuffix )
      	return true;
		}
  }  
  return false;
}

/**
 * \return file mask
 */
const string CFileHandler::legalFileTypes() const throw()
{
  std::ostringstream os;
  for ( std::vector<string>::const_iterator it = supportedFileTypesVec.begin();
    it != supportedFileTypesVec.end(); ++it )
  {
    os << "*." << (*it) << " ";
  }
  return os.str();
}

/**
 * Data types are identified by the following characters: I(nteger), C(har), B(yte) and S(hort) denote an
 * integral data type. These characters can also be of lower case and don't hold any information about the
 * actual length of the data. This is given seperately by the numbers 8, 16 or 32 for the bit length.
 * Similar to this F(loat) and D(ouble) denote floating point types with valid bit lengths of 16, 32 or
 * 64 bits.
 * \param sHeaderEntry String representation of data type from header
 * \return Data type in native enum format
 * \throws NotPresentException if the given string does not represent a valid data type
 */
CFileHandler::EDataType CFileHandler::getDataType( const std::string& sHeaderEntry ) const
	throw (NotPresentException)
{
	if ( sHeaderEntry.find_first_of( "IiCcBbSs") != string::npos )
	{
		if ( sHeaderEntry.find_first_of( "Uu") != string::npos )
		{
			if ( sHeaderEntry.find( "8" ) != string::npos )
				return DUInt8;
			else if ( sHeaderEntry.find( "16" ) != string::npos )
				return DUInt16;
			else if ( sHeaderEntry.find( "32" ) != string::npos )
				return DUInt32;
		}
		else
		{
			if ( sHeaderEntry.find( "8" ) != string::npos )
				return DInt8;
			else if ( sHeaderEntry.find( "16" ) != string::npos )
				return DInt16;
			else if ( sHeaderEntry.find( "32" ) != string::npos )
				return DInt32;
		}
	}
	else 	if ( sHeaderEntry.find_first_of( "FfDd") != string::npos )
	{
		if ( sHeaderEntry.find( "64" ) != string::npos )
			return DFloat64;
		else if ( sHeaderEntry.find( "16" ) != string::npos )
			return DFloat16;
		else if ( sHeaderEntry.find( "32" ) != string::npos )
			return DFloat32;
	}
	throw( NotPresentException(
		"Given string <"+sHeaderEntry+"> represents no valid data type", CException::RECOVER, ERR_WRONGKEYTYPE ) );
}

const string CFileHandler::dump() const throw()
{
  std::ostringstream os;
  os << "\nsupportedFileTypes size: " << supportedFileTypesVec.size() << " ";
  os << "\nlegalFileTypes: " << legalFileTypes() << " ";
  return CBase::dump() + os.str();
}
