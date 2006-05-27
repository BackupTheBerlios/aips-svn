/************************************************************************
 * File: cedgethinner.h                                                 *
 * Project: AIPS                                                        *
 * Description: Thins out and homogenieses the given vector field       *
 *              to produce a better edge field                          *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                    *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status:  Beta                                                        *
 * Created: 04-05-18                                                    *
 * Changed: 2006-05-24 Added origin and spacing for output              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CEDGETHINNER_H
#define CEDGETHINNER_H
#define CEDGETHINNER_VERSION "0.3"

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>

// library includes
#include "libid.h"

using namespace aips;

/** 
 * Implementation of a non-maximum suppression for vector fields
 * Inputs:
 * 1. A 2D or 3D vector field.
 * Outputs:
 * 1. A 2D or 3D vector field.
 */
class CEdgeThinner : public CFilter
{
private:
  /// Standard constructor
  CEdgeThinner();
  /// Copy constructor
  CEdgeThinner( CEdgeThinner& );
  /// Assignment operator
  CEdgeThinner& operator=( CEdgeThinner& );
public: 
/** \name Structors */
	//@{
  /// Constructor
  CEdgeThinner( ulong ulID )
    throw();
  /// Destructor
  ~CEdgeThinner()
    throw();
  //@}
/** \name Other methods */
	//@{
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  NEW_INSTANCE( CEdgeThinner );
  //}@
private:
	DECLARE_CALL_MACRO( call );
	template<typename FieldType> bool suppress() 
  	throw();
};

#endif
