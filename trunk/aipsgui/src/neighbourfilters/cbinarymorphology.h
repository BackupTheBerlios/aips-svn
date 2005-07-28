/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: $DATE                                                       *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CBINARYMORPHOLOGY_H
#define CBINARYMORPHOLOGY_H

#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

#include "libid.h"

using namespace aips;
 
/**
 * A class for simple morphological operations
 */
class CBinaryMorphology : public CFilter
{
private:
  /// Standard constructor
  CBinaryMorphology();
  /// Copy constructor
  CBinaryMorphology( CBinaryMorphology& );
  /// Assignment operator
  CBinaryMorphology& operator=( CBinaryMorphology& );
public:
  /// Constructor
  CBinaryMorphology( ulong ulID )
    throw();
  /// Destructor
  virtual ~CBinaryMorphology()
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
