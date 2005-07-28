/***********************************************************************
 * File: cbinarization.h                                               *
 * Project: AIPS                                                       *
 * Description: Creates a thresholded binary image from a graylevel    *
 *              input image                                            *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.4                                                        *
 * Status : Beta                                                       *
 * Created: 27.08.03                                                   *
 * Changed: 09.09.03 Standardized variable names                       *
 *                   Exceptions added, attributes are now private      *
 *                   Constructor initializes constant parameters       *
 *          23.09.03 Filter now works on "short" data                  *
 *          29.09.03 Adapted filter for arbitrary intensity ranges     *
 *          04.02.04 Clarified and documented source code              *
 *                   Re-added file to project                          *
 ***********************************************************************/
#ifndef CBINARIZATION_H
#define CBINARIZATION_H

// AIPS includes
#include <cgrayleveltransformation.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;

/**
 * Creates a thresholded binary image from an graylevel input image
 */
class CBinarization : public CGrayLevelTransformation
{
private:
  /// Standard constructor
  CBinarization();
  /// Copy constructor
  CBinarization( CBinarization& );
  /// Assignment operator
  CBinarization& operator=( CBinarization& );
public:
/* Structors */
  /// Constructor
  CBinarization( ulong ulID )
    throw();
  /// Destructor
  virtual ~CBinarization()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();  
  /// Reimplemented from CPipelineItem
  virtual void apply ()
    throw();
};

#endif
