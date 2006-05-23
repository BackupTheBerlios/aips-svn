/************************************************************************
 * File: chistogram2d.h                                                 *
 * Project: AIPS Histogram filter modules                               *
 * Description: Heuristic to compute hysteresis thresholds for a        *
 *               given image                                            *
 *                                                                      *
 * Author: Hendrik Belitz <hbelitz@users.berlios.de>                    *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-03-09                                                  *
 * Changed: 2006-04-08 Correction of some minor bugs                    *
 *          2006-05-23 Updated source to accept all integral inputs     *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CHISTOGRAM2D_H
#define CHISTOGRAM2D_H
#define CHISTOGRAM2D_VERSION "0.3"

#include <boost/lambda/lambda.hpp>

#include <cfilter.h>
#include <chistogram.h>
#include "libid.h"

namespace aips {

/**
 * Heuristic to compute hysteresis thresholds for a given image, edge image and
 * classification image. 
 * \remarks All input images need to be of the same type.
 * Inputs:
 * 1: A scalar multidimensional, multichannel data set (the intensity image)
 * 2: A scalar multidimensional, multichannel data set (the classified image)
 * 3: A scalar multidimensional, multichannel data set (the gradient image)
 * Parameters:
 * 1: Class1 - class number one in input 2 to examine
 * 2: Class2 - class number two in input 2 to examine
 * 3: HistoSmooth - Amount of smoothing for the resulting edge histogram (0.25)
 * 4: Shift - Minimum shift between the thresholds (2)
 * 5: T1 - First threshold (output)
 * 6: T2 - Second threshold (output)
 */
class CHistogram2D : public CFilter, private CHistogram
{
private:
  /// Standard constructor
  CHistogram2D();
  /// Copy constructor
  CHistogram2D( CHistogram2D& );
  /// Assignment operator
  CHistogram2D& operator=( CHistogram2D& );
public:
/** \name Structors */
//{@
  /// Constructor
  CHistogram2D( ulong ulID )
    throw();
  /// Destructor
  virtual ~CHistogram2D()
    throw();
//@}    
/** \name Other methods */
//{@
	NEW_INSTANCE( CHistogram2D );
  /// Reimplemented from CPipelineItem    
  virtual void apply()
    throw();
//@}
private:
	DECLARE_CALL_MACRO( call );
	template<typename ImageType>
	bool compute() throw();
};

}
#endif
