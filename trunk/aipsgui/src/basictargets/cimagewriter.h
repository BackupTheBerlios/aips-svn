/************************************************************************
 * File: cimagewriter.h                                                 *
 * Project: AIPS - Basic targets plugin library                         *                 
 * Description: Writes data volumes in a format specified by the        *
 *              file extension                                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 2003-12-15                                                  *
 * Changed: 2004-01-28 Renamed class to CImageWriter                    *
 *                     Clarified and documented the source code         *
 *          2004-02-06 Removed uneccassary constructor param sName      *
 *          2004-04-21 Writer now also supports image mirroring and     *
 *                      endianess swapping                              *
 *          2004-06-16 Updated documentation and error output           *
 *          2005-01-13 Templatized saving method to enable saving       *
 *                     of all dataset types                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CSIMPLEDATWRITER_H
#define CSIMPLEDATWRITER_H

// AIPS includes
#include <ctarget.h>
#include <cdatafileserver.h>
#include "cimagewriterdialog.h"

// lib includes
#include "libid.h"

using namespace aips;
using namespace std;



/**
 * Writes data volumes in a format specified by the file extension
 */
class CImageWriter : public CTarget, CObserver
{
private:
  /// Standard constructor
  CImageWriter();
  /// Copy constructor
  CImageWriter( CImageWriter& );
  /// Assignment operator
  CImageWriter& operator=( CImageWriter& );
public:
  /// Constructor
  CImageWriter( ulong ulID )
    throw();
  /// Destructor
  ~CImageWriter()
    throw();
  /// Reimplemented from CTarget. This is a dummy function and will not be used
  virtual void apply()
    throw();
  /// Reimplemented from CTarget
  virtual const std::string dump() const
    throw();
	/// Reimplemented from CTarget
  CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
public:	
  /// Opens a file dialog to allow selection of new source data
  void selectNewFile( string sFilename );
  /// Loads the new dataset
  void updateData();
	virtual void execute( boost::shared_ptr<CEvent> anEvent );
private:
	boost::shared_ptr<CImageWriterDialog> myDialog;		
  std::string sFilename;          ///< Chosen filename
private:
	template<typename T> void actualSaver() throw();
	TDataSetPtr myInput;
};

#endif
