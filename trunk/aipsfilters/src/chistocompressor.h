/************************************************************************
 * File: chistocompressor.h                                             *
 * Project: AIPS Histogram filter modules                               *
 * Description: Compresses each channel of the input by erasing unused  *
 *              intensities                                             *
 *                                                                      *
 * Author: Hendrik Belitz <hbelitz@users.berlios.de>                    *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 2004-02-20                                                  *
 * Changed: 2004-05-13 Corrected an error on images with full range     *
 *                     of 65535 intensities (infinite loop)             *
 *          2006-05-23 Updated source to accept all integral inputs     *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CHISTOCOMPRESSOR_H
#define CHISTOCOMPRESSOR_H
#define CHISTOCOMPRESSOR_VERSION "0.3"

// AIPS includes
#include <cgrayleveltransformation.h>
#include <chistogram.h>
// Library includes
#include "libid.h"

namespace aips {

/**
 * Compresses each channel of the input by erasing unused intensities
 * Inputs:
 * 1. A scalar image of integral type.
 * Outputs:
 * 1. A scalar image of integral type.
 */
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
/** \name Structors */
//{@
	/// Constructor
  CHistoCompressor( ulong ulID ) throw();
	/// Destructor
  ~CHistoCompressor() throw();
//@}
/** \name Other methods */
//{@
	NEW_INSTANCE( CHistoCompressor );
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
//@}
private:
	DECLARE_CALL_MACRO( call );
	template <typename ImageType>
	bool compute() throw();
};

}

#endif
