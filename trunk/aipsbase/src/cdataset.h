/************************************************************************
 * File: cdataset.h                                                     *
 * Project: AIPS                                                        *
 * Description: Classes supporting generic data fields                  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.10                                                        *
 * Status : Beta                                                        *
 * Created: 2003-11-25                                                  *
 * Changed:                                                             *
 *        2003-11-27 Added class CSingleValue for zerodimensional       *
 *                   values like scalars and vectors                    *
 *                   Template methods are now seperated from            *
 *                   declarations                                       *
 *                   CDataSet is now derived from CBase                 *
 *        2004-01-16 Corrected an error in CTypedData::operator=        *
 *                    this member also is no longer virtual             *
 *                   Moved CVector classes to aipsnumeric.h             *
 *        2004-01-20 Virtual methods and structors are no longer inline *
 *                   Made the code simpler and look prettier            *
 *                   Deleted method convert() from CTypedData<>         *
 *                   File splitted up into the three files              *
 *                    cdataset.h csinglevalue.h and ctypeddata.h        *
 *        2004-01-21 Moved inline members to cpp file                   *
 *        2004-04-22 Removed unneccessary includes                      *
 *        2005-03-23 Added data vector for base elements dimensions     *
 *                    (e.g. physical voxel size)                        *
 *        2005-05-23 Added constructor for 1D-data sets                 *
 *                   Added parameter default to getExtent()             *
 *        2005-08-01 Minor code improvements                            *
 *        2005-11-20 Update documentation                               *
 *                   Added verbose output                               *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CDATASET_H
#define CDATASET_H

#define CDATASET_VERSION "0.10"

// Standard includes
#include <vector>
#include <sstream>

// Boost includes
#include <boost/lexical_cast.hpp> 
#include <boost/shared_ptr.hpp>   

// AIPS includes
#include "clog.h"
#include <aipsnumbertraits.h>

/// AIPS namespace for all libaipsbase classes
namespace aips {

// Some symbolic constants for dimension access

const ushort DATAWIDTH  = 0; ///< First dimension of a data set
const ushort DATAHEIGHT = 1; ///< Second dimension of a data set
const ushort DATADEPTH  = 2; ///< Third dimension of a data set
const ushort DATATIME   = 3; ///< Fourth dimension of a data set

/**
 * \brief An abstract base class for generic data fields.
 *
 * A data set is an abstract entity that can contain arbitraty data. Dimension and type of data can
 * vary from field to field. Each field entry can be a scalar value or a vector of any type. Real
 * data will be stored in the inherited classes CTypedData<> and CSingleValue<>.
 *
 * \todo Write test cases and sample code.
 * \todo Add an swap axis function.
 */
class CDataSet : public CBase
{
private:
	/// Standard constructor
	CDataSet();
public:
/** \name Structors */
  //@{
  /// Constructor
  CDataSet( const ushort usDimension_, const size_t* extentArr_,
    const size_t dataDimensionSize_ = 1, const std::string &sClassName_ = "CDataSet", 
		const std::string &sClassVersion_ = CDATASET_VERSION,
    const std::string &sDerivedFrom_ = "CBase") throw();
  /// Constructor (2nd argument overloaded)
  CDataSet( const ushort usDimension_, const std::vector<size_t> extentVec_,
    const size_t dataDimensionSize_ = 1, const std::string &sClassName_ = "CDataSet",
		const std::string &sClassVersion_ = CDATASET_VERSION,
    const std::string &sDerivedFrom_ = "CBase") throw();
  /// Constructor (for one-dimensional data sets)
  CDataSet( const size_t extent_, const size_t dataDimensionSize_ = 1,
  	const std::string &sClassName_ = "CDataSet",
		const std::string &sClassVersion_ = CDATASET_VERSION,
    const std::string &sDerivedFrom_ = "CBase") throw();   
  /// Copy Constructor
  CDataSet( const CDataSet& aDataSet )
    throw();
  /// Destructor
  virtual ~CDataSet()
    throw();
  //@}
/** \name Accessors */
  //@{
  /// Get data type. Pure virtual method.
  virtual const std::type_info& getType() const
    throw() =0;
  /// Returns the Dimension of the data set
  ushort getDimension() const
    throw();
  /// Returns the data dimension of each field entry of the data set
  size_t getDataDimension() const
    throw();
  /// Returns the extent of the given dimension
  size_t getExtent( const ushort usIndex = 0 ) const
    throw( OutOfRangeException );
  /// Get the array extents
  const std::vector<size_t> getExtents() const
    throw();
  /// Return a handle to the data array. Pure virtual method.
  virtual void* getVoidArray()
    throw() =0;
  /// Get the dimensions of the base element
  std::vector<double> getBaseElementDimensions() const
  	throw();
  /// Get one dimension of the base element
  double getBaseElementDimension( const ushort usIndex ) const
  	throw( OutOfRangeException );
  /// Get the origin of the data set
  std::vector<double> getOrigin() const
    throw();
  /// Get a coordinate of the origin of the data set
  double getOrigin( const ushort usIndex ) const
    throw( OutOfRangeException );
  //@}
/** \name Mutators */
  //@{
  /// Set a specific base element dimension
	void setBaseElementDimension( const ushort usIndex, const double dValue )
		throw( OutOfRangeException );
  /// Set all base element dimensions (vector version)
	void setBaseElementDimensions( const std::vector<double> dimensionsVec )
		throw( OutOfRangeException );
  /// Set all base element dimensions (array version)
  void setBaseElementDimensions( const double* dimensionsVec_ )
    throw();
  /// Set a specific origin coordinate
  void setOrigin( const ushort usIndex, const double dValue )
    throw( OutOfRangeException );
  /// Set all origin coordinates (vector version)
  void setOrigin( const std::vector<double> originVec_ )
    throw( OutOfRangeException );
  /// Set all origin coordinates (array version)
  void setOrigin( const double* originVec_ )
    throw();
  //@}
/** \name Assignment operators */
  //@{
  /// Assignment operator for data sets    
  CDataSet& operator=( CDataSet& aDataSet )
    throw();
  //@}
/** \name Other Methods */
  //@{
  /// Reimplemented from CBase
  virtual const std::string dump() const
    throw();
  //@}
protected:
  ushort usDimension;            ///< Dimension of the data set
  std::vector<size_t> extentVec; ///< Size of each dimension
  std::vector<double> baseElementDimensionsVec; ///< Size of one base element
  std::vector<double> originVec; ///< Size of one base element
  size_t dataDimensionSize;      ///< Dimension of each field entry
};

/** \name Type checking functions */
//@{

/**
 * Function to compare field/scalar types with a given data set type
 * Call with checkType<FieldType>( data )
 * \param aDataSet data set to check
 */
template<typename T> inline bool checkType( const CDataSet& aDataSet ) throw()
{
	return( aDataSet.getType() == typeid( typename T::TDataType ) );
}

/**
 * Function to compare field/scalar types with a given data set type
 * Call with checkType<FieldType>( data )
 * Overloaded function for pointer types
 * \param aDataSet data set to check
 */
template<typename T> inline bool checkType( const CDataSet* aDataSet ) throw()
{
	return( aDataSet->getType() == typeid( typename T::TDataType ) );
}

/**
 * Function to compare field/scalar types with a given data set type
 * Call with checkType<FieldType>( data )
 * Overloaded function for boost::shared_ptr pointer types
 * \param aDataSet data set to check
 */
template<typename T> inline bool checkType( const boost::shared_ptr<CDataSet> aDataSet ) throw()
{
	return( aDataSet->getType() == typeid( typename T::TDataType ) );
}
//@}

} // end of namespace aips
#endif
