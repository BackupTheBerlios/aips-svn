/************************************************************************
 * File: ctypedmap.cpp                                                  *
 * Project: AIPS                                                        *
 * Description: Associative structure for file header information       *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 16.12.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "ctypedmap.h"

using namespace aips;
using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/** Normally this constructor should be used */
CTypedMap::CTypedMap() throw()
  : CBase( "CTypedMap", CTYPEDMAP_VERSION, "CBase" )
{    
}

/**
 * \param aTypedMap typed map object to assign from
 */
CTypedMap::CTypedMap( CTypedMap& aTypedMap ) throw()
  : CBase( "CTypedMap", CTYPEDMAP_VERSION, "CBase" )
{
  valueMap = aTypedMap.valueMap;
	keyVec = aTypedMap.keyVec;
}

CTypedMap::~CTypedMap() throw()
{  
}

/*************
 * Accessors *
 *************/
 
/**
 * \param sKey key string
 * \return string value of key
 * \throws NotPresentException if the supplied key wasn't found in the map
 */
std::string CTypedMap::getString( const std::string& sKey ) const throw( NotPresentException )
{
	if ( isDefined( sKey ) )
	{
		string sValue = valueMap.find(sKey)->second;	
  	return sValue.substr( 1 );
	}
	throw( NotPresentException( "Key not defined <"+sKey+">", CException::RECOVER, ERR_KEYNOTDEFINED ) );
}

/**
 * \param sKey key string
 * \return double value of key
 * \throws NotPresentException if the supplied key wasn't found in the map
 * \throws NotPresentException if the key value is not a numerical type
 */
double CTypedMap::getDouble( const std::string& sKey ) const throw( NotPresentException )
{
	if ( isDefined( sKey ) )
	{
	  istringstream is ( valueMap.find(sKey)->second );
		char sign;
  	double dValue;
  	is >> sign >> dValue;
		if ( sign == 's' )
			throw( NotPresentException( "Value is not of type <double> for key <"+sKey+">", CException::RECOVER ) );
  	return dValue;
	}
	throw( NotPresentException( "Key not defined <"+sKey+">", CException::RECOVER, ERR_KEYNOTDEFINED ) );
}

/**
 * Returns the long value stored under the given key
 * \param sKey key string
 * \return long value of key
 * \throws NotPresentException if the supplied key wasn't found in the map
 * \throws NotPresentException if the key value wasn't ulong or bool
 */
long CTypedMap::getLong( const std::string& sKey ) const throw( NotPresentException )
{
	if ( isDefined( sKey ) )
	{
	  istringstream is ( valueMap.find(sKey)->second );
		char sign;
  	long lValue;
	  is >> sign >> lValue;
		if ( sign != 'l' && sign != 'b' )
			throw( NotPresentException( "Value is not of type <long> for key <"+sKey+">", 
				CException::RECOVER, ERR_WRONGKEYTYPE ) );
  	return lValue;
	}
	throw( NotPresentException( "Key not defined <"+sKey+">", CException::RECOVER, ERR_KEYNOTDEFINED ) );
}

/**
 * Returns the ulong value stored under the given key
 * \param sKey key string
 * \return ulong value of key
 * \throws NotPresentException if the supplied key wasn't found in the map
 * \throws NotPresentException if the key value wasn't ulong or bool
 */
ulong CTypedMap::getUnsignedLong( const std::string& sKey ) const throw( NotPresentException )
{
	if ( isDefined( sKey ) )
	{
	  istringstream is ( valueMap.find(sKey)->second );
  	ulong ulValue;
		char sign;
	  is >> sign >> ulValue;
		if ( sign != 'u' && sign != 'b' )
			throw( NotPresentException( "Value is not of type <unsigned long> for key <"+sKey+">", 
				CException::RECOVER, ERR_WRONGKEYTYPE ) );
  	return ulValue;
	}
	throw( NotPresentException( "Key not defined <"+sKey+">", CException::RECOVER, ERR_KEYNOTDEFINED ) );
}

/**
 * Returns the bool value stored under the given key
 * \param sKey key string
 * \return bool value of key
 * \throws NotPresentException if the supplied key wasn't found in the map
 * \throws NotPresentException if the key value wasn't bool
 */
