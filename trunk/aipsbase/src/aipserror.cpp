/************************************************************************
 * File: aipserror.cpp                                                  *
 * Project: AIPS                                                        *
 * Description: Definition of error codes and error reporting function  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Created: 21.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "aipserror.h"

using namespace std;
   
/**
 * Should be used for error string generation within exception handling
 * For simplified usage, a macro named SERROR is defined which only takes sErrorMsg and handles
 * the other strings automatically
 * \param sFile name of source file
 * \param uiLine line number of error/exception occurence
 * \param sFunction function/method name
 * \param sErrorMsg descriptive error message
 * \returns Prettyprinted error string
 */
string aips::errorString ( const char* sFile, uint uiLine,
  const char* sFunction, const char* sErrorMsg )
{
  ostringstream os;
  os << sErrorMsg << "\n######### [" << sFile << "] Line (" << uiLine << ") in \n######### "
      << sFunction << "\n";
  string sTmp( os.str() );
  return sTmp;
}
