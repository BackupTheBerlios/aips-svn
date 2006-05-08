/************************************************************************
 * File: cvectorflow.h                                                  *
 * Project: AIPS                                                        *
 * Description: Implementation of the GVF and GGVF algorithms           *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.7                                                         *
 * Status : Alpha                                                       *
 * Created: 01.09.03                                                    *
 * Changed: 04.09.03 Implementation of generalized model                *
 *          09.09.03 Standardized variable names                        *
 *                   Exceptions added, attributes are now private       *
 *                   Constructor initializes constant parameters        *
 *          15.09.03 Added dialog window as nested class                *
 *          23.09.03 Filter now works on "short" data                   *
 *          09.09.03 Optimized vector flow visualization                *
 *          25.01.04 Implemented 3D case                                *
 *          26.01.04 Added support for AIPS progress bar                * 
 *                   Simplified and clarified code                      *
 *          07.05.04 Corrected an error ( min/max of output were not    *
 *                   set, so some modules failes on using the output )  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CVECTORFLOW_H
#define CVECTORFLOW_H

/// Standard includes
#include <cmath>


// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>

// library includes
#include "libid.h"

/// Blitz includes
#ifdef USE_BLITZ
#include <blitz/array.h>
#include <blitz/array/stencils.h>
#endif

using namespace aips;

/**
 * Class to compute the GVF field from a given vector field
 */
class CVectorFlow : public CFilter
{
private:
	/// Constructor
	CVectorFlow();
	/// Copy constructor
	CVectorFlow( CVectorFlow& );
	/// Assignment operator
	CVectorFlow& operator=( CVectorFlow& );
public: 
/* Structors */
	/// Constructor
  CVectorFlow( ulong ulID )
    throw();
	/// Destructor
  virtual ~CVectorFlow()
    throw();
/* Other methods */		
	/// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID_ ) const
    throw();
  /// Reimplemented from CPipelineItem
	virtual void apply()
    throw();
private:
	boost::shared_ptr<TImage> roiPtr;
	bool bRoiSelf;
/* Implementation of the different algorithms */
	/// 2D GVF algorithm
	void gvf2D( boost::shared_ptr<TField2D> gradientFieldPtr ) throw();
	/// 2D GGVF algorithm
	void ggvf2D( boost::shared_ptr<TField2D> gradientFieldPtr ) throw();
	/// 3D GVF algorithm
	void gvf3D( boost::shared_ptr<TField3D> gradientFieldPtr ) throw();
	/// 3D GGVF algorithm
	void ggvf3D( boost::shared_ptr<TField3D> gradientFieldPtr ) throw();
#ifdef USE_BLITZ	
	/// 2D GGVF algorithm with blitz support
	void gvf2Dblitz( boost::shared_ptr<TField2D> gradientFieldPtr ) throw();
	/// 3D GGVF algorithm with blitz support
	void gvf3Dblitz( boost::shared_ptr<TField3D> gradientFieldPtr ) throw();
#endif
};

#endif
