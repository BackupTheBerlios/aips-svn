/************************************************************************
 * File: aipsautovalue.h                                                *
 * Project: AIPS basic library                                          *
 * Description: A value type that checks if it has been initialised     *
 *              properly                                                *
 *                                                                      *
 * Author: Hendrik Belitz <hbelitz@users.berlios.de>                          *
 *                                                                      *
 * Created: 2005-05-17                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

/** \post object is marked as being uninitialised */
template <typename TDataType>
CAutoValue<TDataType>::CAutoValue() : bIsDefined( false )
{
}

/** 
 * \param aValue value used to initialise the object 
 * \pre aValue can be casted implicitly into the value type of the current object
 * \post new value is stored in object and object is marked as being initialised
 */
template<typename TDataType>
template<typename U>
CAutoValue<TDataType>::CAutoValue( const U& aValue ) 
	: bIsDefined( true ), theValue( aValue )
{
}

/** 
 * \param aValue value to assign 
 * \pre aValue can be casted implicitly into the value type of the current object
 * \post new value is stored in object
 */
template<typename TDataType>
template<typename U>
CAutoValue<TDataType>& CAutoValue<TDataType>::operator=( const U& aValue )
{
	theValue = aValue;
	bIsDefined = true;
	return *this;
}

/** 
 * \returns the value of the object 
 * \throws std::runtime_error if variable is not initialised
 */
template <typename TDataType>
CAutoValue<TDataType>::operator TDataType&()
{
	if ( !bIsDefined )
		throw std::runtime_error( "Uninitialised variable" );
	return theValue;
}
	
/** 
 * \returns the value of the object
 * \throws std::runtime_error if variable is not initialised
 */ 
template <typename TDataType>	
CAutoValue<TDataType>::operator const TDataType&() const
{
	if( !bIsDefined )
		throw std::runtime_error( "Uninitialised variable" );
	return theValue;
}
	
/** 
 * \returns true if variable was initialised 
 */
template <typename TDataType>
bool CAutoValue<TDataType>::defined() const
{
	return bIsDefined;
}
	
/** 
 * \post object is returned to uninitialised state
 */
template <typename TDataType>
void CAutoValue<TDataType>::clear()
{
	bIsDefined = false;
}
