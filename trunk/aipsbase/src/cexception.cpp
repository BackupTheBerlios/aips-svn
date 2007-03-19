/************************************************************************
 * File: cexception.cpp                                                 *
 * Project: AIPS                                                        *
 * Description: Exception classes                                       *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 12.09.03                                                    *
 * Based on: SDL++ exception.h                                          *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#include "cexception.h"

using namespace std;
using namespace aips;

//++++++++++++++++++++++++++++++++++++++++
// CException 
//++++++++++++++++++++++++++++++++++++++++

/*************
 * Structors *
 *************/
 
/**
 * \param sError_ String describing the exception
 * \param theExceptionType_ Type of exception (\see TExceptiontype)
 * \param uiErrorNumber_ Error identification number
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 * \post All members are initialised properly
 * The constructor automatically prepends an error type to the given error string.
 */
CException::CException( string sError_, TExceptionType theExceptionType_, unsigned int uiErrorNumber_,
	const string &sClassName_, const string &sClassVersion_, const string &sDerivedFrom_ )
  throw() : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ),
  sErrorString( sError_ ), theExceptionType( theExceptionType_ ), uiErrorNumber( uiErrorNumber_ )
{
  // Prepend an error type to the given string.
  switch ( theExceptionType )
  {
    case WARNING:
      sErrorString = "#WARNING# " + sErrorString;
      break;
    case RECOVER:
      sErrorString = "#RECOVER# " + sErrorString;
      break;
    case FATAL:
      sErrorString = "# FATAL # " + sErrorString;
      break;
    default:
      sErrorString = "#UNKNOWN# " + sErrorString;
      break;
  }
  if ( isVerbose() )
    cerr << dump() << endl;
}

/**
 * \param otherException exception to copy information from
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 * \post All members are initialised properly
 */
CException::CException( const CException& otherException,
	const string &sClassName_, const string &sClassVersion_, const string &sDerivedFrom_ ) throw()
  : exception(), CBase( sClassName_, sClassVersion_, sDerivedFrom_ ),
    sErrorString( otherException.sErrorString ), theExceptionType ( otherException.theExceptionType ),
    uiErrorNumber ( otherException.uiErrorNumber )
{  
  if ( isVerbose() )
    cerr << dump() << endl;
}
    
/** Does nothing */
CException::~CException() throw()
{  
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of CException" << endl;
}

/*************
 * Operators *
 *************/

/** 
 * \param otherException exception to assign
 * \post All information got copied from otherException to *this.
 */
CException& CException::operator=( CException& otherException ) throw()
{
  if ( &otherException == this )
    return *this;
  sErrorString = otherException.sErrorString;
  theExceptionType = otherException.theExceptionType;
  uiErrorNumber = otherException.uiErrorNumber;

  if ( isVerbose() )
    cerr << dump() << endl;
    
  return *this;
}


/**
 * Inherited and overwritten from std::exception
 * \returns error string
 */
const char* CException::what() const throw()
{
  return sErrorString.data();
}

/** 
 * Returns an error code to further specify the type of the exception
 * \returns the error code 
 */
unsigned int CException::getErrorNumber() const throw()
{
  return uiErrorNumber;
}

/**
 * Returns the type of the exception
 * \returns type of exception
 */
CException::TExceptionType CException::getExceptionType() const throw()
{
  return theExceptionType;
}

const string CException::dump() const throw()
{
  ostringstream os;  
  os << "\n- sErrorString " << sErrorString << " ";
  os << "\n- theExceptionType " << static_cast<int>( theExceptionType ) << " ";
  os << "\n- uiErrorNumber " << uiErrorNumber << " ";
  return CBase::dump() + os.str();
}

/********************
 * BadInitException *
 ********************/

BadInitException::BadInitException ( std::string sError_, TExceptionType theExceptionType_,
	unsigned int uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "BadInitException", "0.4", "CException" )
{ 
}

BadInitException::BadInitException( const BadInitException& otherException ) throw()
  : CException ( otherException, "BadInitException", "0.4", "CException" )
{ 
}

BadInitException::~BadInitException () throw() 
{
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of CBadInitException" << endl;
}

/***********************
 * OutOfRangeException *
 ***********************/
 
OutOfRangeException::OutOfRangeException ( std::string sError_, TExceptionType theExceptionType_,
	unsigned int uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "OutOfRangeException", "0.4", "CException" )
{ 
}

OutOfRangeException::OutOfRangeException( const OutOfRangeException& otherException ) throw()
	: CException ( otherException, "OutOfRangeException", "0.4", "CException" )
{ 
}
	
OutOfRangeException::~OutOfRangeException () throw() 
{
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of COutfRangeException" << endl;
}

/*****************
 * FileException *
 *****************/
 
FileException::FileException( std::string sError_, TExceptionType theExceptionType_, 
	unsigned int uiErrorNumber_ ) throw()
	: CException ( sError_, theExceptionType_, uiErrorNumber_, "FileException", "0.4", "CException" )
{ 
}

FileException::FileException( const FileException& otherException ) throw()
  : CException ( otherException, "FileException", "0.4", "CException" )
{ 
}

FileException::~FileException() throw() 
{
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of CFileException" << endl;
}

/*******************
 * PlugInException *
 *******************/
 
PlugInException::PlugInException( std::string sError_, TExceptionType theExceptionType_,
	unsigned int uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "PlugInException", "0.4", "CException" )
{

}
	
PlugInException::PlugInException( const PlugInException& otherException ) throw()
	: CException ( otherException, "PlugInException", "0.4", "CException" )
{ 
}
  
PlugInException::~PlugInException() throw() 
{
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of CPlugInException" << endl;
}

/***********************
 * NotPresentException *
 ***********************/
 
NotPresentException::NotPresentException( std::string sError_, TExceptionType theExceptionType_,
	unsigned int uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "NotPresentException", "0.4", "CException" )
{ 
}
	
NotPresentException::NotPresentException( const NotPresentException& otherException ) throw()
	: CException ( otherException, "NotPresentException", "0.4", "CException" )
{ 
}

NotPresentException::~NotPresentException() throw() 
{
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of CNotPresentException" << endl;
}

/*****************
 * NullException *
 *****************/
 
NullException::NullException( std::string sError_, TExceptionType theExceptionType_, 
	unsigned int uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "NullException", "0.4", "CException" )
{ 
}
		
NullException::NullException( const NullException& otherException ) throw()
	: CException ( otherException, "NullException", "0.4", "CException" )
{ 
}
		
NullException::~NullException() throw() 
{
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of CNullException" << endl;
}

/************************
 * RunTimeTypeException *
 ************************/

RunTimeTypeException::RunTimeTypeException( std::string sError_, TExceptionType theExceptionType_,
	unsigned int uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "RunTimeTypeException", "0.4", "CException" )
{ 
}
		
RunTimeTypeException::RunTimeTypeException( const RunTimeTypeException& otherException ) throw()
	: CException ( otherException, "RunTimeTypeException", "0.4", "CException" )
{ 
}
		
RunTimeTypeException::~RunTimeTypeException() throw()
{
  if ( isVerbose() )
    cerr << "Deleting instance " << static_cast<void*>( this ) << " of CRunTimeException" << endl;
}

