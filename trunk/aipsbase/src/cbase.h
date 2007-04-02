/************************************************************************
 * File: cbase.h                                                        *
 * Project: AIPS                                                        *
 * Description: This is a base class for easy runtime version checking  *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.14                                                        *
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
 *        2005-11-20 Correction of constructor parameter defaults       *
 *                   Added and updated documentation                    *
 *                   Added runtime verbosity functions                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CBASE_H
#define CBASE_H

#define CBASE_VERSION "0.14"

// Include aipsbase_config.h
#include "aipsbase_config.h"

// AIPS includes
#include "aipsstring.h"

/// Namespace for all members of the aipsbase library
namespace aips {

/**
 * \brief A class for easy runtime version checking.
 *
 * This class should is an abstract base class
 * of all bigger AIPS-classes (with the exception of all policy and trait
 * classes which are not involved in the main class hierarchy. CVector also
 * isn't a child of CBase for efficiency purposes). You cannot instantiate
 * any objects of this class since the destructor is pure virtual.
 */
class CBase
{
private:
  /// Standard constructor
  CBase();
  /// Copy constructor
  CBase( const CBase& other );
  /// Assignment operator
  CBase& operator=( const CBase& );
public:
/** \name Structors */
  //@{
  /// Constructor
// CHANGE Added explicit keyword and removed first default string to eliminate warning in VS2005 (HB 2007-03-19)
  explicit CBase( const std::string &sClassName_, const std::string &sClassVersion_ = CBASE_VERSION,
    const std::string &sDerivedFrom_ = "-" ) throw();
  /// Destructor. Pure virtual to generate an abstract base class
  virtual ~CBase()
    throw() = 0;
  //@}
/** \name Accessors */
  //@{
  /// Returns the class name
  const std::string& getClassName()
    throw();
  /// Returns the class version
  const std::string& getClassVersion()
    throw();
  /// Returns the classes parents
  const std::string& getClassParents()
    throw();
  /// Return current verbosity setting
  bool isVerbose()
    throw();
  //@}
/** \name Mutators */
  //@{
  /// Set verbosity to the given parameter
  void setVerbosity( bool bVerbose_ )
    throw();
  /// Turn on verbose output
  void turnVerbosityOn()
    throw();
  /// Turn off verbose output
  void turnVerbosityOff()
    throw();
  //@}
/** \name Other Methods */
  //@{
  /// Produces an information string about the actual object.
  /// This is mainly meant for debugging purposes.
  virtual const std::string dump() const
    throw();
  //@}
private:
  std::string sClassName;    ///< Class name
  std::string sClassVersion; ///< Class version
  std::string sDerivedFrom;  ///< Direct parents of class
  bool bVerbose;             ///< Does the class have to produce verbose output?
};

}
#endif
