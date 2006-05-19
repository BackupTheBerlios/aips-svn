//
// C++ Interface: CVTKAdapter
//
// Description: 
//
//
// Author: Hendrik Belitz <hbelitz@users.berlios.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

// TODO: Support for vector fields and data with more than one channel

#ifndef AIPSCVTKADAPTER_H
#define AIPSCVTKADAPTER_H
#define CVTKADAPTER_VERSION "0.1"
#include <aipsbase_config.h>
#ifdef USE_VTK
#include <cstructureddataadapter.h>

#include "vtkPointData.h"
#include "vtkBitArray.h"
#include "vtkCharArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkShortArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkLongArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkStructuredPoints.h"
#include "vtkImageData.h"

namespace aips {

/**
@author Hendrik Belitz
*/
class CVTKAdapter : public CStructuredDataAdapter
{
private:
  /// Copy constructor
  CVTKAdapter( const CVTKAdapter& );
  /// Assignment operator
  CVTKAdapter operator=( const CVTKAdapter& );
public:
/* Structors */
  /// Constructor
  CVTKAdapter()
    throw(); 
  /// Constructor with additional parameter for internal data
  CVTKAdapter( TDataSetPtr internalDataPtr_ )
    throw();  
  /// Destructor - pure virtual
  virtual ~CVTKAdapter()
    throw();
/* Accessors */
  /// Sets the external data pointer to the given dataset (short input)
  void setExternalData( vtkImageData* aVTKImage )
    throw( NullException );
/* Other methods */
  /// Converts the external data into an internal representation
  virtual TDataSetPtr convertToInternal();
  /// Returns external short array representing our internal data  
  vtkImageData* convertToExternal()
    throw( NullException );
private:
  vtkImageData* externalDataPtr; ///< Pointer to external vtk image
  /// Converts a given vtkDataArray into an AIPS image volume
 template<typename TSet, typename TArray>
  boost::shared_ptr<TSet> convertVTKImage( TArray* aVtkArray, std::vector<size_t> dim ) const;
};

}
#endif
#endif
