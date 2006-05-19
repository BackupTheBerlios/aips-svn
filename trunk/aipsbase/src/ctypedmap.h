/************************************************************************
 * File: ctypedmap.h                                                    *
 * Project: AIPS                                                        *
 * Description: A class to strongly typed information in an             *
 *              associative array                                       *
 *                                                                      *
 * Author: Hendrik Belitz  (hbelitz@users.berlios.de)                         *
 *         Patrick Musmann (p.musmann@fz-juelich.de)                    *
 *                                                                      *
 * Version: 0.13                                                        *
 * Status : Beta                                                        *
 * Created: 2004-12-16                                                  *
 * Changed: 2003-12-17 Added more conversion methods                    *
 *          2004-01-20 Renamed set methods                              *
 *          2004-01-26 Implemented dump()-method                        *
 *                     Added copy constructor, assignment operator and  *
 *                      documentation                                   *
 *          2004-04-22 Removed unneccessary includes                    *
 *                     Added method isDefined()                         *
 *          2004-04-27 Incorporated some changes and insprirations      *
 *                      by P. Musmann                                   *
 *                     Added hasKeyValue() and getKeyType()             *
 *                     Added exceptions and type safety for accessors   *
 *                     Added accessors for bool and ulong               *
 *                     Added load and save methods                      *
 *                     Updated documentation                            *
 *          2004-04-29 Renamed class to CTypedMap                       *
 *          2004-04-30 Added getNumberOfEntries() and getKeyList()      *
 *          2004-05-04 Corrected some nasty errors in CTypedMap         *
 *                     getKeyList() now returns a sorted vector of keys *
 *          2004-11-22 Class now provides new operations append() and   *
 *                     getAs()                                          *
 *          2004-12-07 Removed load and save methods                    *
 *          2005-06-06 Corrected an error in assignment operator        *
 *          2005-07-08 Load and save data as XML added                  *
 *          2006-04-18 Output of warning if setting an existing key     *
 *                     changes key type.                                *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CTYPEDMAP_H
#define CTYPEDMAP_H

#define CTYPEDMAP_VERSION "0.13"

// Standard includes
#include <string>  // std::string
#include <vector>  // std::vector
#include <map>     // std::map
#include <fstream> // std::ifstream, std::ofstream
// Boost includes
#include <boost/lexical_cast.hpp>

// AIPS includes
#include "clog.h"
#include "aipsmacros.h"
#include "cbase.h"
#ifdef USE_XMLPP
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#endif

namespace aips {

/**
 * Stores arbitrary information in key-value pairs.
 * This class provides a type-safe storage of all data entries and understands
 * the following types (> denotes implicit casting):
 * bool > unsigned long > double > string
 * bool > long > double > string
 */
class CTypedMap : public CBase
{
public:
/* Structors */
  /// Constructor
  CTypedMap()
    throw();
  /// Copy constructor
  CTypedMap( CTypedMap& aTypedMap )
    throw();
  /// Destructor
  virtual ~CTypedMap()
    throw();
/* Accessors */
  /// Returns the string value stored under the given key
  std::string getString( const std::string& sKey ) const
    throw( NotPresentException );
  /// Returns the double value stored under the given key
  double getDouble( const std::string& sKey ) const
    throw( NotPresentException );
  /// Returns the long value stored under the given key
  long getLong( const std::string& sKey ) const
    throw( NotPresentException );
	/// Returns the ulong value stored under the given key
  ulong getUnsignedLong( const std::string& sKey ) const
    throw( NotPresentException );
	/// Returns the bool value stored under the given key
  bool getBool( const std::string& sKey ) const
    throw( NotPresentException );
	/// Returns the type of the value for the given key
	const std::type_info& getValueType( const std::string& sKey ) const
    throw( NotPresentException );
	/// Tries to implicitly cast the keys value and returns it in Value
	template<typename T> bool getAs( const std::string& sKey, T& theValue ) const
		throw( NotPresentException );
	/// Returns the number of entries 
	size_t getNumberOfEntries() const
		throw();
	/// Returns a vector containing all keys in the map
	std::vector<std::string> getKeyList() const
		throw();
/* Mutators */
  /// Sets the value for the given key to a new string value
  void setString( const std::string& sKey, const std::string& sValue )
    throw();
  /// Sets the value for the given key to a new double value
  void setDouble( const std::string& sKey, const double dValue )
    throw();
  /// Sets the value for the given key to a new long value
  void setLong( const std::string& sKey, const long lValue )
    throw();
  /// Sets the value for the given key to a new ulong value
  void setUnsignedLong( const std::string& sKey, const ulong ulValue )
    throw();	
  /// Sets the value for the given key to a new bool value
  void setBool( const std::string& sKey, const bool bValue )
    throw();		
/* Operators */
  /// Assignment operator
  CTypedMap& operator=( const CTypedMap& aTypedMap )
    throw();  
	/// Append operator
	bool append( CTypedMap& aTypedMap )
		throw();
/* Other methods */
	/// Checks if the given key is defined in the typed map
	bool isDefined( const std::string& sKey ) const
		throw();
	/// Returns whether a value for the given key is set or not
  bool hasKeyValue( const std::string &sKey )
		throw( NotPresentException );  
	/// Reimplemented from CBase
  virtual const std::string dump() const 
		throw();
	/// Clears the map
	void clear()
		throw();
	/// Write data to a XML file
	void writeXMLFile( const std::string sFilename, 
		const std::string sDescription = "Typed map dump" )
		throw( FileException );
	/// Read data from a XML file
	void readXMLFile( const std::string sFilename )
		throw( FileException );
private:
  std::map<std::string, std::string> valueMap; ///< Map to store all relevant value pairs
	std::vector<std::string> keyVec;             ///< List of keys
};

#include "ctypedmap.tpp"

} // End of namespace aips
#endif
