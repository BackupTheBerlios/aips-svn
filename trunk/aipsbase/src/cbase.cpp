/************************************************************************
 * File: cbase.cpp                                                      *
 * Project: AIPS                                                        *
 * Description: This is a base class for easy runtime version checking  *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 2003-09-09                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cbase.h"

using namespace std;
using namespace aips;

/*************
 * Structors *
 *************/

/**
 * Verbosity is turned off by default in non-debug code. If DL1 is activated, verbosity is turned on by
 * default.
 * \param sClassName_ Class name
 * \param sClassVersion_ Class version
 * \param sDerivedFrom_ Parent classes
 * \post All member variables are initialised
 */
CBase::CBase( const string &sClassName_, const string &sClassVersion_,
  const string &sDerivedFrom_ ) throw()
  : sClassName( sClassName_ ), sClassVersion( sClassVersion_ ), sDerivedFrom( sDerivedFrom_ ),
#ifdef DL1  
  bVerbose( true )
#else
  bVerbose( false )
#endif
{
}

CBase::CBase( const CBase& other )
	: sClassName( other.sClassName ), sClassVersion( other.sClassVersion ),
	  sDerivedFrom( other.sDerivedFrom ), bVerbose( other.bVerbose )
{
}

CBase::~CBase() throw()
{
}

/************* 
 * Accessors *
 *************/

/**
 * \returns the class name
 */
const string& CBase::getClassName() throw()
{
  return sClassName;
}

/**
 * \returns the class version
 */
const string& CBase::getClassVersion() throw()
{
  return sClassVersion;
}

/**
 * \returns the classes parents
 */
const string& CBase::getClassParents() throw()
{
  return sDerivedFrom;
}

/**
 * \return true if verbosity is turned on
 */
bool CBase::isVerbose() throw()
{
  return bVerbose;
}

/************
 * Mutators *
 ************/

/**
 * \param bVerbose_ new verbosity status
 * \post verbosity level bVerbose was updated
 */
void CBase::setVerbosity( bool bVerbose_ ) throw()
{
  bVerbose = bVerbose_;
}

/** \post verbosity is enabled */
void CBase::turnVerbosityOn() throw()
{
  bVerbose = true;
}

/** \post verbosity is disabled */
void CBase::turnVerbosityOff() throw()
{
  bVerbose = false;
}

/*****************
 * Other Methods *
 *****************/

/**
 * This should be overwritten by each direct and indirect child of CBase, since CBase::dump only
 * shows name, version and parents of a class. You should output the complete state, which means
 * all data members, of the class here.
 * \returns information string
 */
const string CBase::dump() const throw()
{
  return ( sClassName + ", V" + sClassVersion + ", Parents: " + sDerivedFrom + "\n " );
}
