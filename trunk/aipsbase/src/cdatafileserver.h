/************************************************************************
 * File: cdatafileserver.h                                              *
 * Project: AIPS                                                        *
 * Description: The file server handles all file io of AIPS             *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.10                                                        *
 * Status:  Beta                                                        *
 * Created: 2003-10-23                                                  *
 * Changed:                                                             *
 *        2003-03-24 Creation of class                                  *
 *        2003-11-09 Moved code into namespace aips                     *
 *                   Moved inline members to cpp file                   *
 *        2003-11-21 Removed unused implementations of private methods  *
 *        2003-12-16 Load/save now also work with file header           *
 *                   information                                        *
 *                   Added error handling to load and save methods      *
 *                   saveDataSet now also throws NullException if the   *
 *                   given data set is a NULL pointer                   *
 *        2004-01-08 Iteration loops now use ++it instead of it++       *
 *        2004-01-20 Made the source code look prettier                 *
 *        2004-04-26 Added method removeHandler()                       *
 *        2004-11-23 Now uses boost::shared_ptr to store file handlers  *
 *        2005-05-17 Method getNumberOfRegisteredHandlers() added       *
 *                   Method getHandler() added                          *
 *        2005-08-01 Updated documentation                              *
 *                   Minor code improvements                            *
 *        2005-11-21 Updated documentation                              *
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
 * \brief A generic file handler.
 *
 * This class is implemented as a Meyers-Singleton
 * so you only have one instance per application. Simply add childs of
 * CFileHandler at application startup to gain full functionality.
 *
 * The correct file handler to use for a specific file will be determined by
 * the file extension. If a extension matches for several handlers,
 * all of these handlers will try to load the file until one of it succeeds.
 * File loading is more robust in respect to this. If a file
 * handler produces an exception on reading a file, the file server tries to
 * use another handler. This mechanism does not work for file saving!
 *
 * \todo Write test cases and example code
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
/** \name Structors */
  //@{
  /// Destructor
  virtual ~CDataFileServer()
    throw();
  //@}
/** \name Accessors */
  //@{
	/// Returns the given handler
	boost::shared_ptr<CFileHandler> getHandler( uint uiIndex_ )
		throw( OutOfRangeException );
  //@}  
/** \name Other Methods */
  //@{
  /// Adds a new fileHandler to the vector of available file handlers
  void addHandler( boost::shared_ptr<CFileHandler> handlerSPtr )
    throw( NullException );
	/// Remove the given handler
	void removeHandler( boost::shared_ptr<CFileHandler> handlerSPtr )
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
  //@}  
/** \name Friend functions */
  //@{
  /// Returns a reference to the file server
  friend aips::CDataFileServer& getFileServer()
    throw();
  //@}
private:
	/// Vector of all registered file handlers 
	static std::vector<boost::shared_ptr<CFileHandler> > fileHandlerSPtrVec; 
};

/// Function that returns a reference to the file server
CDataFileServer& getFileServer() throw();

}
#endif
