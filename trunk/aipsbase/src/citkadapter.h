//
// C++ Interface: citkadapter
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AIPSCITKADAPTER_H
#define AIPSCITKADAPTER_H
#include <aipsbase_config.h>
#ifdef USE_ITK
#define ITKADAPTER_VERSION "0.1"

#include <aipsnumeric.h>
#include <cstructureddataadapter.h>

#include <itkImage.h>
#include <itkImportImageFilter.h>
#include <itkCastImageFilter.h>

namespace aips {

/**
Structured data adapter for ITK images

@author Hendrik Belitz
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
};

template<typename itkImageType>
void CITKAdapter::setExternalData( typename itkImageType::Pointer anImage ) throw( NullException )
{
  if ( anImage.IsNull )
    throw( NullException( "Given image pointer is invalid" ) );
  if ( typeid( char ) == anImage->PixelType || typeid( unsigned char ) == anImage->PixelType ||
      typeid( short ) == anImage->PixelType || typeid( ushort ) == anImage->PixelType ||
      typeid( long ) == anImage->PixelType || typeid( ulong ) == anImage->PixelType )
    {
      TImagePtr anImage( new TImage( uiDimensions, dims, dataDimension ) );
      memcpy( anImage->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
      for( TImage::iterator imageIt = anImage->begin(); imageIt != anImage->end(); ++imageIt )
      {
        if ( *imageIt > anImage->getMaximum() ) anImage->setMaximum( *imageIt );
        if ( *imageIt < anImage->getMinimum() ) anImage->setMinimum( *imageIt );
      }
      externalDataPtr = anImage;
    }
    else if( typeid( float ) == anImage->PixelType )
    {
      typedef CTypedData<float> FloatImage;
      typedef boost::shared_ptr<FloatImage> FloatPtr;
      FloatPtr aFField( new FloatImage( uiDimensions, dims, dataDimension ) );
      memcpy( aFField->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
      FloatImage::iterator ffieldIt = aFField->begin();
      TFieldPtr aField( new TField( uiDimensions, dims, dataDimension ) );
      for( TField::iterator fieldIt = aField->begin(); fieldIt != aField->end(); 
        ++fieldIt, ++ffieldIt )
      {
        *fieldIt = *ffieldIt;
        if ( *fieldIt > aField->getMaximum() ) aField->setMaximum( *fieldIt );
        if ( *fieldIt < aField->getMinimum() ) aField->setMinimum( *fieldIt );
      }
      externalDataPtr = aField;
    }
    else
    {
      TFieldPtr aField( new TField( uiDimensions, dims, dataDimension ) );
      memcpy( aField->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
      for( TField::iterator fieldIt = aField->begin(); fieldIt != aField->end(); ++fieldIt )
      {
        if ( *fieldIt > aField->getMaximum() ) aField->setMaximum( *fieldIt );
        if ( *fieldIt < aField->getMinimum() ) aField->setMinimum( *fieldIt );
      }
      externalDataPtr = aField;
    }
}

template<typename itkImageType>
void CITKAdapter::convertToExternal( typename itkImageType::Pointer& anImage ) throw( NullException )
{
  cerr << ">>> CITKAdapter::convertToExternal" << endl;
  // Determine dimension size
  size_t dimensionSize[3];
  dimensionSize[0] = internalDataSPtr->getExtent(0);
  dimensionSize[1] = internalDataSPtr->getExtent(1);
  if( internalDataSPtr->getDimension() > 2 )
      dimensionSize[2] = internalDataSPtr->getExtent(2);
  else
      dimensionSize[2] = 1;
  size_t siz = dimensionSize[0] * dimensionSize[1] * dimensionSize[2];
  cerr << dimensionSize[0] << " " << dimensionSize[1] << " " << dimensionSize[2] << endl;        
      
  if ( internalDataSPtr->getType() == typeid( short ) )
  {
    cerr << "Data type is short" << endl;
    typedef itk::Image<short,3> ImageType;
    typedef itk::ImportImageFilter<short,3> ImportFilterType;
    TImagePtr image = boost::static_pointer_cast<TImage>( internalDataSPtr );
    cerr << image->getExtent(0) << " " << image->getExtent(1) << " " << image->getExtent(2) << " " << endl;
    ImportFilterType::Pointer importFilter = ImportFilterType::New();
    ImportFilterType::SizeType size;
    for( uint i = 0; i < 3; ++i )
      size[i] = dimensionSize[i];
    ImportFilterType::IndexType start;
    start.Fill(0);
    ImportFilterType::RegionType region;
    region.SetIndex( start );
    region.SetSize( size );
    importFilter->SetRegion( region );
    /* FIXME Origin and Spacing are not imported */
    double origin[3] = {0.0,0.0,0.0};
    importFilter->SetOrigin( origin );
    double spacing[3] = {1.0,1.0,1.0};
    importFilter->SetSpacing( spacing );
    importFilter->SetImportPointer( image->getArray(), siz, false );    
    typedef itk::CastImageFilter<ImageType,itkImageType> outputConverter;
    typedef typename outputConverter::Pointer outputPtr;
    outputPtr output = outputConverter::New();
    output->SetInput( importFilter->GetOutput() );
    output->Update();
    importFilter->Update();
    anImage = importFilter->GetOutput();
    return;
  }
  else if( internalDataSPtr->getType() == typeid( double ) )
  {
    typedef itk::Image<double,3> ImageType;
    typedef itk::ImportImageFilter<double,3> ImportFilterType;
    TFieldPtr image = boost::static_pointer_cast<TField>( internalDataSPtr );
    ImportFilterType::Pointer importFilter = ImportFilterType::New();
    ImportFilterType::SizeType size;
    for( uint i = 0; i < 3; ++i )
      size[i] = dimensionSize[i];
    ImportFilterType::IndexType start;
    start.Fill(0);
    ImportFilterType::RegionType region;
    region.SetIndex( start );
    region.SetSize( size );
    importFilter->SetRegion( region );
    /* FIXME Origin and Spacing are not imported */
    double origin[3] = {0.0,0.0,0.0};
    importFilter->SetOrigin( origin );
    double spacing[3] = {1.0,1.0,1.0};
    importFilter->SetSpacing( spacing );
    importFilter->SetImportPointer( image->getArray(), siz, false );
    typedef itk::CastImageFilter<ImageType,itkImageType> outputConverter;
    typedef typename outputConverter::Pointer outputPtr;
    outputPtr output = outputConverter::New();
    output->SetInput( importFilter->GetOutput() );
    output->Update();
    anImage = output->GetOutput();
  }
  else
  {
    alog << "Unknown datatype. No conversion made" << endl;
  }
  cerr << "<<< CITKAdapter::convertToExternal" << endl;
}

}
#endif
#endif
