/***********************************************************************
 * File: chistogramequalization.h                                      *
 * Project: QtVis                                                      *
 * Description: A histogram equalization filter                        *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.5                                                        *
 * Status : Beta                                                       *
 * Created: 28.08.03                                                   *
 * Changed: 09.09.03 Standardized variable names                       *
 *                   Exceptions added, attributes are now private      *
 *                   Constructor initializes constant parameters       *
 *          23.09.03 Filter now works on "short" data                  *
 *          29.09.03 Adapted filter for arbitrary intensity ranges     *
 *          16.12.03 FIlter now works on multichannel 2D/3D images     *
 *          25.01.04 Filter now works on multidimensional images       *
 *          29.01.04 Documented and clarified source code              *
 *          05.05.04 Added progress bar and event handler              *
 ***********************************************************************/

#ifndef CHISTOGRAMEQUALIZATION_H
#define CHISTOGRAMEQUALIZATION_H

// AIPS includes
#include <cgrayleveltransformation.h>
#include <chistogram.h>
#include <cglobalprogress.h>
// lib includes
#include "libid.h"

using namespace aips;

class CHistogramEqualization : public CGrayLevelTransformation, private CHistogram
{
private:
  /// Standard constructor
  CHistogramEqualization();
  /// Copy constructor
  CHistogramEqualization( CHistogramEqualization& );
  /// Assignment operator
  CHistogramEqualization& operator=( CHistogramEqualization& );
public:
/* Structors */
  /// Constructor
  CHistogramEqualization( ulong ulID )
    throw();
  /// Destructor
  virtual ~CHistogramEqualization()
    throw();
/* Other methods */    
  /// Reimplemented from CPipelineItem  
  virtual CPipelineItem* newInstance( ulong ulID ) const
    throw();
  /// Reimplemented from CPipelineItem    
  virtual void apply()
    throw();
};

#endif
