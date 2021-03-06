/************************************************************************
 * File: aipserror.h                                                    *
 * Project: AIPS                                                        *
 * Description: Definition of error codes and error reporting function  *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 1.0                                                         *
 * Status : Stable                                                      *
 * Created: 2004-01-21                                                  *
 * Changed: 2004-04-26 Macro SERROR moved to this file                  *
 *          2005-07-07 Added runtime type error codes                   *
 * ToDo: Add more specific error codes                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef AIPSERROR_H
#define AIPSERROR_H
 
// Standard includes
#include <sstream> 	// std::ostringstream

namespace aips {

/// Produces a string signaling entering or leaving a function
std::string functionString( const char* sFunction, 
	bool bEnter = true, const char* sErrorMsg = "" ) throw();

/// Produces a short error string with line number
std::string shortErrorString( unsigned int uiLine, const char* sErrorMsg ) throw();

/// Produces an error string with line number, file and function name
std::string errorString( const char* sFile, unsigned int uiLine, const char* sFunction,
  const char* sErrorMsg ) throw();

/** \def SSHORTERROR(s) A macro for simplified usage of the shortErrorString function */
#define SSHORTERR( s ) shortErrorString( __LINE__, s )

/** \def SERROR(s) A macro for simplified usage of the errorString function */
#define SERROR( s ) errorString( __FILE__, __LINE__, __FUNCTION__, s )

/** \def SFULLERROR(s) A macro for simplified usage of the errorString function */
#define SFULLERR( s ) errorString( __FILE__, __LINE__, __PRETTY_FUNCTION__, s )
 
// Generic error codes
const unsigned int ERR_NOERROR  =   0; ///< No error at all ( never use with exceptions ;-)
const unsigned int ERR_GENERIC  = 100; ///< Generic error
const unsigned int ERR_FILE     = 200; ///< File error
const unsigned int ERR_NULL     = 300; ///< NULL pointer found where it wasn't allowed
const unsigned int ERR_RANGE    = 400; ///< Range / Bounds error
const unsigned int ERR_INIT     = 500; ///< Initialisation error
const unsigned int ERR_MISSING  = 600; ///< Failed presence and missing objects or features errors
const unsigned int ERR_PLUGIN   = 700; ///< Some sort of plugin error occured
const unsigned int ERR_TYPE     = 800; ///< Some sort of runtime type error

// More specific error codes
// File errors
const unsigned int ERR_FILENOTFOUND           = 201; ///< Desired file was not found
const unsigned int ERR_FILEFORMATUNSUPPORTED  = 202; ///< The format of the file is not supported
const unsigned int ERR_FILEACCESS             = 203; ///< Error on accessing data in file
const unsigned int ERR_ILLEGALFILENAME        = 204; ///< Illegal filename
const unsigned int ERR_FILECREATIONERROR      = 205; ///< Error occured on creation of file
const unsigned int ERR_FILEHEADER             = 206; ///< The file header is faulty or unsupported
const unsigned int ERR_FILESIZEMISMATCH       = 207; ///< File size and header info do not match

// NULL pointer errors
const unsigned int ERR_CALLERNULL             = 301; ///< Callers parameter was NULL
const unsigned int ERR_REQUESTNULL            = 302; ///< Requested data member is NULL
const unsigned int ERR_UNKNOWNTYPE            = 303; ///< Some object type cannot be determined, so casting yields NULL

// Range and bounds errors
const unsigned int ERR_BADDIMENSION           = 401; ///< Dataset dimension is invalid
const unsigned int ERR_BADCOORDS              = 402; ///< Invalid coordinates in data set
const unsigned int ERR_ARRAYTOOSMALL          = 403; ///< Given array is too small

// "Missing" errors
const unsigned int ERR_DIALOGNOTPRESENT       = 601; ///< Requested GUI widget does not exist
const unsigned int ERR_KEYNOTDEFINED          = 630; ///< A key in a associative array was not found
const unsigned int ERR_WRONGKEYTYPE           = 631; ///< Requested key type is wrong

// "Type" errors
const unsigned int ERR_PORTTYPE               = 701; ///< Module port types do not match

} // namespace aips

#endif
