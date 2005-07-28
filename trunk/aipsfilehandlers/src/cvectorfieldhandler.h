/************************************************************************
 * File: cvectorfieldhandler                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz <hendrik@darkon.info>                                           *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Pre-Alpha                                                   *
 * Created: Do Jun 2 2005                                                      *
 * Based on:                                                            *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CVECTORFIELDHANDLER_H
#define CVECTORFIELDHANDLER_H

// AIPS includes
#include <cbinaryfilehandler.h>
#include "cvectorfieldheader.h"

using namespace aips;

/**
 * A handler for the simple data file format
 * <.dat> Files are build the following way:
 * 12 bytes Header, consisting of three 32-bit unsigned integer values:
 *   0..3   X-Dimension
 *   4..7   Y-Dimension
 *   8..11  Z-Dimension
 * Data in raw short format (usually 12 to 16 bits per voxel)
 */  
class CVectorFieldHandler : public CBinaryFileHandler
{
private:
  /// Copy constructor
  CVectorFieldHandler( CVectorFieldHandler& );
  /// Assignment operator
  CVectorFieldHandler& operator=( CVectorFieldHandler& );
public:
  /// Constructor
  CVectorFieldHandler()
    throw();
  /// Destructor
  virtual ~CVectorFieldHandler()
    throw();
  /// Loads a data set
  virtual TDataFile load( const std::string& sFilename )
    const throw( FileException );
  /// Saves a data set
  virtual void save( const std::string& sFilename, const TDataFile& theData )
    const throw( FileException );
};

#endif