bool CTypedMap::getBool( const std::string& sKey ) const throw( NotPresentException )
{
	if ( isDefined( sKey ) )
	{
	  istringstream is ( valueMap.find(sKey)->second );
  	bool bValue;
		char sign;
	  is >> sign >> bValue;
		if ( sign != 'b' )
			throw( NotPresentException( "Value is not of type <bool> for key <"+sKey+">", 
				CException::RECOVER, ERR_WRONGKEYTYPE ) );
  	return bValue;
	}
	throw( NotPresentException( "Key not defined <"+sKey+">", CException::RECOVER, ERR_KEYNOTDEFINED ) );
}

/**
 * \param sKey key to get the type for
 * \returns type_info of the value for the given key
 * \throws NotPresentException if the supplied key wasn't found in the map
 */
const std::type_info& CTypedMap::getValueType( const std::string& sKey ) const
	throw( NotPresentException )
{
	if ( isDefined( sKey ) )
	{
		char sign = valueMap.find(sKey)->second[0];
		switch( sign )
		{
			case 's':
				return typeid( std::string );
			case 'd':
				return typeid( double );
			case 'u':
				return typeid( ulong );
			case 'l':
				return typeid( long );
			case 'b':
				return typeid( bool );
			default:
				throw( NotPresentException( "Illegal key type found for <"+sKey+">", 
					CException::RECOVER, ERR_WRONGKEYTYPE ) );
		}
	}
	throw( NotPresentException( "Key not defined <"+sKey+">", CException::RECOVER, ERR_KEYNOTDEFINED ) );
}

/** \returns the number of entries */
size_t CTypedMap::getNumberOfEntries()	const throw() 
{
	return valueMap.size();
}

/** \returns a vector containing all keys in the map */
std::vector<std::string> CTypedMap::getKeyList() const throw()
{	
	return keyVec;
}

/************
 * Mutators *
 ************/
 
/**
 * \param sKey key to use
 * \param sValue value to store
 */
void CTypedMap::setString( const std::string& sKey, const std::string& sValue ) throw()
{
	if ( !isDefined( sKey ) )
		keyVec.push_back( sKey );
  else if ( getValueType( sKey ) != typeid( std::string ) )
    alog << LWARN << "Type change for key <" << sKey << ">. New type is std::string, old type was "
      << getValueType( sKey ).name() << endl;
  valueMap[sKey] = "s" + sValue;
}

/**
 * \param sKey key to use
 * \param dValue value to store
 */
void CTypedMap::setDouble( const std::string& sKey, const double dValue ) throw()
{
	if ( !isDefined( sKey ) )
		keyVec.push_back( sKey );
  else if ( getValueType( sKey ) != typeid( double ) )
    alog << LWARN << "Type change for key <" << sKey << ">. New type is double, old type was "
      << getValueType( sKey ).name() << endl;    
	ostringstream os;
  os << "d" << dValue;
  valueMap[sKey] = os.str();
}

/**
 * \param sKey key to use
 * \param lValue value to store
 */
void CTypedMap::setLong( const std::string& sKey, const long lValue ) throw()
{
	if ( !isDefined( sKey ) )
		keyVec.push_back( sKey );
  else if ( getValueType( sKey ) != typeid( long ) )
    alog << LWARN << "Type change for key <" << sKey << ">. New type is long, old type was "
      << getValueType( sKey ).name() << endl;    
  ostringstream os;
  os << "l" << lValue;
	valueMap[sKey] = os.str();
}

/**
 * \param sKey key to use
 * \param ulValue value to store
 */
void CTypedMap::setUnsignedLong( const std::string& sKey, const ulong ulValue ) throw()
{
	if ( !isDefined( sKey ) )
		keyVec.push_back( sKey );
  else if ( getValueType( sKey ) != typeid( ulong ) )
    alog << LWARN << "Type change for key <" << sKey << ">. New type is unsigned long, old type was "
      << getValueType( sKey ).name() << endl;    
  ostringstream os;
  os << "u" << ulValue;
	valueMap[sKey] = os.str();
}

/**
 * \param sKey key to use
 * \param bValue value to store
 */
void CTypedMap::setBool( const std::string& sKey, const bool bValue ) throw()
{
	if ( !isDefined( sKey ) )
		keyVec.push_back( sKey );
  else if ( getValueType( sKey ) != typeid( bool ) )
    alog << LWARN << "Type change for key <" << sKey << ">. New type is bool, old type was "
      << getValueType( sKey ).name() << endl;    
  ostringstream os;
  os << "b" << bValue;
	valueMap[sKey] = os.str();
}

/*************
 * Operators *
 *************/
 
/**
 * \param aHeader header object to assign from
 */
