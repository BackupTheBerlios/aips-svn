/************************************************************************
 * File: canalyzehandler.h                                              *
 * Project: AIPS                                                        *
 * Description: A handler for the ANALYZE 7.5 medical image format      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Alpha                                                       *
 * Created: 16.12.03                                                    *
 * Changed: 07.01.04 Added support for big and little endian headers    *
 *          21.01.04 Simplified the code                                *
 *          22.01.04 Moved data load/save code to CBinaryFileHandler    *
 *          26.01.04 Added debugs and fixmes                            *
 *          24.04.04 Deleted unneccessary includes                      *
 *          04-05-11 Handler can now also save data                     *
 *                   (single-channel, 2D and 3D images)                 *
 *                   Handler now provides its own header class          *
 *          25-01-05 hist.orient field is now interpreted.              *
 *                   This feature has not been tested throrougly yet!   *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CANALYZEHANDLER_H
#define CANALYZEHANDLER_H

// AIPS includes
#include "gzstream.h"
#include "cbinaryfilehandler.h"
#include "canalyzeheader.h"

using namespace aips;

/**
 * A handler for the ANALYZE 7.5 medical image format. Some of the SPM99 extensions are also supported.
 * FIXME I currently only have access to a small amount of
 * ANALYZE files, so the current support is rather mutual and may even be faulty
 */  
class CAnalyzeHandler : public CBinaryFileHandler
{
private:
  /// Copy constructor
  CAnalyzeHandler( CAnalyzeHandler& );
  /// Assignment operator
  CAnalyzeHandler& operator= ( CAnalyzeHandler& );  
public: 
/* Structors */
  /// Standard constructor
  CAnalyzeHandler()
    throw();
  /// Destructor
  virtual ~CAnalyzeHandler()
    throw();
/* Other methods */
  /// Loads a dataset from the given file. 
  virtual TDataFile load( const std::string& sFilename ) const
    throw( FileException );
  /// Saves the dataset to the given file. Pure virtual.
  virtual void save( const std::string& sFilename, const TDataFile& theData ) const
    throw( FileException );
private:
	/// Determines the data type of the dataset
	EDataType determineDataType( CDataSet* theDataSet, CImageHeader* theHeader ) const;		
	/// Flips the given axes
	template<typename T> boost::shared_ptr<T> flip( boost::shared_ptr<T> original, 
		bool bSwapX, bool bSwapY, bool bSwapZ ) const;
	/// Rotates axes to new positions
	template<typename T> boost::shared_ptr<T> rotate( boost::shared_ptr<T> original, 
		ushort usNewX, ushort usNewY, ushort usNewZ ) const;
};

#endif
