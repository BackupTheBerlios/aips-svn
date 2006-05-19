/************************************************************************
 * File: aipsstring.h                                                   *
 * Project: AIPS aipsbase library                                       *
 * Description: Some useful string routines                             *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status:  Beta                                                        *
 * Created: 2004-04-28                                                  *
 * Changed: 2004-09-13 Removed unnecessary functions. Conversions and   *
 *                     tokenizing are now done via boost                *
 *          2004-12-22 Corrected an error in stripEnclosingChars        *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef AIPSSTRING_H
#define AIPSSTRING_H

#include <string> // std::string
#include <boost/lexical_cast.hpp>

namespace aips {

/// Strips unwanted characters from the beginning and the end of the string
std::string stripEnclosingChars( const std::string& aString, 
	const std::string& sEnclosingChars ="\"\'\t\n " ) throw ();
	
};

#endif
