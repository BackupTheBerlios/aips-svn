/************************************************************************
 * File: cmorphology.h                                                  *
 * Project: AIPS neighbourhood filters plugin library                   *
 * Description: A class for simple morphological operations             *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Beta                                                        *
 * Created: 2004-02-11                                                  *
 * Changed: 2004-05-06 Demangled and documented source code             *
 *          2004-07-09 Added parameter to filter only pixels with       *
 *                      background neighbourhood                        *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CMORPHOLOGY_H
#define CMORPHOLOGY_H

#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

#include "libid.h"

using namespace aips;
 
/**
 * A class for simple morphological operations
 */
class CMorphology : public CFilter
{
private:
  /// Standard constructor
  CMorphology();
  /// Copy constructor
  CMorphology( CMorphology& );
  /// Assignment operator
  CMorphology& operator=( CMorphology& );
public:
  /// Constructor
  CMorphology( ulong ulID )
    throw();
  /// Destructor
  virtual ~CMorphology()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply ()
    throw();
private:
	/// Actual implementation for 2D
	void morph2D( const TImage& input, const TImage& kernel ) 
		throw();
	/// Actual implementation for 3D
	void morph3D( const TImage& input, const TImage& kernel ) 
		throw();
};

#endif
