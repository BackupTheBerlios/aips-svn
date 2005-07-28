/************************************************************************
 * File: ccommonimagehandler.h                                          *
 * Project: AIPS                                                        *
 * Description: A file handler for most common 2D image formats         *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-03-22                                                  *
 * Changed: 2005-03-23 Added support for writing image sequences        *
 *                      Beware. Support of writing single frames for    *
 *                      formats that don't support multiframe writing   *
 *                      seems to be quite buggy. Better stick to gif    *
 *                      mng and mpg for writing sequences.              *
 * TODO: png support doesn't seem to work on SUSE 9.1                   *
 *       parameters for image sequences (delay, quality)                *
 *       support for movies and image sequences                         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CCOMMONIMAGEHANDLER_H
#define CCOMMONIMAGEHANDLER_H

#include <aipsfilehandlers_config.h>
#ifdef HAVE_IM

// Standard includes
#include <cstring>
#include <cctype>
#include <string>
#include <list>
#include <algorithm>

// AIPS includes
#include "cbinaryfilehandler.h"
#include "aipsnumeric.h"

// Image magick includes
#include <Magick++.h>

using namespace aips;

/**
 * A file handler for some common 2D image formats like JPG, BMP, PNG,...
 * Most work is done internally by the excellent image magick libraries
 * You can also use this to export your images directly to PS or EPS!
 */
class CCommonImageHandler : public CBinaryFileHandler
{
private:
  /// Copy Constructor
  CCommonImageHandler( CCommonImageHandler& );
  /// Assignment operator
  CCommonImageHandler& operator= ( CCommonImageHandler& );
public:
  /// Constructor
  CCommonImageHandler()
    throw();
  /// Destructor
  virtual ~CCommonImageHandler()
    throw();
  /// Loads an unsigned short scalar image dataset from the given file
  virtual TDataFile load( const std::string& sFilename ) const
    throw( FileException );
  /// Saves an unsigned short scalar image dataset to the given file
  virtual void save( const std::string& sFilename, const TDataFile& theData ) const
    throw( FileException );
private:
	void saveImageSequence( const std::string& sFilename, TImagePtr aDataSet ) const
		throw( FileException );
	TDataFile loadImageSequence( const std::string& sFilename ) const
		throw( FileException );
};
#endif
#endif
