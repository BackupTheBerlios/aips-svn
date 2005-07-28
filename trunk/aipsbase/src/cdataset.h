/************************************************************************
 * File: cdataset.h                                                     *
 * Project: AIPS                                                        *
 * Description: Classes supporting generic data fields                  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status : Beta                                                        *
 * Created: 25.11.03                                                    *
 * Changed: 27.11.03 Added class CSingleValue for zerodimensional       *
 *                   values like scalars and vectors                    *
 *                   Template methods are now seperated from            *
 *                   declarations                                       *
 *                   CDataSet is now derived from CBase                 *
 *          16.01.04 Corrected an error in CTypedData::operator=        *
 *                    This member also is no longer virtual             *
 *                   Moved CVector classes to aipsnumeric.h             *
 *          20.01.04 Virtual methods and structors are no longer inline *
 *                   Made the code simpler and look prettier            *
 *                   Deleted method convert() from CTypedData<>         *
 *                   File splitted up into the three files              *
 *                    cdataset.h csinglevalue.h and ctypeddata.h        *
 *          21.01.04 Moved inline members to cpp file                   *
 *          22.04.04 Removed unneccessary includes                      *
 *        2005-03-23 Added data vector for base elements dimensions     *
 *                    (e.g. physical voxel size)                        *
 *        2005-05-23 Added constructor for 1D-datasets                  *
 *                   Added parameter default to getExtent()             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CDATASET_H
#define CDATASET_H

// Standard includes
#include <vector> // std::vector

// Boost includes
#include <boost/lexical_cast.hpp> // boost::lexical_cast<>
#include <boost/shared_ptr.hpp>   // boost::shared_ptr<>

// AIPS includes
#include "clog.h"

/// AIPS namespace for all libaipsbase classes
namespace aips {

// Some symbolic constants for dimension access

const ushort DATAWIDTH  = 0; ///< First dimension of a data set
const ushort DATAHEIGHT = 1; ///< Second dimension of a data set
const ushort DATADEPTH  = 2; ///< Third dimension of a data set

/**
 * An abstract base class for generic data fields. A data set is an abstract
 * entity that can contain arbitraty data. Dimension and type of data can
 * vary from field to field. Each field entry can be a scalar value or a
 * vector of any type. Real data will be stored in the inherited classes
 * CTypedData<> and CSingleValue<>
 */
class CDataSet : public CBase
{
private:
	/// Standard constructor
	CDataSet();
public:
/* Structors */
  /// Constructor
  CDataSet( const ushort usDimension_, const size_t* extentArr_,
    const size_t dataDimensionSize_, const std::string &sClassName_ = "CDataSet", 
		const std::string &sClassVersion_ = "0.4",
    const std::string &sDerivedFrom_ = "CBase") throw();
  /// Constructor (2nd argument overloaded)
  CDataSet( const ushort usDimension_, const std::vector<size_t> extentVec_,
    const size_t dataDimensionSize_, const std::string &sClassName_ = "CDataSet", 
		const std::string &sClassVersion_ = "0.4",
    const std::string &sDerivedFrom_ = "CBase") throw();
  /// Constructor (for one-dimensional data sets)
  CDataSet( const size_t extent_, const size_t dataDimensionSize_,
  	const std::string &sClassName_ = "CDataSet",
		const std::string &sClassVersion_ = "0.4",
    const std::string &sDerivedFrom_ = "CBase") throw();   
  /// Copy Constructor
  CDataSet( const CDataSet& aDataSet )
    throw();
  /// Destructor
  virtual ~CDataSet()
    throw();
/* Accessors */
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
/* Mutators */
	void setBaseElementDimension( const ushort usIndex, const double dValue )
		throw( OutOfRangeException );
	void setBaseElementDimensions( const std::vector<double> dimensionsVec )
		throw( OutOfRangeException );	
/* Operators (Assignment) */
  /// Assignment operator for datasets    
  CDataSet& operator=( CDataSet& aDataSet )
    throw();
/* Other Methods */
  /// Reimplemented from CBase
  virtual const std::string dump() const
    throw();
protected:
  ushort usDimension;            ///< Dimension of the data set
  std::vector<size_t> extentVec; ///< Size of each dimension
  std::vector<double> baseElementDimensionsVec; ///< Size of one base element
  size_t dataDimensionSize;      ///< Dimension of each field entry
};

} // end of namespace aips
#endif
