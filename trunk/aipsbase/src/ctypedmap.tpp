/**
 * /param sKey key to search for
 * /param theValue reference to return casted value in
 * /returns true if succesful, false if given key/value pair could'nt be casted into the given type
 * /throws NotPresentException if the given key cannot be found
 */
 
template<typename T> bool CTypedMap::getAs( const std::string& sKey, T& theValue ) const
	throw( NotPresentException )
{	
	if ( isDefined( sKey ) )
	{		
		std::string sValue = valueMap.find(sKey)->second.substr( 1 );	
		try
		{
  		theValue = boost::lexical_cast<T>( sValue.substr( 1 ) );
		}
		catch ( boost::bad_lexical_cast& )
		{
			return false;
		}
		return true;
	}
	throw( NotPresentException( "Key not defined <"+sKey+">", RECOVER, ERR_KEYNOTDEFINED ) );
}
