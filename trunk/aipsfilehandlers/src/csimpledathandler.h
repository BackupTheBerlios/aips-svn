/************************************************************************
 * File: csimpledathandler.h                                            *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: A handler for the simple dat format                     *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status: Beta                                                         *
 * Created: 11.12.03                                                    *
 * Changed: 15.12.03 Added save functionality                           *
 *          22.01.04 Simplified code                                    *
 *                   Moved load/save code to CBinaryFileHandler         *
 *          27.01.04 Corrected wrong endianess on loading               *
 *          24.04.04 Deleted unneccessary includes                      *
 *                   Added writing of big and little endianess files    * 
 *                   Automatic detection of big and little endian files *
 *        2004-11-25 Automatic detection of data type (ubyte or ushort) *
 *                   Automatic writing of best data type                *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CSIMPLEDATHANDLER_H
#define CSIMPLEDATHANDLER_H

// AIPS includes
#include <cbinaryfilehandler.h>
#include "csimpledatheader.h"

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
class CSimpleDatHandler : public CBinaryFileHandler
{
private:
  /// Copy constructor
  CSimpleDatHandler( CSimpleDatHandler& );
  /// Assignment operator
  CSimpleDatHandler& operator=( CSimpleDatHandler& );
public:
  /// Constructor
  CSimpleDatHandler()
    throw();
  /// Destructor
  virtual ~CSimpleDatHandler()
    throw();
  /// Loads a data set
  virtual TDataFile load( const std::string& sFilename )
    const throw( FileException );
  /// Saves a data set
  virtual void save( const std::string& sFilename, const TDataFile& theData )
    const throw( FileException );
};

#endif
