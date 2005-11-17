/** \post POSTCOND: object is marked as being uninitialised */
template <typename TDataType>
CAutoValue<TDataType>::CAutoValue() : bIsDefined( false )
{
}

/** 
 * \param aValue value used to initialise the object 
 * \pre PRECOND: aValue can be casted implicitly into the value type of the current object
 * \post POSTCOND: new value is stored in object and object is marked as being initialised
 */
template<typename TDataType>
template<typename U>
CAutoValue<TDataType>::CAutoValue( const U& aValue ) 
	: bIsDefined( true ), theValue( aValue )
{
}

/** 
 * \param aValue value to assign 
 * \pre PRECOND: aValue can be casted implicitly into the value type of the current object
 * \post POSTCOND: new value is stored in object
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
 * \post POSTCOND: object is returned to uninitialised state
 */
template <typename TDataType>
void CAutoValue<TDataType>::clear()
{
	bIsDefined = false;
}
