/***********************************************************************
 * File: cdatahandler.h                                                *
 * Project: AIPS                                                       *
 * Description: A file handler for the ".data" volume format           *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.4                                                        *
 * Status : Beta                                                       *
 * Created: 24.10.03                                                   *
 * Changed: 27.10.03 Added documentation and exception handling        *
 *          17.12.03 Added save functionality                          *
 *          20.01.04 Made the source code look prettier                *
 *                   Images are no longer restircted to XDim 1024      *
 *                   Removed unneccesary code                          *
 *          21.01.04 Simplified code                                   *
 *          22.01.04 Moved load/save code to CBinaryFileHandler        *
 *          22.04.04 Detection of file endianess (header format has    *
 *                    been altered to include endianess information.   *
 *                    Old headers are not outdated by this (s.d.)!     *
 *          27.04.04 Added the new CDataHeader                         *
 *          23.12.04 Added support for gzip data compression           *
 ***********************************************************************/

#ifndef CDATAHANDLER_H
#define CDATAHANDLER_H

// AIPS includes
#include <cbinaryfilehandler.h>
#include <aipsnumeric.h>
#include <gzstream.h>
#include "cdataheader.h"

using namespace aips;
/**
 * A file handler for the ".data" volume format (raw byte, raw short)
 * The data format can be described the following way:
 *  The header file (.data): This is an ascii file with four lines
 *   0: Size of one voxel (must be 1 for rbyte or 2 for rshort data)
 *   1: X-Dimension 
 *   2: Y-Dimension
 *   3: Z-Dimension
 *   4: Endianess ( 0 = little, 1 = big ). This line is optional. If it's ommited, little endian
 *      data will be assumed
 *  The data file (.rawb, .raws): This is an binary file. All data is
 * written serially into the file. X is the fastest varying dimension.
 */
class CDataHandler : public CBinaryFileHandler
{
private:
  /// Copy constructor
  CDataHandler( CDataHandler& );
  /// Assignment operator
  CDataHandler& operator= ( CDataHandler& );
public:
/* Structors */
  /// Constructor
  CDataHandler() throw();
  /// Destructor
  virtual ~CDataHandler() throw();
/* Other methods */  
  /// Loads a data set
  virtual TDataFile load( const std::string& sFilename )
    const throw( FileException );
  /// Saves a data set
  virtual void save( const std::string& sFilename, const TDataFile& theData )
    const throw( FileException );
};

#endif
