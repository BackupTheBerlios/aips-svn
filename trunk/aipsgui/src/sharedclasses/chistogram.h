/***********************************************************************
 * File: chistogram.h                                                  *
 * Project: AIPS                                                       *
 * Description: A histogram calculation class for 2D and 3D images     *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.6                                                        *
 * Status : Beta                                                       *
 * Created: 28.08.03                                                   *
 * Changed: 09.09.03 Standardized variable names                       *
 *                   Exceptions added, attributes are now private      *
 *                   Constructor initializes constant parameters       *
 *          23.09.03 Filter now works on "short" data                  *
 *          29.09.03 Adapted filter for arbitrary intensity ranges     *
 *          16.12.03 CHistogram now uses std::vector and               *
 *                   works on 2D/3D multichannel images                *
 *          19.12.03 Moved CHistogram to library aipsbase              *
 *                   CHistogram no longer inherits from CFilter        *
 *          20.01.03 Made the source code look prettier                *
 *          21.01.03 Now works with any image dimension                *
 ***********************************************************************/

#ifndef CHISTOGRAM_H
#define CHISTOGRAM_H

// AIPS includes
#include "aipsnumeric.h"
#include "cglobalprogress.h"
#include <boost/timer.hpp>

namespace aips {

/**
 * A class for histogram calculation. All image channels are treated separately
 */
class CHistogram : public CSubject
{
public:
/* Structors */
  /// Constructor
  CHistogram()
    throw();
  /// Destructor
  virtual ~CHistogram()
    throw();
protected:
/* Other methods */
  /// Calculates the image histogram
  void calculateHistogram( TImagePtr sourcePtr )
    throw( NullException );
protected:
  /// The resulting histogram
  std::vector<std::vector<double> > histogramVec; 
  double dMaxValue; ///< Maximum histogram value
};

}

#endif
