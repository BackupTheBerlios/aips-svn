/************************************************************************
 * File: cdf3handler.h                                                  *
 * Project: AIPS                                                        *
 * Description: Handler for the POVRay volume format df3                *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-06-03                                                  *
 * Changed: 2004-12-23 Updated and documented source code               *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef CDF3HANDLER_H
#define CDF3HANDLER_H

// AIPS includes
#include <cbinaryfilehandler.h>
#include "cdf3header.h"

using namespace aips;

/**
 * A handler for the df3 POVRAY density file format
 * <.df3> Files are build the following way:
 * 6 bytes Header, consisting of three 16-bit, little endian, unsigned integer values:
 *   0..1  X-Dimension
 *   2..3  Y-Dimension
 *   4..5  Z-Dimension
 * Data in raw byte format 
 */  
class CDF3Handler : public CBinaryFileHandler
{
private:
  /// Copy constructor
  CDF3Handler( CDF3Handler& );
  /// Assignment operator
  CDF3Handler& operator=( CDF3Handler& );
public:
  /// Constructor
  CDF3Handler()
    throw();
  /// Destructor
  virtual ~CDF3Handler()
    throw();
  /// Loads a data set
  virtual TDataFile load( const std::string& sFilename )
    const throw( FileException );
  /// Saves a data set
  virtual void save( const std::string& sFilename, const TDataFile& theData )
    const throw( FileException );
};

#endif
