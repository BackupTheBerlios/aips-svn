/***********************************************************************
 * File: clog.h                                                        *
 * Project: AIPS                                                       *
 * Description: Logfile and error printing functions                   *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.6                                                        *
 * Status : Beta                                                       *
 * Created: 15.09.03                                                   *
 * Based on: SDL++ CLog.h                                              *
 * Changed: 16.09.03 Corrected error in logfile implementation which   *
 *                   made it impossible to write logs to disk          *
 *          18.09.03 Added dump() method to CLog                       *
 *          22.09.03 Added makro SERROR(s) as an alias for             *
 *                   stringError( ..., s )                             *
 *                   Deleted useless class logstream                   *
 *          30.09.03 Reorganized file structure, moved documentation   *
 *                   to clog.cpp                                       *
 *          17.10.03 Added debugging macros DBGx, FBEGIN and FEND      *
 *          09.11.03 Corrected coding scheme.                          *
 *                   Moved code into namespace aips                    *
 *          20.01.04 Made the source code look prettier                *
 *                   Renamed CLog to CLogServer and made it a singleton*
 *          21.01.04 Moved errorString() to aipserror.h                *
 *                   Moved macros to aipsmacros.h                      *
 *          22.04.04 Removed unneccessary includes                     *
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

extern CSubject newLog;

class cloneStreamBuf : public std::streambuf
{
public:
	cloneStreamBuf( std::streambuf* assocbuf ) : assoc( assocbuf ) {}
protected:	
	virtual std::streamsize xsputn ( const char * s, std::streamsize n );
	virtual int overflow( int c );
	virtual std::streampos seekoff ( std::streamoff off, std::ios_base::seekdir way,
		std::ios_base::openmode which );
	virtual std::streampos seekpos ( std::streampos sp, std::ios_base::openmode which );
	virtual std::streambuf* setbuf ( char * s, std::streamsize n );
	virtual int sync();
private:
	std:: streambuf* assoc;
};


/**
 * Central log file handling. Create one instance on application start with
 * aips::initLogServer()
 * Implemented as a singleton pattern.
 */
#ifdef HAVE_SIGPP
class CLogServer : public CBase, public SigC::Object
#else
class CLogServer : public CBase
#endif
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
