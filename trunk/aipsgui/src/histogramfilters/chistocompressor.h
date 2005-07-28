/************************************************************************
 * File: chistocompressor.h                                             *
 * Project: AIPS Histogram filter modules                               *
 * Description: Compresses each channel of the input by erasing unused  *
 *              intensities                                             *
 *                                                                      *
 * Author: Hendrik Belitz <h.belitz@fz-juelich.de>                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 04-02-20                                                    * 
 * Changed: 04-05-13 Corrected an error on images with full range       *
 *                   of 65535 intensities (infinite loop)               *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CHISTOCOMPRESSOR_H
#define CHISTOCOMPRESSOR_H

// AIPS includes
#include <cgrayleveltransformation.h>
#include <chistogram.h>
#include <cglobalprogress.h>
// Library includes
#include "libid.h"

using namespace aips;

/** Compresses each channel of the input by erasing unused intensities */
class CHistoCompressor : public CGrayLevelTransformation, private CHistogram
{
private:
  /// Standard constructor
  CHistoCompressor();
  /// Copy constructor
  CHistoCompressor( CHistoCompressor& );
  /// Assignment operator
  CHistoCompressor& operator=( CHistoCompressor& );
public:
/* Structors */
	/// Constructor
  CHistoCompressor( ulong ulID ) throw();
	/// Destructor
  ~CHistoCompressor() throw();
/* Other methods */    
  /// Reimplemented from CPipelineItem  
  virtual CPipelineItem* newInstance( ulong ulID ) const
    throw();
  /// Reimplemented from CPipelineItem    
  virtual void apply()
    throw();	
};

#endif
