/************************************************************************
 * File: csinglevalue.h                                                 *
 * Project: AIPS                                                        *
 * Description: A dataset representing a single value                   *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                    *
 *                                                                      *
 * Version: 0.6                                                         *
 * Status : Beta                                                        *
 * Created: 2003-11-25                                                  *
 * Changed: 2003-11-27 Added class CSingleValue for zerodimensional     *
 *                     values like scalars and vectors                  *
 *                     Template methods are now seperated from          *
 *                     declarations                                     *
 *                     CDataSet is now derived from CBase               *
 *          2004-01-16 Corrected an error in CTypedData::operator=      *
 *                      This member also is no longer virtual           *
 *                     Moved CVector classes to aipsnumeric.h           *
 *          2004-01-20 Virtual methods and structors are no longer      *
 *                      inline                                          *
 *                     Made the code simpler and look prettier          *
 *                     Deleted method convert() from CTypedData<>       *
 *                     File splitted up into the three files            *
 *                      cdataset.h csinglevalue.h and ctypeddata.h      *
 *          2004-04-26 Added method swap()                              *
 *          2004-04-28 Updated documentation                            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CSINGLEVALUE_H
#define CSINGLEVALUE_H

#define CSINGLEVALUE_VERSION "0.6"

// AIPS includes
#include "cdataset.h"

namespace aips {

/**
 * A data set that represents a single value. Single values should be either
 * scalar values or mathematical vectors of any type.
 */
template<typename valueType>
class CSingleValue : public CDataSet
{
private:
  /// Standard constructor
  CSingleValue();
public:
  typedef valueType TDataType;
/* Structors */
  /// Constructor for scalar values
  explicit CSingleValue( valueType theValue_ )
    throw();
  /// Constructor for vectors
  CSingleValue( valueType* valueArr_, size_t dataDimensionSize_ )
    throw();
  /// Copy Constructor
  CSingleValue( const CSingleValue& aSingleValue )
    throw();
  /// Destructor
  virtual ~CSingleValue()
    throw();
/* Operators (non-access) */
  /// Operator=
  inline CSingleValue& operator=( const CSingleValue& aSingleValue )
    throw();
/* Accessors */
  /// Returns the type of the single value
  virtual const std::type_info& getType() const
    throw();
  /// Return a handle to the void array of the data
  virtual void* getVoidArray()
    throw();
  /// Return a handle to the typed array of the data
  inline valueType* getArray()
    throw();
/* Operators (access) */		
  /// Access operator for data members. This won't make any range checking!
  inline valueType& operator()( const unsigned short usIndex = 0 )
    throw();
  /// Const version of the access operator
  inline const valueType& operator()( const unsigned short usIndex = 0 ) const
    throw();
  /// Access operator for data members. This does range checking.
  inline valueType& at( unsigned short usIndex = 0 )
    throw( OutOfRangeException );
  /// Const version of access operator for data members. This does range checking.
  inline const valueType& at( unsigned short usIndex = 0 ) const
    throw( OutOfRangeException );
/* Other Methods */
  /// Produces an information string about the actual object.
  virtual const std::string dump() const
    throw();
	/// Swaps the data with another data set of the same type
	void swap( CSingleValue<valueType>& aDataSet ) 
		throw();
private:
  std::vector<valueType> valueVec; ///< Data values
};

#include "csinglevalueinlines.tpp"

}
#endif
