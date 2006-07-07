//
// C++ Interface: cdiscrepancymeasures
//
// Description: 
//
//
// Author:  Hendrik Belitz <hbelitz@users.berlios.de>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CDISCREPANCYMEASURES_H
#define CDISCREPANCYMEASURES_H
#define CDISCREPANCYMEASURES_VERSION "0.3"

// AIPS includes
#include <cfilter.h>

using namespace aips;
/** 
 * Computes different discrepancy measurements for two segmented images.
 * - Inputs (both images need to be of the same type and extent):
 * 	-# Segmentation mask
 * 	-# Reference mask
 * - Outputs:
 * 	-# Done using parameters, see Parameters.
 * - Parameters:
 * 	-# Label: Label to evaluate (Input)
 * 	-# SegmentationVolume: Volume of segmentation (in voxels) (Output)
 * 	-# ReferenceVolume: Volume of reference (in voxels) (Output)
 * 	-# DistanceHD: Hausdorff distance (Output)
 * 	-# DistanceMS: Mean surface distance (Output)
 * 	-# CoeffDice: Dice Coefficient (Output)
 * 	-# NoFP: Number of false positives (not normalized) (Output)
 * 	-# NoFN: Number of false negatives (not normalized) (Output)
 *
 */
class CDiscrepancyMeasures : public CFilter
{
public:
/** \name Structors */
  //@{
  /// Constructor
  CDiscrepancyMeasures( ulong ulID )
    throw();
  /// Destructor
  virtual ~CDiscrepancyMeasures()
    throw();
  //@}
/** \name Other methods */
  //@{
  /// Reimplemented from CPipelineItem  
  virtual void apply() 
    throw();
  NEW_INSTANCE( CDiscrepancyMeasures );
  //@}
private:
  DECLARE_CALL_MACRO( call );
  /// Actual computation for specific image types
  template<typename ImageType> bool compute()
    throw();
};
/** 
 * \example evaltool.cpp
 * An example for using CDiscrepancyMeasures
 */
#endif
