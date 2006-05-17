//
// C++ Interface: cdiscrepancymeasures
//
// Description: 
//
//
// Author:  <>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CDISCREPANCYMEASURES_H
#define CDISCREPANCYMEASURES_H
#define CDISCREPANCYMEASURES_VERSION "0.1"

// AIPS includes
#include <cfilter.h>

using namespace aips;
/** 
 * Computes different discrepancy measurements for two segmented images.
 * Inputs (both images need to be of the same type and extent):
 * 1. Segmentation mask
 * 2. Reference mask
 * Outputs:
 *    See parameters
 * Parameters:
 * 0. Label: Label to evaluate
 * 1. SegmentationVolume: Volume of segmentation (in voxels)
 * 2. ReferenceVolume: Volume of reference (in voxels)
 * 3. DistanceHD: Hausdorff distance
 * 4. DistanceMS: Mean surface distance
 * 5. CoeffDice: Dice Coefficient
 * 6. NoFP: Number of false positives (not normalized)
 * 7. NoFN: Number of false negatives (not normalized)
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
  template<typename ImageType> bool compute()
    throw();
};
#endif