CTypedMap& CTypedMap::operator=( const CTypedMap& aTypedMap ) throw()
{
  if ( &aTypedMap == this )
    return *this;
  valueMap = aTypedMap.valueMap;
  keyVec = aTypedMap.keyVec;
  return *this;
}


/**
 * Appends the given CTypedMap to the actual map. Duplicate entries will not be overwritten
 * \param aTypedMap header object to assign from
 * \returns true if succesful, false if duplicate entries were found
 */	
bool CTypedMap::append( CTypedMap& aTypedMap ) throw()
{
	bool bDuplicatesFound = false;
	vector<string> otherKeys = aTypedMap.getKeyList();
	for( vector<string>::const_iterator it = otherKeys.begin(); it != otherKeys.end(); ++it )
	{
		if ( !isDefined( *it ) )
			valueMap[ *it ] = aTypedMap.valueMap[ *it ];
		else
			bDuplicatesFound = true;
	}
	return bDuplicatesFound;
}

/*****************
 * Other methods *
 *****************/
 
/** 
 * \param sKey key to search for
 */
bool CTypedMap::isDefined( const string& sKey ) const throw()
{
	if ( valueMap.find( sKey ) == valueMap.end() )
		return false;
	return true;
}

/** 
 * \returns wether a value for the given key is set or not 
 */
bool CTypedMap::hasKeyValue( const std::string &sKey )
		throw( NotPresentException )
{
	if ( !isDefined( sKey ) )
		throw( NotPresentException( "Key not defined <"+sKey+">", CException::RECOVER, ERR_KEYNOTDEFINED ) );
	return( !valueMap[sKey].empty() );
}

const string CTypedMap::dump() const throw()
{
  return CBase::dump() + "valueMap size: " + lexical_cast<string>( valueMap.size() ) + "\n";
}

void CTypedMap::clear() throw()
{
	valueMap.clear();
	keyVec.clear();
}

/**
 * \param sFilename name of file to write to
 * \param sDescription description written into XML dump
 */
void CTypedMap::writeXMLFile( const string sFilename, const string sDescription ) throw( FileException )
{
	ofstream file( sFilename.c_str() );
	if ( !file.is_open() )
		throw( FileException( "Error on file creation", CException::RECOVER, ERR_FILECREATIONERROR ) );
	try
	{
		file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl << "<typedmap creator=\"aips\" description=\""
			<< sDescription << "\">" << endl;		
		for( vector<string>::iterator it = keyVec.begin(); it != keyVec.end(); ++it )
		{
			file << "<entry>\n <key>" << *it << "</key>\n <type>";
			if ( getValueType( *it ) == typeid( string ) )
				file << "string</type>\n <value>" << getString( *it );
			else if ( getValueType( *it ) == typeid( double ) )
				file << "double</type>\n <value>" << getDouble( *it );
			else if ( getValueType( *it ) == typeid( ulong ) )
				file << "unsigned long</type>\n <value>" << getUnsignedLong( *it );
			else if ( getValueType( *it ) == typeid( long ) )
				file << "long</type>\n <value>" << getLong( *it );
			else if ( getValueType( *it ) == typeid( bool ) )
				file << "bool</type>\n <value>" << getBool( *it );
			file << "</value>\n</entry>" << endl;
		}		 
		file << "</typedmap>" << endl;
		file.close();
	}
	catch( exception &e )
	{
		throw( FileException( SERROR( e.what() ), CException::RECOVER, ERR_FILEACCESS ) );
	}
}

/**
 * \param sFilename file to read XML dump from
 */
