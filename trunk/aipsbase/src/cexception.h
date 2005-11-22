/************************************************************************
 * File: cexception.h                                                   *
 * Project: AIPS                                                        *
 * Description: The AIPS run time error handling system                 *
 *              Exception types and classes                             *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.17                                                        *
 * Status : Beta                                                        *
 * Created: 2003-09-12                                                  *
 * Based on: SDL++ exception.h                                          *
 * Changed: 2003-09-13 Documentation, NotPresentException added         *
 *          2003-09-15 NullException added, Class moved to qtvisbase    *
 *          2003-09-16 Correction of default initialisations            *
 *                     Moved empty constructors/destructors to .h-file  *
 *          2003-09-18 Added dump method to CException                  *
 *                     Added new exception AllocException               *
 *          2003-09-23 Added documentation of CException members        *
 *                     CException::AddTypePrefix now included in        *
 *                     CException constructor                           *
 *          2003-09-24 Removed AllocException                           *
 *          2003-09-30 Reorganized file structure, moved documentation  *
 *                     to cexception.cpp                                *
 *                     Added error codes to the exception classes       *
 *          2003-10-17 Added even more error codes :-)                  *
 *          2003-11-09 Moved classes into namespace aips                *
 *                     Moved inline members back to cexception.cpp      *
 *          2003-11-13 Added new error codes                            *
 *          2004-01-21 Moved error codes to aipserror.h                 *
 *                     Added copy constructors to exceptions            *
 *          2004-04-22 Added more source documentation                  *
 *                     getErrorNumber() and getExceptionType() are no   *
 *                      longer virtual                                  *
 *                     Exception implementations moved to cpp file      *
 *                     All exception desctructors were made virtual     *
 *                     Removed unneccessary includes                    *
 *          2004-04-26 Added missing documentation of                   *
 *                     CException::operator=                            *
 *          2004-11-22 Removed setinfo. Updated constructor accordingly *
 *          2005-07-07 Added runtime type exception                     *
 *          2005-11-22 Updated documentation                            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CEXCEPTION_H
#define CEXCEPTION_H
#define CEXCEPTION_VERSION "0.17"

// Standard library includes
#include <exception> 
#include <stdexcept>
#include <iostream>

// System includes
#include <sys/types.h> 

// AIPS includes
#include "cbase.h"
#include "aipserror.h"

namespace aips {
  

/**
 * \brief Exception base class.
 *
 * Normally instances of this class should not be thrown but could be catched for easy
 * error handling (since all exceptions are derived from this class).
 * You could also catch std::exception since CException is a child of this class.
 *
 * \todo Write test cases and examples.
 */
class CException : public std::exception, public CBase
{
public:
	/**
 	 * TException Type.
 	 * Could be used in catch blocks to determine further error handling routines
 	 */
	enum TExceptionType { UNKNOWN, WARNING, RECOVER, FATAL };
/** \name Structors */
  //@{
  /// Constructor
  CException ( std::string sError_ = "Generic AIPS Exception",
    TExceptionType theExceptionType_ = UNKNOWN, uint uiErrorNumber_ = 100, 
		const std::string &sClassName_ = "CException", 
		const std::string &sClassVersion_ = CEXCEPTION_VERSION, 
		const std::string &sDerivedFrom_ = "std::exception, CBase" )
    throw();
  /// Copy constructor
  CException ( const CException& otherException,
		const std::string &sClassName_ = "CException", 
		const std::string &sClassVersion_ = CEXCEPTION_VERSION, 
		const std::string &sDerivedFrom_ = "std::exception, CBase"  )
    throw();
  /// Destructor 
  virtual ~CException ()
    throw();
  //@}
/** \name Operators */
  //@{
	/// Assignment operator
  CException& operator= ( CException& otherException )
    throw();
  //@}
/** \name Accessors */
  //@{
  /// Returns the error code 
  uint getErrorNumber() const
    throw(); 
  /// Returns the type of the exception
  TExceptionType getExceptionType() const
    throw(); 
  /// Returns an error string 
  virtual const char* what() const
    throw();
  //@}
/** \name Other methods */
  //@{
  /// Reimplemented from CBase 
  virtual const std::string dump() const
    throw();
  //@}
private:
  std::string sErrorString;        ///< Error string returned by what()
  TExceptionType theExceptionType; ///< Type of Exception
  uint uiErrorNumber;              ///< Error code
};

/** \brief Class for initialization exceptions */
class BadInitException : public CException
{
public:
	/// Constructor
  BadInitException ( std::string sError_ = "System could not be initialized",
    TExceptionType theExceptionType_ = RECOVER, uint uiErrorNumber_ = 500 ) throw();
	/// Copy constructor
  BadInitException( const BadInitException& otherException ) 
		throw();
	/// Destructor
  virtual ~BadInitException ()
    throw();
};

/** \brief Class for out of range (e.g. array bounds) exceptions */
class OutOfRangeException : public CException
{
public:
	/// Constructor
  OutOfRangeException ( std::string sError_ = "Parameter is out of range",
    TExceptionType theExceptionType_ = RECOVER, uint uiErrorNumber_ = 400 ) throw();    
	/// Copy constructor
  OutOfRangeException( const OutOfRangeException& otherException ) 
		throw();
	/// Destructor
  virtual ~OutOfRangeException ()
    throw();
};

/** \brief Class for file exceptions */
class FileException : public CException
{
public:
	/// Constructor
  FileException( std::string sError_ = "Generic file error",
    TExceptionType theExceptionType_ = RECOVER, uint uiErrorNumber_ = 200 ) throw();
	/// Copy constructor
  FileException( const FileException& otherException ) 
		throw();
	/// Destructor
  virtual ~FileException()
    throw();
};

/** \brief Class for plugin and library exceptions */
class PlugInException : public CException
{
public:
	/// Constructor
  PlugInException( std::string sError_ = "Error on registering plugins",
    TExceptionType theExceptionType_ = FATAL, uint uiErrorNumber_ = 700 ) throw();
	/// Copy constructor
  PlugInException( const PlugInException& otherException ) 
		throw();
  /// Destructor
	virtual ~PlugInException()
    throw();
};

/** \brief Class for exceptions thrown on calls to invalid or uninitialised members */
class NotPresentException : public CException
{
public:
	/// Constructor
  NotPresentException( std::string sError_ = "Tried to access NULL reference member",
    TExceptionType theExceptionType_ = RECOVER, uint uiErrorNumber_ = 600 ) throw();
	/// Copy constructor
  NotPresentException( const NotPresentException& otherException ) 
		throw();
	/// Destructor
	virtual ~NotPresentException()
    throw();
};

/** \brief Class for exceptions thrown on detected NULL references */
class NullException : public CException
{
public:
	/// Constructor
  NullException( std::string sError_ = "Got NULL pointer! SEGFAULT feared...",
    TExceptionType theExceptionType_ = FATAL, uint uiErrorNumber_ = 300 ) throw();
	/// Copy constructor
  NullException( const NullException& otherException ) 
		throw();
	/// Destructor
  virtual ~NullException()
    throw();
};

/** \brief Class for exceptions thrown on mismatching or wrong runtime types */
class RunTimeTypeException : public CException
{
public:
	/// Constructor
  RunTimeTypeException( std::string sError_ = "Runtime type error",
    TExceptionType theExceptionType_ = FATAL, uint uiErrorNumber_ = 800 ) throw();
	/// Copy constructor
  RunTimeTypeException( const RunTimeTypeException& otherException )
		throw();
	/// Destructor
  virtual ~RunTimeTypeException()
    throw();
};

}
#endif
