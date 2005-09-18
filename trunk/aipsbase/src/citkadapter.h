/************************************************************************
 * File: citkadapter.h                                                  *
 * Project: AIPS                                                        *
 * Description: Adapter class for ITK datasets                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-09-03                                                  *
 * Changed: 2005-09-18 Added debugging output                           *
 * ToDo: Add more specific error codes                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSCITKADAPTER_H
#define AIPSCITKADAPTER_H
#include <aipsbase_config.h>
#ifdef USE_ITK
#define ITKADAPTER_VERSION "0.2"

#include <aipsnumeric.h>
#include <cstructureddataadapter.h>

#include <itkImage.h>
#include <itkImportImageFilter.h>
#include <itkCastImageFilter.h>

namespace aips {

/**
 * Structured data adapter for ITK images
 *@author Hendrik Belitz
 */
class CITKAdapter : public CStructuredDataAdapter
{
private:
  /// Copy constructor
  CITKAdapter( const CITKAdapter& );
  /// Assignment operator
  CITKAdapter operator=( const CITKAdapter& );
public:
/* Structors */
  /// Constructor
  CITKAdapter()
    throw(); 
  /// Constructor with additional parameter for internal data
  CITKAdapter( TDataSetPtr internalDataPtr_ )
    throw();  
  /// Destructor - pure virtual
  virtual ~CITKAdapter()
    throw();
/* Accessors */
  /// Sets the external data pointer to the given dataset (short input)
  template<typename itkImageType>
  void setExternalData( typename itkImageType::Pointer anImage )
    throw( NullException ); 
/* Other methods */
  /// Converts the external data into an internal representation
  virtual TDataSetPtr convertToInternal();
  /// Returns external short array representing our internal data
  template<typename itkImageType>
  void convertToExternal( typename itkImageType::Pointer& anImage )
    throw( NullException );
private:
  TDataSetPtr externalDataPtr; ///< Pointer to external itk image
  /// Template function to do the actual conversion
  template<typename internalDataType, typename itkImageType, typename dataType>
  typename itkImageType::Pointer doActualConversion()
    throw();
};


#include <citkadapter.tpp>
}
#endif
#endif
