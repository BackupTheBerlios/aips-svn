/************************************************************************
 * File: ccnthandler                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz <hendrik@darkon.info>                                           *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Pre-Alpha                                                   *
 * Created: Di Mai 17 2005                                                      *
 * Based on:                                                            *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CCNTHANDLER_H
#define CCNTHANDLER_H

#include <iostream.h>
#include <cfilehandler.h>

using namespace aips;

/**
 * Handler for the cnt contour data format
 */  
class CCntHandler : public CFileHandler
{
private:
  /// Copy constructor
  CCntHandler( CCntHandler& );
  /// Assignment operator
  CCntHandler& operator= ( CCntHandler& );
public: 
/* Structors */
  /// Standard constructor
  CCntHandler()
    throw();
  /// Destructor
  virtual ~CCntHandler()
    throw();
/* Other methods */
  /// Loads a dataset from the given file. 
  virtual TDataFile load( const std::string& sFilename ) const
    throw( FileException );
  /// Saves the dataset to the given file. Pure virtual.
  virtual void save( const std::string& sFilename, const TDataFile& theData ) const
    throw( FileException );
private:    
	/// Load the data from a file. Pure virtual
  virtual void loadData( TDataSetPtr theTargetDataAPtr, std::istream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) const throw( FileException, NullException );    
  /// Save the data to a file. Pure virtual
  virtual void saveData( TDataSetPtr theSourceDataAPtr, std::ostream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) 
		const throw( FileException, NullException );
};
#endif
