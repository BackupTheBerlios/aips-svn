/************************************************************************
 * File: aipserror.cpp                                                  *
 * Project: AIPS                                                        *
 * Description: Definition of error codes and error reporting function  *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 0.1                                                         *
 * Created: 21.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include <cstring>
#include "aipserror.h"

using namespace std;
   
/**
 * Should be used for error/message string generation within function calls
 * on entering and leaving a function
 * \param sFunction function/method name
 * \param bEnter wether function was entered or not
 * \param sErrorMsg descriptive error message ( may be empty )
 * \returns Pretty-printed error string (
 */
string aips::functionString ( const char* sFunction, bool bEnter, const char* sErrorMsg )
	throw()
{
  ostringstream os;
  if ( bEnter )
  	os << "(+++++) " << sFunction << " (+++++)\n";
  else
  	os << "(-----) " << sFunction << " (-----)\n"; 
  if ( strlen( sErrorMsg ) > 0 )
  	os << sErrorMsg << "\n ";
  string sTmp( os.str() );
  return sTmp;
}

/**
 * Should be used for short descriptions of errors or debug outputs
 * For simplified usage, a macro named SSHORTERROR is defined which 
 * only takes sErrorMsg and handles the line number automatically
 * \param uiLine line number of error/exception occurence
 * \param sErrorMsg descriptive error message
 * \returns Pretty-printed error string
 */
std::string shortErrorString ( unsigned int uiLine, const char* sErrorMsg ) 
	throw()
{
  ostringstream os;
  os << "Line (" << uiLine << "): " << sErrorMsg << "\n"; 
  string sTmp( os.str() );
  return sTmp;
}

/**
 * Should be used for error string generation within exception handling
 * For simplified usage, macros named SERROR and SFULLERROR are defined 
 * which only takes sErrorMsg and handles the other parameters automatically
 * \param sFile name of source file
 * \param uiLine line number of error/exception occurence
 * \param sFunction function/method name
 * \param sErrorMsg descriptive error message
 * \returns Pretty-printed error string
 */
string aips::errorString ( const char* sFile, unsigned int uiLine,
  const char* sFunction, const char* sErrorMsg ) throw()
{
  ostringstream os;
  os << "\n######### [" << sFile << "] Line (" << uiLine << ") in \n######### "
      << sFunction << "\n" << sErrorMsg << "\n";
  string sTmp( os.str() );
  return sTmp;
}
