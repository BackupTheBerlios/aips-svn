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
 *          2006-01-30 Corrected many errors. Added TImage and TField   *
 *                     support.                                         *
 * TODO: Add more specific error codes                                  *
 * TODO: Add support for more data types                                *
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
#include <itkDataObject.h>

namespace aips {

/**
 * Structured data adapter for ITK images
 * @author Hendrik Belitz
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
  /// Sets the external data pointer to the given dataset
  template<typename itkImageType>
  void setExternalData( typename itkImageType::Pointer anImage )
    throw( NullException ); 
/* Other methods */
  /// Converts the external data into an internal representation
  virtual TDataSetPtr convertToInternal() throw( NullException );
  /// Returns external short array representing our internal data
  template<typename itkImageType>
  typename itkImageType::Pointer convertToExternal()
    throw( NullException );
private:
	/// Actual conversion from AIPS to ITK
	template<typename itkImageType, typename DataType, int Dimension>
  typename itkImageType::Pointer actualExternalConversion();
  /// Actual conversion from ITK to AIPS
  template<typename itkImageType>
  TDataSetPtr actualInternalConversion();
  itk::DataObject::Pointer externalData; ///< Pointer to external ITK data
};


#include <citkadapter.tpp>
}
#endif
#endif
