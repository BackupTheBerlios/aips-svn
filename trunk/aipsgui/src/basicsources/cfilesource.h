/************************************************************************
 * File: cfilesource.h                                                  *
 * Project: AIPS basic sources plugin library                           *
 * Description: A file loading dialog module                            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status : Beta                                                        *
 * Created: 29.10.03                                                    *
 * Changed: 28.01.04 Moved inline members to cpp file                   *
 *                   Documented and clarified the code                  *
 *                   Merged convertImage and convertVolume              *
 *          06.02.04 Deleted uneccessary consrtuctor parameter sCaption *
 *          27.02.04 Inserted global progress bar support and           *
 *                   benchmarking                                       *
 *          05.05.04 Updated and documented source                      *
 *                   Filename and path are now stored in module params  *
 *          04-05-11 Removed mirror and endianess swapping              *
 *                   (There're now seperate modules for this)           *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CFILESOURCE_H
#define CFILESOURCE_H

// AIPS includes
#include <csource.h>
#include <cdatafileserver.h>
#include <cfilesourcedialog.h>
#include <cmoduledialog.h>

using namespace aips;
using namespace std;
using namespace boost;

class CFileSource : public CSource, CObserver
{
private:
  /// Standard constructor
  CFileSource();
  /// Copy constructor
  CFileSource( CFileSource& );
  /// Assignment operator
  CFileSource& operator=( CFileSource& );
public:
/* Structors */
  /// Constructor
  CFileSource( ulong ulID )
    throw();
  /// Destructor
  virtual ~CFileSource()
    throw();
/* Other methods */
	/// Reimplemented from CPipelineItem
  CPipelineItem* newInstance( ulong ulID = 0 ) const 
		throw();
 	virtual void apply() throw();
public:	
  /// Opens a file dialog to allow selection of new source data
  void selectNewFile( string sFilename );
  /// Loads the new dataset
  void updateData();
	virtual void execute( boost::shared_ptr<CEvent> anEvent );
private:
	boost::shared_ptr<CFileSourceDialog> myFileSourceDialog;
};

#endif
