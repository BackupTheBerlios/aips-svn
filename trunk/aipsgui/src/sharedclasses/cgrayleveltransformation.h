/***********************************************************************
 * File: cgrayleveltransformation.h                                    *
 * Project: AIPS                                                       *
 * Description: An abstract base class for all intensity based filters *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.3                                                        *
 * Status : Beta                                                       *
 * Created: 27.08.03                                                   *
 * Changed: 09.09.03 Standardized variable names                       *
 *                   Exceptions added, attributes are now private      *
 *                   Constructor initializes constant parameters       *
 *          23.09.03 Filter now works on "short" data                  *
 *          29.09.03 Adapted filter for arbitrary intensity ranges     *
 *          19.12.03 Moved class to library aispbase                   *
 *          20.01.04 Made the source code look prettier                *
 *          21.01.04 Now works on images of any dimension              *
 *          26.01.04 Added documentation of member functions           *
 ***********************************************************************/

#ifndef CGRAYLEVELTRANSFORMATION_H
#define CGRAYLEVELTRANSFORMATION_H

// AIPS includes
#include "cfilter.h"
#include "aipsnumeric.h"

namespace aips {

/** An abstract base class for all gray level transformation filters */
class CGrayLevelTransformation : public CFilter
{
public:
/* Structors */
  /// Constructor
  CGrayLevelTransformation( ulong ulID, const std::string &sName =
    "Generic gray-level transformation", 
    const ushort usNoOfInputs = 1, const ushort usNoOfOutputs = 1,
		const std::string &sClassName_ = "CGrayLevelTransformation", 
		const std::string &sClassVersion_ = "0.3", const std::string &sDerivedFrom_ = "CFilter" ) throw();
  /// Destructor
  virtual ~CGrayLevelTransformation()
    throw();
/* Other methods */
  /// Applies the filter to the given input
  virtual void apply()
    throw() = 0;  
protected:
  std::vector<ushort> usNewGrayValuesVec; ///< the new gray values
  /// Apply the new gray values
  void applyNewGrayValues ( TImagePtr sourcePtr, TImagePtr outputPtr, 
    ushort usChannel = 0 ) throw( NullException );
};

}
#endif
