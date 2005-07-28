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

/**************
 * CException *
 **************/

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
 */
CException::CException( string sError_, TExceptionType theExceptionType_, uint uiErrorNumber_,
	const string &sClassName_, const string &sClassVersion_, const string &sDerivedFrom_ )
  throw() : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ),
  sErrorString( sError_ ), theExceptionType( theExceptionType_ ), uiErrorNumber( uiErrorNumber_ )
{
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
}

CException::CException( const CException& otherException,
	const string &sClassName_, const string &sClassVersion_, const string &sDerivedFrom_ ) throw()
  : exception(), CBase( sClassName_, sClassVersion_, sDerivedFrom_ ) 
{
  sErrorString = otherException.sErrorString;
  theExceptionType = otherException.theExceptionType;
  uiErrorNumber = otherException.uiErrorNumber;  
}
    
/** Does nothing */
CException::~CException() throw()
{  
}

/*************
 * Operators *
 *************/

/** 
 * \param otherException exception to assign
 */
CException& CException::operator=( CException& otherException ) throw()
{
  if ( &otherException == this )
    return *this;
  sErrorString = otherException.sErrorString;
  theExceptionType = otherException.theExceptionType;
  uiErrorNumber = otherException.uiErrorNumber;
  
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
uint CException::getErrorNumber() const throw()
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
  os << "\nErrString: " << sErrorString << " ";
  os << "\ntheExceptionType " << static_cast<int>( theExceptionType ) << " ";
  os << "\nuiErrorNumber " << uiErrorNumber << " ";
  return CBase::dump() + os.str();
}

/********************
 * BadInitException *
 ********************/

BadInitException::BadInitException ( std::string sError_, TExceptionType theExceptionType_,
	uint uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "BadInitException", "0.3", "CException" )
{ 
}

BadInitException::BadInitException( const BadInitException& otherException ) throw()
  : CException ( otherException, "BadInitException", "0.3", "CException" )
{ 
}

BadInitException::~BadInitException () throw() 
{
}

/***********************
 * OutOfRangeException *
 ***********************/
 
OutOfRangeException::OutOfRangeException ( std::string sError_, TExceptionType theExceptionType_,
	uint uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "OutOfRangeException", "0.3", "CException" )
{ 
}

OutOfRangeException::OutOfRangeException( const OutOfRangeException& otherException ) throw()
	: CException ( otherException, "OutOfRangeException", "0.3", "CException" )
{ 
}
	
OutOfRangeException::~OutOfRangeException () throw() 
{
}

/*****************
 * FileException *
 *****************/
 
FileException::FileException( std::string sError_, TExceptionType theExceptionType_, 
	uint uiErrorNumber_ ) throw()
	: CException ( sError_, theExceptionType_, uiErrorNumber_, "FileException", "0.3", "CException" )
{ 
}

FileException::FileException( const FileException& otherException ) throw()
  : CException ( otherException, "FileException", "0.3", "CException" )
{ 
}

FileException::~FileException() throw() 
{
}

/*******************
 * PlugInException *
 *******************/
 
PlugInException::PlugInException( std::string sError_, TExceptionType theExceptionType_,
	uint uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "PlugInException", "0.3", "CException" )    
{  
}
	
PlugInException::PlugInException( const PlugInException& otherException ) throw()
	: CException ( otherException, "PlugInException", "0.3", "CException" )
{ 
}
  
PlugInException::~PlugInException() throw() 
{
}

/***********************
 * NotPresentException *
 ***********************/
 
NotPresentException::NotPresentException( std::string sError_, TExceptionType theExceptionType_,
	uint uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "NotPresentException", "0.3", "CException" )    
{ 
}
	
NotPresentException::NotPresentException( const NotPresentException& otherException ) throw()
	: CException ( otherException, "NotPresentException", "0.3", "CException" )
{ 
}

NotPresentException::~NotPresentException() throw() 
{
}

/*****************
 * NullException *
 *****************/
 
NullException::NullException( std::string sError_, TExceptionType theExceptionType_, 
	uint uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "NullException", "0.3", "CException" )    
{ 
}
		
NullException::NullException( const NullException& otherException ) throw()
	: CException ( otherException, "NullException", "0.3", "CException" )
{ 
}
		
NullException::~NullException() throw() 
{
}

/************************
 * RunTimeTypeException *
 ************************/

RunTimeTypeException::RunTimeTypeException( std::string sError_, TExceptionType theExceptionType_,
	uint uiErrorNumber_ ) throw()
  : CException ( sError_, theExceptionType_, uiErrorNumber_, "RunTimeTypeException", "0.1", "CException" )
{ 
}
		
RunTimeTypeException::RunTimeTypeException( const RunTimeTypeException& otherException ) throw()
	: CException ( otherException, "RunTimeTypeException", "0.1", "CException" )
{ 
}
		
RunTimeTypeException::~RunTimeTypeException() throw()
{
}

