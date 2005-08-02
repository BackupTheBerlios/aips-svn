/***********************************************************************
 * File: clog.h                                                        *
 * Project: AIPS                                                       *
 * Description: Logfile and error printing functions                   *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.11                                                       *
 * Status : Beta                                                       *
 * Created: 2003-09-15                                                 *
 * Based on: SDL++ CLog.h                                              *
 * Changed: 2003-09-16 Corrected error in logfile implementation which *
 *                     made it impossible to write logs to disk        *
 *          2003-09-18 Added dump() method to CLog                     *
 *          2003-09-22 Added makro SERROR(s) as an alias for           *
 *                     stringError( ..., s )                           *
 *                     Deleted useless class logstream                 *
 *          2003-09-30 Reorganized file structure, moved documentation *
 *                     to clog.cpp                                     *
 *          2003-10-17 Added debugging macros DBGx, FBEGIN and FEND    *
 *          2003-11-09 Corrected coding scheme.                        *
 *                     Moved code into namespace aips                  *
 *          2004-01-20 Made the source code look prettier              *
 *                     Renamed CLog to CLogServer and made it a        *
 *                      singleton                                      *
 *          2004-01-21 Moved errorString() to aipserror.h              *
 *                     Moved macros to aipsmacros.h                    *
 *          2004-04-22 Removed unneccessary includes                   *
 *          2005-03-15 Added Subject-Pattern for in-situ logging       *
 ***********************************************************************/
#ifndef CLOG_H
#define CLOG_H

// Standard library includes
#include <fstream>  // std::ostream
#include <iostream> // std::clog, std::cerr
#include <sstream>
#include <ctime>    // time(), ctime()

#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// AIPS includes
#include "cexception.h"
#include "aipsmacros.h"
#include "csubject.h"

/**
 * \file clog.h This file contains global definitions for log levels and a
 * logging class. An overloaded ostream operator for logging a string
 * with a defined log level and a timestamp is also provided.
 */

namespace aips {
 
/// Enumerate type for Log levels 
enum ELogLevel { LINFO, LWARN, LERR, LFATAL, LDEBUG, LFIXME };

extern ELogLevel logState;

std::ostream& operator<<( std::ostream& ostr, const ELogLevel theLogLevel );

/// The logging output stream
extern std::ostream alog;

/// Subject that generates logging events
extern CSubject newLog;

/// Streambuf to catch logging operations
class cloneStreamBuf : public std::streambuf
{
public:
	/// Constructor
	cloneStreamBuf( std::streambuf* assocbuf ) : assoc( assocbuf ) {}
protected:	
	/// Reimplemented from std::streambuf
	virtual std::streamsize xsputn ( const char * s, std::streamsize n );
	/// Reimplemented from std::streambuf
	virtual int overflow( int c );
	/// Reimplemented from std::streambuf
	virtual std::streampos seekoff ( std::streamoff off, std::ios_base::seekdir way,
		std::ios_base::openmode which );
	/// Reimplemented from std::streambuf
	virtual std::streampos seekpos ( std::streampos sp, std::ios_base::openmode which );
	/// Reimplemented from std::streambuf
	virtual std::streambuf* setbuf ( char * s, std::streamsize n );
	/// Reimplemented from std::streambuf
	virtual int sync();
private:
	/// Streambuffer associated with this clone buffer
	std:: streambuf* assoc;
};


/**
 * Central log file handling. Create one instance on application start with
 * aips::initLogServer()
 * Implemented as a singleton pattern.
 */
class CLogServer : public CBase
{
private:
/* Structors */
  /// Constructor
  CLogServer()
    throw();
  /// Constructor
  CLogServer( const std::string& sLogFilename, const bool bAppend )
    throw( BadInitException );
public:
  /// Destructor 
  ~CLogServer()
    throw();
/* Other methods */
  /// Reimplemented from CBase
  virtual const std::string dump() const
    throw();
/* Friends */
  friend void initLogServer( const std::string& sLogFilename, const bool bAppend )
    throw();
private:
  static std::ofstream* logFilePtr;  ///< Pointer to logging stream
};

/// Method to initialize the logging server
void initLogServer( const std::string& sLogFilename = "screen", const bool bAppend = true )
  throw();

}
#endif
