/***********************************************************************
 * File: cnoiseestimator.h                                             *
 * Project: AIPS                                                       *
 * Description: Estimation of noise level in volume image              *
 *                                                                     *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                   *
 *                                                                     *
 * Version: 0.9                                                        *
 * Status : Beta                                                       *
 * Created: 2004-08-28                                                 *
 * Changed: 2004-09-09 Standardized variable names                     *
 *                     Exceptions added, attributes are now private    *
 *                     Constructor initializes constant parameters     *
 *          2004-09-23 Filter now works on "short" data                *
 *          2004-09-29 Adapted filter for arbitrary intensity ranges   *
 *          2004-12-19 CHistogram now uses std::vector and             *
 *                     works on 2D/3D multichannel images              *
 *                     Moved CHistogram to library aipsbase            *
 *                     CHistogram no longer inherits from CFilter      *
 *          2005-01-20 Made the source code look prettier              *
 *          2005-01-21 Now works with any image dimension              *
 ***********************************************************************/
#ifndef CNOISEESTIMATOR_H
#define CNOISEESTIMATOR_H
#define CNOISEESTIMATOR_VERSION "0.9"

// Standard includes
#include <boost/lambda/lambda.hpp>

// AIPS includes
#include <cfilter.h>
#include <aipstypelist.h>

// library includes
#include "libid.h"

namespace aips {

/**
 * Estimate noise in an image. The noise is assumed to be Gaussian and homogeneous, and
 * it is assumed that the +X+Y-Z corner of the image should be homogeneous.
 * Inputs:
 * 1. A 2D or 3D image.
 * Parameters:
 * 1. Boxsize : Size of image area to estimate noise from.
 * 2. Mean : Estimated mean for the tested image area. (Output)
 * 3. Variance : Estimated mean for the tested image area. (Output)
 */
class CNoiseEstimator : public CFilter
{
private:
	/// Standard constructor 
	CNoiseEstimator();
	/// Copy constructor
	CNoiseEstimator( CNoiseEstimator& );
	/// Assignment operator
	CNoiseEstimator& operator= ( CNoiseEstimator& );
public:
/** \name Structors */
//{@
	/// Constructor
  CNoiseEstimator( ulong ulID )
    throw();
	/// Destructor
  virtual ~CNoiseEstimator()
    throw();
//@}
/** \name Other methods */
//{@
	/// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
	NEW_INSTANCE( CNoiseEstimator );
//@}
private:
	DECLARE_CALL_MACRO( call );
	template<typename ImageType>
	bool compute() throw();
};

}
#endif
