/************************************************************************
 * File: aipsautovalue.h                                                *
 * Project: AIPS basic library                                          *
 * Description: A value type that checks if it has been initialized     *
 *              properly                                                *
 *                                                                      *
 * Author: Hendrik Belitz <hendrik@darkon.info>                         *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-05-17                                                  *
 * Based on: A suggestion of Hyman Rosen made on                        *
 *  comp.lang.c++.moderated on Thu, 12 May 2005 16:28:58 +0200          *
 * Changed:                                                             *
 *  2005-11-17 Renamed file to aipsautovalue.h                          *
 *             Implementation moved to aipsautovalueinlines.tpp         *
 *             Added documentation                                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSAUTOVALUE_H
#define AIPSAUTOVALUE_H

#include <stdexcept>

namespace aips {

/**
 * A value type that checks if it has been initialized properly
 */
template <typename TDataType>
class CAutoValue
{
public:
	/// Standard constructor. Calling this will result in getting an uninitialised variable
	CAutoValue();
	/// Copy constructor which will set the object to an initialised state.
	template<typename U>
	CAutoValue( const U& aValue );
	/// Assignment operator which will set the object to an initialised state.
	template<typename U>
	CAutoValue& operator=( const U& aValue );
	/// Return the current value
	operator TDataType&();
	/// Return the current value (const version)
	operator const TDataType&() const;
	/// Returns if the variable has been initialised
	bool defined() const;
	/// Resets the variable into an uninitialised state
	void clear();
private:
	bool bIsDefined;    ///< Is this variable initialised?
	TDataType theValue;	///< The stored value
};

#include <aipsautovalueinlines.tpp>
}

#endif
