/************************************************************************
 * File: cbinaryfilehandler.h                                           *
 * Project: AIPS                                                        *
 * Description: A class to handle binary file types                     *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.6                                                         *
 * Status : Beta                                                        *
 * Created: 22.01.04                                                    *
 * Changed: 26.01.04 saveData now throws an exception if given          *
 *                   voxel size is greater than 2                       *
 *          27.01.04 Corrected wrong endianess on saving and loading    *
 *          24.04.04 Deleted unneccessary includes                      *
 *                   Added writing of big and little endianess files    *
 *          27.04.04 Updated docs and corrected minor errors            *
 *        2004-11-22 load and save now handle multiple data types       *
 *        2004-11-24 Class now uses boost::shared_ptr                   *
 *        2004-11-25 Corrected some errors in load() and save()         *
 *        2005-01-26 Added function templates for simplified loading    *
 *                    and saving of different data types                *
 *        2005-04-04 Updated documentation and nomenclature             *
 * TODO: Better vector field handling                                   *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CBINARYFILEHANDLER_H
#define CBINARYFILEHANDLER_H

// AIPS includes
#include "aipsdatatraits.h"
#include "cfilehandler.h"

namespace aips {

/**
 * A class to handle binary file types. This should be a parent class of
 * all file handlers which don't rely on external libraries to load binary
 * image data
 */
class CBinaryFileHandler : public CFileHandler
{
private:
	/// Copy constructor
	CBinaryFileHandler( CBinaryFileHandler& );
	/// Assignment operator
	//CBinaryFileHandler operator=( CBinaryFileHandler& );//icc error: function returning abstract class is not allowed
public:
/* Structors */
  /// Constructor
  CBinaryFileHandler( const std::string &sClassName_ = "CBinaryFileHandler", 
		const std::string &sClassVersion_ = "0.6",
    const std::string &sDerivedFrom_ = "CFileHandler" ) 
		throw();
  /// Destructor
  virtual ~CBinaryFileHandler() 
		throw();
protected:
/* Other methods */
  /// Load the data from a file
  virtual void loadData( TDataSetPtr theTargetDataAPtr, std::istream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) const 
		throw( FileException, NullException );
  /// Save the data to a file
  virtual void saveData( TDataSetPtr theSourceDataAPtr, std::ostream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess )	const 
		throw( FileException, NullException ); 
private:
/* Other methods */
	/// Internal member function template to actually load data of a specific type
	template<typename SetType, typename DataType>
	void loadSpecificType( boost::shared_ptr<SetType> theTargetDataAPtr, std::istream& theFile,
		const bool bFileEndianess, const size_t theVoxelSize ) const
		throw( FileException );
	/// Internal member function template to actually save data of a specific type
	template<typename SetType, typename DataType>
	void saveSpecificType( boost::shared_ptr<SetType> theSourceDataAPtr, std::ostream& theFile,
		const bool bFileEndianess, const size_t theVoxelSize ) const
		throw( FileException );
	void loadVectorField( TField3DPtr theTargetDataAPtr, std::istream& theFile,
		const bool bFileEndianess ) const throw( FileException );
	void saveVectorField( TField3DPtr theSourceDataAPtr, std::ostream& theFile,
		const bool bFileEndianess ) const throw( FileException );
};

}
#endif
