/***********************************************************************
 * File: cgrayleveltransformation.h                                    *
 * Project: AIPS                                                       *
 * Description: Base class for histogram-based transformations         *
 *                                                                     *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                   *
 *                                                                     *
 * Version: 0.4                                                        *
 * Status : Beta                                                       *
 * Created: 2003-08-27                                                 *
 * Changed: 2003-09-09 Standardized variable names                     *
 *                     Exceptions added, attributes are now private    *
 *                     Constructor initializes constant parameters     *
 *          2003-09-23 Filter now works on "short" data                *
 *          2003-09-29 Adapted filter for arbitrary intensity ranges   *
 *          2003-12-09 Moved class to library aispbase                 *
 *          2004-01-20 Made the source code look prettier              *
 *          2004-01-24 Now works on images of any dimension            *
 *          2004-01-26 Added documentation of member functions         *
 *          2004-05-23 Updated documentation                           *
 ***********************************************************************/

#ifndef CGRAYLEVELTRANSFORMATION_H
#define CGRAYLEVELTRANSFORMATION_H
#define CGRAYLEVELTRANSFORMATION_VERSION "0.4"

// AIPS includes
#include "cfilter.h"
#include "aipsnumeric.h"

namespace aips {

/**
 * An abstract base class for all gray level transformation filters
 * \remarks This transformation only works on integral image types. In addition,
 * the input and output images need to be of the same type.
 */
class CGrayLevelTransformation : public CFilter
{
public:
/** \name Structors */
//{@
  /// Constructor
  CGrayLevelTransformation( ulong ulID, const std::string &sName =
    "Generic gray-level transformation", 
    const ushort usNoOfInputs = 1, const ushort usNoOfOutputs = 1,
		const std::string &sClassName_ = "CGrayLevelTransformation", 
		const std::string &sClassVersion_ = CGRAYLEVELTRANSFORMATION_VERSION,
		const std::string &sDerivedFrom_ = "CFilter" ) throw();
  /// Destructor
  virtual ~CGrayLevelTransformation()
    throw();
//@}    
/** \name Other methods */
//{@
  /// Applies the filter to the given input
  virtual void apply()
    throw() = 0;
//@}    
protected:
  std::vector<ushort> usNewGrayValuesVec; ///< the new gray values
  /// Apply the new gray values
  void applyNewGrayValues ( TDataSetPtr aSourceSPtr, TDataSetPtr anOutputSPtr,
    ushort usChannel = 0 ) throw( NullException );
private:
	template <typename ImageType>
	void compute( boost::shared_ptr<ImageType> aSourceSPtr,
		boost::shared_ptr<ImageType> anOutputSPtr, ushort usChannel = 0 ) throw();
};

}
#endif
