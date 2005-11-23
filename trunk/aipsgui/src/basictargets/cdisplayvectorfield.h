/************************************************************************
 * File: cdisplayvectorfield.h                                          *
 * Project: AIPS 0- Basic targets plugin library                        *
 * Description: A display for 2D vector fields                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 2003-12-10                                                  *
 * Changed: 2004-01-19 Simplified and corrected vector drawing          *
 *          2004-01-28 Documented and simplified source code            *
 *                     Display is now using the new CImageDisplay class *
 *                     Moved inline members to cpp file                 *
 *          2004-02-06 Removed uneccessary constructor param sCaption   *
 *          2004-06-16 Updated documenation and error output            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CDISPLAYVECTORFIELD_H
#define CDISPLAYVECTORFIELD_H


// AIPS includes
#include <ctarget.h>
#include <aipsnumeric.h>
#include <cimagedisplay.h>
#include "cdisplayvectordialog.h"


using namespace aips;

/**
 * A display for 2D vector fields
 */
class CDisplayVectorField : public CTarget
{
private:
  /// Standard constructor
  CDisplayVectorField();
  /// Copy constructor
  CDisplayVectorField( CDisplayVectorField& );
  /// Assignment operator
  CDisplayVectorField& operator= ( CDisplayVectorField& );
public:
/* Structors */
  /// Constructor
  CDisplayVectorField( ulong ulID )
    throw();
  /// Destructor
  virtual ~CDisplayVectorField()
    throw();
/* Other functions */    
  /// Reimplemented from CPipelineItem
  virtual void apply() throw();
  /// Reimplemented from CPipelineItem
  virtual const std::string dump() const
    throw();
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
private:		
	boost::shared_ptr<CDisplayVectorDialog> theDisplay;
	
}; 

#endif