void CTypedMap::readXMLFile( const std::string sFilename )	throw( FileException )
{
	#ifdef USE_XMLPP
	struct SEntry
	{
		string key;
		string type;
		string value;
	};

	try
  {
    xmlpp::TextReader reader( sFilename.c_str() );
		bool bLegalMap = false;
		bool bInsideEntry = false;
		uint uiReadEntry = 0;
		SEntry entry;
    while(reader.read())
    {
      int depth = reader.get_depth();
			cerr << "On depth "<< depth << endl;
      if ( depth != 0 && !bLegalMap )
      {
      	throw( FileException( "Invalid XML document structure" ) );
      }
      else if ( depth == 0 ) // Global level. Only "typedmap", #comment or #text may reside here
      {
      	
      	string sNodeName = reader.get_name();
      	if ( sNodeName[0] != '#' && sNodeName != "typedmap" )
      		throw( FileException( "Illegal tag in XML file" ) );
      	else if ( sNodeName == "typedmap" )
      	{
      		bLegalMap = !bLegalMap;
      	}
      	else if ( sNodeName == "#comment" )
      	{
      		alog << LDEBUG << "Found comment: <" << reader.get_value() << ">" << endl;
      	}
      	else if ( sNodeName == "#text" )
      	{
      		string sNodeData = reader.get_value();
      		if ( stripEnclosingChars( sNodeData ).length() != 0 )
      			throw( FileException( "Illegal tag in XML file" ) );
      	}
      }
      else if ( depth == 1 ) // Inside typedmap. #comment, #text and "entry" are allowed
      {
      	string sNodeName = reader.get_name();
      	if ( sNodeName[0] != '#' && sNodeName != "entry" )
      		throw( FileException( "Illegal tag in XML file <"+sNodeName+">" ) );
      	else if ( sNodeName == "entry" )
      	{
      		if ( bInsideEntry )
      		{
      			if ( entry.type[0] == 's' )
      				setString( entry.key, entry.value );
      			else if ( entry.type[0] == 'd' )
      				setDouble( entry.key, lexical_cast<double>( entry.value ) );
      			else if ( entry.type[0] == 'l' )
      				setLong( entry.key, lexical_cast<long>( entry.value ) );
      			else if ( entry.type[0] == 'u' )
      				setUnsignedLong( entry.key, lexical_cast<ulong>( entry.value ) );
      			else if ( entry.type[0] == 'b' )
      				setBool( entry.key, lexical_cast<bool>( entry.value ) );
      		}
      		bInsideEntry = !bInsideEntry;
      	}
      	else if ( sNodeName == "#comment" )
      	{
      		alog << LDEBUG << "Found comment: <" << reader.get_value() << ">" << endl;
      	}
      	else if ( sNodeName == "#text" )
      	{
      		string sNodeData = reader.get_value();
      		sNodeData = stripEnclosingChars( sNodeData );
      		if ( sNodeData.length() != 0 )
      			throw( FileException( "Illegal tag in XML file <"+sNodeData+">" ) );
      	}
      }
      else if ( depth == 2 && bInsideEntry ) // key, type, value and #text, #comment
      {
      	string sNodeName = reader.get_name();

      	if ( sNodeName == "key" )
      	{
      		if ( uiReadEntry != 0 && uiReadEntry != 1 )
      			throw( FileException( "Illegal tag in XML file" ) );
      		uiReadEntry = (uiReadEntry==0)?1:0;
      	}
      	else if ( sNodeName == "type" )
      	{
      		if ( uiReadEntry != 0 && uiReadEntry != 2 )
      			throw( FileException( "Illegal tag in XML file" ) );
      		uiReadEntry = (uiReadEntry==0)?2:0;
      	}
      	else if ( sNodeName == "value" )
      	{
      		if ( uiReadEntry != 0 && uiReadEntry != 3 )
      			throw( FileException( "Illegal tag in XML file" ) );
      		uiReadEntry = (uiReadEntry==0)?3:0;
      	}
      	else if ( sNodeName == "#comment" )
      	{
      		alog << LDEBUG << "Found comment: <" << reader.get_value() << ">" << endl;
      	}
      	else if ( sNodeName == "#text" )
      	{
      		string sNodeData = reader.get_value();
      		if ( stripEnclosingChars( sNodeData ).length() != 0 )
      			throw( FileException( "Illegal tag in XML file" ) );
      	}
      	else
      		throw( FileException( "Illegal tag in XML file" ) );
      }
      else if ( depth == 3 && bInsideEntry && uiReadEntry )
      {
      	string sNodeName = reader.get_name();
      	if ( sNodeName[0] != '#' )
      		throw( FileException( "Illegal tag in XML file" ) );
      	if ( sNodeName == "#comment" )
      	{
      		alog << LDEBUG << "Found comment: <" << reader.get_value() << ">" << endl;
      	}
      	else if ( sNodeName == "#text" )
      	{
      		string sNodeData = reader.get_value();
      		if ( stripEnclosingChars( sNodeData ).length() != 0 )
      		{
      			switch ( uiReadEntry )
      			{
      				case 1:
      					entry.key = sNodeData;
      					break;
      				case 2:
      					entry.type = sNodeData;
      					break;
      				case 3:
      					entry.value = sNodeData;
      					break;
      			}
      		}
      	}
      }      
    }
  }
  catch(const std::exception& e)
  {
    throw( FileException( SERROR( e.what() ), CException::RECOVER, ERR_FILEACCESS ) );
  }
  #else
  #warning Compiling without XML++. CTypedMap will not be able to read XML dumps
  #endif  
}
