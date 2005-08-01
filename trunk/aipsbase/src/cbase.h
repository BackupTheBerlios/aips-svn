/************************************************************************
 * File: cbase.h                                                        *
 * Project: AIPS                                                        *
 * Description: This is a base class for easy runtime version checking  *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.13                                                        *
 * Status:  Beta                                                        *
 * Created: 2003-09-09                                                  *
 * Changed:                                                             *
 *        2003-09-13 Documentation                                      *
 *        2003-09-18 Added dump() method                                *
 *        2003-09-29 Reorganized file structure, documentation moved    *
 *                    to cbase.cpp                                      *
 *        2003-11-09 Renamed parameters to new coding standard scheme.  *
 *                    get...-Methods are no longer inline               *
 *                   Added CBase to namespace aips                      *
 *        2003-11-27 Removed implementation of private methods          *
 *        2003-12-17 Added include statement for config.h               *
 *        2004-01-14 Made the source code look prettier                 *
 *        2004-01-26 Added private copy constructor to CBase            *
 *        2004-04-28 Added include for aipsstring.h                     *
 *        2004-11-22 Removed setinfo. Updated constructor accordingly   *
 *                   Made contructor protected                          *
 *        2004-11-25 Exchanged SigC++ for boost::signal                 *
 *        2005-03-28 Removed all signaling stuff                        *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CBASE_H
#define CBASE_H

// Include aipsbase_config.h (for USE_BLITZ or USE_STANDALONE, BENCHMARK and USE_FLOAT)
#include "aipsbase_config.h"

// AIPS includes
#include "aipsstring.h"

/// Namespace for all members of the aipsbase library
namespace aips {

/**
 * A class for easy runtime version checking. This class should is an abstract base class 
 * of all AIPS-classes (except CVector for effeciency purposes). You cannot instantiate any
 * objects of this class since the destructor is pure virtual.
 */
class CBase 
{
private:
  /// Standard constructor 
  CBase();
  /// Copy constructor
  CBase( const CBase& );
  /// Assignment operator
  CBase& operator=( const CBase& );
public:
/* Structors */
  /// Constructor
  CBase( const std::string &sClassName_, const std::string &sClassVersion_ = "0.6",
    const std::string &sDerivedFrom_ = "CBase" ) throw();
  /// Destructor. Pure virtual to generate an abstract base class
  virtual ~CBase() 
    throw() = 0;
/* Accessors */
  /// Returns the class name
  const std::string& getClassName()
    throw();
  /// Returns the class version
  const std::string& getClassVersion()
    throw();
  /// Returns the classes parents
  const std::string& getClassParents()
    throw();
/* Other Methods */    
  /// Produces an information string about the actual object. This is mainly meant for debugging purposes
  virtual const std::string dump() const
    throw();
private:
  std::string sClassName;   ///< Class name
  std::string sClassVersion;///< Class version
  std::string sDerivedFrom; ///< Direct parents of class
};

}
#endif
