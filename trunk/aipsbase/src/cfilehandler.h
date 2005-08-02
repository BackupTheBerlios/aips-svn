/************************************************************************
 * File: cfilehandler.h                                                 *
 * Project: AIPS                                                        *
 * Description: A file handler base class                               *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.12                                                        *
 * Status:  Beta                                                        * 
 * Created: 2003-10-24                                                  *
 * Changed:                                                             *
 *        2003-11-09 Code is now conform with coding scheme             *
 *                   Moved code into namespace aips                     *
 *        2003-12.16 Added support for header files                     *
 *        2004-01-08 Added abbreviation type TDataFile                  *
 *        2004-01-21 Added createProgressBar()                          *
 *                   Added private copy constructor and operator=       *
 *        2004-01-22 Added abstract methods loadData() and saveData()   *
 *				2004-04-26 Corrected an error in supports() method:           *
 *                    File extensions are now extracted correctly from  *
 *                    the given filename.                               *
 *        2004-04-27 Updated documentation.                             *
 *                   Removed method createProgressBar()                 *
 *        2004-11-22 Added enum type for data type identification       *
 *                   Provided class information constructor             *
 *        2004-11-23 Class now uses boost::shared_ptr                   *
 *        2004-11-25 Updated getDataType() to accept more variations    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CFILEHANDLER_H
#define CFILEHANDLER_H

// Standard includes
#include <utility> // std::pair

// AIPS includes
#include "cimageheader.h"
#include "ctypeddata.h"
#include "aipsnumeric.h"

namespace aips {

/** Abbreviation type for std::pair<TDataSetPtr, boost::shared_ptr<CImageHeader> > */
typedef std::pair<TDataSetPtr, boost::shared_ptr<CImageHeader> > TDataFile;
      
/**
 * A generic base class for all file handlers.
 * File handlers cannot be assigned or copy-constructed.
 */
class CFileHandler : public CBase
{
private:
  /// Copy Constructor
  CFileHandler( CFileHandler& );
  /// Assignment operator
  // CFileHandler operator= ( CFileHandler& ); //icc error: function returning abstract class is not allowed
public:
/* Structors */
  /// Standard constructor
  CFileHandler( const std::string &sClassName_, const std::string &sClassVersion_ = "0.4.1",
    const std::string &sDerivedFrom_ = "CFileHandler" )
    throw();
  /// Destructor
  virtual ~CFileHandler()
    throw();
/* Other methods */
  /**
   * Loads a dataset from the given file. Pure virtual.
   * \param sFilename Filename of file to load
   * \returns Pair of data and header information
   */
  virtual TDataFile load( const std::string& sFilename ) const
    throw( FileException ) = 0;
  /**
   * Saves the dataset to the given file. Pure virtual.
   * \param sFilename Filename of file to save to.
   * \param theData Pair of data and (in most cases optional) header information
   */
  virtual void save( const std::string& sFilename, const TDataFile& theData ) const
    throw( FileException ) = 0;
  /// Returns true if the filehandler supports the format of the given extension
  bool supports( const std::string& sFilename ) const
    throw();
  /// Returns a string containing a file mask for all supported file types
  const std::string legalFileTypes() const
    throw();
  /// Reimplemented from CBase
  virtual const std::string dump() const
    throw();  
protected:
/** \enum EDataType Voxel datatype enumeration */
	enum EDataType { DUInt8 = 1, DUInt16 = 2, DUInt32 = 4, DInt8 = 11, DInt16 = 12 , DInt32 = 14, 
		DFloat16 = 20+sizeof(float), DFloat32 = 20+sizeof(double), DFloat64 = 20+sizeof(long double) };	
/* Other methods */
	/// Converts a header map entry into an EDataType
	EDataType getDataType( const std::string& sHeaderEntry ) const 
		throw (NotPresentException);
  /// Load the data from a file. Pure virtual
  virtual void loadData( TDataSetPtr theTargetDataSPtr, std::istream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) const throw( FileException, NullException ) =0;    
  /// Save the data to a file. Pure virtual
  virtual void saveData( TDataSetPtr theSourceDataSPtr, std::ostream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) 
		const throw( FileException, NullException ) =0;
/* Member variables */  
  std::vector<std::string> supportedFileTypesVec; ///< Extensions of all supported file types
};

}
#endif
