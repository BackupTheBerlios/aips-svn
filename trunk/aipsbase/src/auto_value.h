/************************************************************************
 * File: auto_value                                                     *
 * Project: AIPS basic library                                          *
 * Description: A value type that checks if it has been initialized     *
 *              properly                                                *
 *                                                                      *
 * Author: Hendrik Belitz <hendrik@darkon.info>                         *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-05-17                                                  *
 * Based on: A suggestion of Hyman Rosen made on                        *
 *  comp.lang.c++.moderated on Thu, 12 May 2005 16:28:58 +0200          *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AUTO_VALUE_H
#define AUTO_VALUE_H

#define AUTO_VALUE_VERSION "0.1"

#include <stdexcept>

namespace aips {

/**
 * A value type that checks if it has been initialized properly
 */
template <typename T>
class auto_value
{
public:
	/// Standard constructor. Calling this will result in getting an uninitialised variable
	auto_value() : bIsDefined( false )
	{}
	/// Copy constructor which will set the auto_value to an initialised state.
	template<typename U>
	auto_value( const U& otherValue ) : bIsDefined( true ), theValue( otherValue )
	{}
	/// Assignment operator which will set the auto_value to an initialised state.
	template<typename U>
	auto_value& operator=( const U& otherValue )
	{
		theValue = otherValue;
		bIsDefined = true;
		return *this;
	}
	operator T&()
	{
		if ( !bIsDefined )
			throw std::runtime_error( "Uninitialised variable" );
		return theValue;
	}
	operator const T&() const
	{
		if( !bIsDefined )
			throw std::runtime_error( "Uninitialised variable" );
		return theValue;
	}
	/// Returns if the variable has been initialised and defined
	bool defined() const
	{
		return bIsDefined;
	}
	/// Resets the variable into uninitialised state
	void clear()
	{
		bIsDefined = false;
	}
private:
	bool bIsDefined; ///< Is this variable initialised?
	T theValue;	     ///< The stored value
};

}

#endif
