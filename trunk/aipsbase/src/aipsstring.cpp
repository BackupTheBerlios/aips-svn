/************************************************************************
 * File: aipsstring.h                                                   *
 * Project: AIPS aipsbase library                                       *
 * Description: Some useful string routines                             *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 2004-04-28                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
  
#include "aipsstring.h"

namespace aips {

/**
 * \param aString string to strip from 'whitespaces'
 * \param sEnclosingChars characters which are used as 'whitespaces'
 */
std::string stripEnclosingChars( const std::string& aString, 
	const std::string& sEnclosingChars ) throw ()
{
	std::string::size_type begin = aString.find_first_not_of( sEnclosingChars );
	std::string::size_type end = aString.find_last_not_of( sEnclosingChars );
	if ( begin != std::string::npos )
	{
		if ( end != std::string::npos )
			return std::string( aString.substr( begin, end - begin + 1 ) );
		else
			return std::string( aString.substr( begin ) );
	}
	else
	{
		if ( end != std::string::npos )
			return std::string( aString.substr( 0, end + 1 ) );
		else
			return "";
	}
}

};
