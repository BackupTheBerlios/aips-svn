/************************************************************************
 * File: cdatafileserver.h                                              *
 * Project: AIPS                                                        *
 * Description: The file server handles all file io of AIPS             *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.5                                                         *
 * Status:  Beta                                                        *
 * Created: 24.10.03                                                    *
 * Changed: 24.10.03 Creation of class                                  *
 *          09.11.03 Moved code into namespace aips                     *
 *                   Moved inline members to cpp file                   *
 *          27.11.03 Removed unused implementations of private methods  *
 *          16.12.03 Load/save now also work with file header           *
 *                   information                                        *
 *                   Added error handling to load and save methods      *
 *                   saveDataSet now also throws NullException if the   *
 *                   given data set is a NULL pointer                   *
 *          08.01.04 Iteration loops now use ++it instead of it++       *
 *          20.01.04 Made the source code look prettier                 *
 *          26.04.04 Added method removeHandler()                       *
 *        2004-11-23 Now uses boost::shared_ptr to store file handlers  *
 *        2005-05-17 Method getNumberOfRegisteredHandlers() added       *
 *                   Method getHandler() added                          *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CDATAFILESERVER_H
#define CDATAFILESERVER_H

// AIPS includes
#include "cfilehandler.h"

namespace aips {

/**
 * A generic file handler. This class is implemented as a Meyers-Singleton
 * so you only have one instance per application. Simply add childs of
 * CFileHandler at application startup to gain full functionality.
 *
 * The correct file handler to use for a specific file will be determined by
 * the file extension, so make sure all formats you are using have different
 * extensions. File loading is more robust in respect to this. If a file
 * handler produces an exception on reading a file, the file server tries to
 * use another handler. This mechanism does not work for file saving!
 */
class CDataFileServer : public CBase
{
private:
  /// Standard constructor
  CDataFileServer()
    throw();
  /// Copy constructor
  CDataFileServer( const CDataFileServer& )
    throw(); 
  /// Operator=
  CDataFileServer& operator=( const CDataFileServer& )
    throw();
public:
/* Structors */
  /// Destructor
  virtual ~CDataFileServer()
    throw();
/* Accessors */
	/// Returns the given handler
	boost::shared_ptr<CFileHandler> getHandler( uint uiIndex_ )
		throw( OutOfRangeException );
/* Other Methods */
  /// Adds a new fileHandler to the vector of available file handlers
  void addHandler( boost::shared_ptr<CFileHandler> handlerPtr )
    throw( NullException );
	/// Remove the given handler
	void removeHandler( boost::shared_ptr<CFileHandler> handlerPtr )
		throw( NullException );
	/// Returns the number of registered file handlers
	uint getNumberOfRegisteredHandlers()
		throw();
  /// Loads a data set. The correct file handler will be determined automatically
  TDataFile loadDataSet( const std::string& sFilename ) const
    throw( FileException );
  /// Saves a data set. The correct file handler will be determined automatically
  void saveDataSet( const std::string& sFilename, const TDataFile& theData ) const
    throw( FileException, NullException );
  /// Returns a string containing a file mask for all supported file types
  const std::string supportedFileTypes() const
    throw();
  /// Reimplemented from CBase
  virtual const std::string dump() const
    throw();

/* Friend functions */  
  /// Returns a reference to the file server
  friend aips::CDataFileServer& getFileServer()
    throw();
private:
	/// Vector of all registered file handlers 
	static std::vector<boost::shared_ptr<CFileHandler> > fileHandlerPtrVec; 
};

/// Function that returns a reference to the file server
CDataFileServer& getFileServer() throw();

}
#endif
