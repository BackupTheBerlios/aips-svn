/************************************************************************
 * File: cdatafileserver.cpp                                            *
 * Project: AIPS                                                        *
 * Description: A generic file handler.                                 *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 24.10.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdatafileserver.h"

using namespace std;
using namespace aips;

// Declaration of static data member
vector<boost::shared_ptr<CFileHandler> > CDataFileServer::fileHandlerSPtrVec;

// Covenience typedef
typedef vector<boost::shared_ptr<CFileHandler> > THandlerVec;

/*************
 * Structors *
 *************/

/**
 * Private default constructor.
 * Only usable by the friend function.
 */
CDataFileServer::CDataFileServer() throw()
  : CBase( "CDataFileServer", "0.5", "CBase" )
{
}

/**
 * Deletes all registered file handlers.
 */
CDataFileServer::~CDataFileServer() throw()
{
	fileHandlerSPtrVec.clear();
}

/*****************
 * Other methods *
 *****************/
 
/**
 * \param handlerPtr A new file handler.
 * \attention This instance given to this member function will be deleted automatically at program
 * termination so DO NOT try to do this by hand or to use non-dynamic objects here!
 * \exception NullException if aHandler is NULL
 */
void CDataFileServer::addHandler( boost::shared_ptr<CFileHandler> handlerSPtr ) throw( NullException )
{
  if ( !handlerSPtr )
    throw( NullException( SERROR("Pointer to file handler is NULL"), CException::RECOVER, ERR_CALLERNULL ) );
  fileHandlerSPtrVec.push_back( handlerSPtr );
}

/**
 * \param handlerPtr A new file handler. This instance will be deleted automatically.
 * \exception NullException if aHandler is NULL or handler wasn't found
 */
void CDataFileServer::removeHandler( boost::shared_ptr<CFileHandler> handlerSPtr ) throw( NullException )
{
  if ( !handlerSPtr )
    throw( NullException( SERROR("Pointer to file handler is NULL"), CException::RECOVER, ERR_CALLERNULL ) );
		
	THandlerVec::iterator it = find( fileHandlerSPtrVec.begin(), fileHandlerSPtrVec.end(), handlerSPtr );
	
	if ( it == fileHandlerSPtrVec.end() )
		throw( NullException( SERROR("Handler not found"), CException::RECOVER, ERR_REQUESTNULL ) );
		
	fileHandlerSPtrVec.erase( it );
}

/**
 * \param sFilename Data file name. This MUST have an file extension.
 * \returns A pair of the loaded data set and the corresponding header.
 * \exception FileException on load error, illegal filename or not supported file type.
 */
TDataFile CDataFileServer::loadDataSet( const string& sFilename )
  const throw( FileException )
{
  // Check filename for an extension
  if ( sFilename.find( ".", 1 ) == string::npos )
    throw ( FileException( SERROR( "No file extension specified" ), CException::RECOVER, ERR_ILLEGALFILENAME ) );

  bool bSuccess = false;
  // This exception will be thrown if no appropiate file handler was found
  FileException anException( SERROR("No appropiate file handler found"), CException::RECOVER );
  
  TDataFile theData;
  theData.first.reset(); theData.second.reset();

  // Check all registered handlers if file type is supported and load the data
  for ( THandlerVec::const_iterator it = fileHandlerSPtrVec.begin(); 
  	it != fileHandlerSPtrVec.end() && !bSuccess; ++it )
  {
    if( (*it)->supports( sFilename ) )
    {
      bSuccess = true;
      try
      {
        theData = (*it)->load( sFilename );        
      }
      catch ( FileException &e )
      {
        alog << LERR << e.what() << endl;
        bSuccess = false;
        // Correct handler found, but a file error occured
        // If we do not find another handler, this error will be thrown again (s.d.)
        anException = e;
      }
    }
  }

  // If we were not succesful, throw an exception
  if ( !bSuccess ) 
  	throw( anException );
		
  return theData;
}

/**
 * \param sFilename Data file name. This MUST have an file extension.
 * \param theData Pair of a data set and a header information class.
 * If the pointer to the header is NULL, default values will be used.
 * \returns The loaded data set.
 * \exception FileException on file error, illegal filename or not supported file type.
 */
void CDataFileServer::saveDataSet( const string& sFilename, const TDataFile& theData )
  const throw( FileException, NullException )
{
  if ( !theData.first )
    throw( NullException( SERROR( "Given dataset is NULL" ), CException::RECOVER, ERR_CALLERNULL ) );
  
  // Check filename for an extension
  if ( sFilename.find( ".", 1 ) == string::npos )
  {
    throw ( FileException( SERROR( "No file extension specified" ),
      CException::RECOVER, ERR_ILLEGALFILENAME ) );
  }

  bool bSuccess = false;
  // This exception will be thrown if no appropiate file handler was found
  FileException anException( SERROR("No appropiate file handler found"), CException::RECOVER );

  // Search for the correct file handler and write data to disk  
  for ( THandlerVec::const_iterator it = fileHandlerSPtrVec.begin(); 
  	it != fileHandlerSPtrVec.end() && !bSuccess; ++it )
  {
    if( (*it)->supports( sFilename ) )
    {
      bSuccess = true;
      try
      {
        (*it)->save( sFilename, theData );
      }
      catch ( FileException &e )
      {
        alog << LERR << e.what() << endl;
        bSuccess = false;
        // Correct handler found, but a file error occured
        // If we do not find another handler, this error will be thrown again (s.d.)
        anException = e; 
      }
    }
  }

  // If we were not succesful, throw an exception
  if ( !bSuccess ) 
  	throw( anException );
}

/**
 * \return file mask
 */
const string CDataFileServer::supportedFileTypes() const throw()
{
  std::ostringstream os;
  for ( THandlerVec::const_iterator it = fileHandlerSPtrVec.begin(); it != fileHandlerSPtrVec.end(); ++it )
    os << (*it)->legalFileTypes();
  return os.str();
}

const string CDataFileServer::dump() const throw()
{
  std::ostringstream os;
  os << "\ntheFileHandlers size: " << fileHandlerSPtrVec.size() << " ";
  os << "\nsupportedFileType: " << supportedFileTypes() << " ";
  return CBase::dump() + os.str();
}

/**
 * \return a reference to the file server
 */
CDataFileServer& aips::getFileServer() throw()
{
  static aips::CDataFileServer theServer;
  return theServer;
}

/**
 * \param uiIndex index of requested handler
 * \returns the handler adressed by uiIndex_
 * \throws OutOfRangeException if uiIndex_ is too large
 */
boost::shared_ptr<CFileHandler> CDataFileServer::getHandler( uint uiIndex_ )
		throw( OutOfRangeException )
{
	if ( uiIndex_ > getNumberOfRegisteredHandlers() )
		throw( OutOfRangeException( SERROR("Given index is too large"), CException::RECOVER ) );
	return fileHandlerSPtrVec[uiIndex_];
}
	
/** \return the number of registered file handlers */
uint CDataFileServer::getNumberOfRegisteredHandlers() throw()
{
	return fileHandlerSPtrVec.size();
}
