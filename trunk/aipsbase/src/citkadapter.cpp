//
// C++ Implementation: citkadapter
//
// Description: 
//
//
// Author: Hendrik Belitz <hbelitz@users.berlios.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "citkadapter.h"
#ifdef USE_ITK

using namespace std;

namespace aips
{

CITKAdapter::CITKAdapter() throw()
  : CStructuredDataAdapter( "CITKAdapter", ITKADAPTER_VERSION, "CStructuredDataAdapter" )
{
}

CITKAdapter::CITKAdapter( TDataSetPtr internalDataSPtr_ ) throw()
  : CStructuredDataAdapter( internalDataSPtr_, "CITKAdapter", ITKADAPTER_VERSION, "CStructuredDataAdapter") 
{
}

CITKAdapter::~CITKAdapter() throw()
{
}
  
/* Other methods */    
  
TDataSetPtr CITKAdapter::convertToInternal() throw( NullException )
{  
	TDataSetPtr anImage;
/*	if ( externalData.IsNull() )
		throw( NullException() );*/ //TODO GCC3.3 fails
	
	if ( typeid( *externalData ) == typeid( itk::Image<short,2> ) )
		return actualInternalConversion< itk::Image<boost::int16_t,2> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<boost::uint8_t,2> ) )
    return actualInternalConversion< itk::Image<boost::uint8_t,2> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<boost::int32_t,2> ) )
    return actualInternalConversion< itk::Image<boost::int32_t,2> >();
	else if ( typeid( *externalData ) == typeid( itk::Image<float,2> ) )
		return actualInternalConversion< itk::Image<float,2> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<double,2> ) )
    return actualInternalConversion< itk::Image<double,2> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<boost::int16_t,3> ) )
    return actualInternalConversion< itk::Image<boost::int16_t,3> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<boost::uint8_t,3> ) )
    return actualInternalConversion< itk::Image<boost::uint8_t,3> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<boost::int32_t,3> ) )
    return actualInternalConversion< itk::Image<boost::int32_t,3> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<float,3> ) )
    return actualInternalConversion< itk::Image<float,3> >();
  else if ( typeid( *externalData ) == typeid( itk::Image<double,3> ) )
    return actualInternalConversion< itk::Image<double,3> >();
	cerr << "Data is neither short nor double." << endl;
	return anImage;
}

template<typename itkImageType>
TDataSetPtr CITKAdapter::actualInternalConversion()
{
	typename itkImageType::Pointer theImage = static_cast<itkImageType*>( externalData.GetPointer() );
	typename itkImageType::RegionType theRegion = theImage->GetLargestPossibleRegion();
	typename itkImageType::SizeType theSize = theRegion.GetSize();
	std::vector<size_t> dimensions;
	for( unsigned int i = 0; i < itkImageType::SizeType::GetSizeDimension(); ++i )
		dimensions.push_back( theSize[i] );
	itk::ImageRegionConstIterator<itkImageType> it ( theImage, theRegion );
	boost::shared_ptr<CTypedData<typename itkImageType::PixelType> > img( 
		new CTypedData<typename itkImageType::PixelType>( itkImageType::SizeType::GetSizeDimension(),
    dimensions ) );
	typename CTypedData<typename itkImageType::PixelType>::iterator ot = img->begin();
	for( it.GoToBegin(); !it.IsAtEnd(); ++it, ++ot )
		*ot = it.Get();
	// Set origin and spacing
	typename itkImageType::PointType theOrigin = theImage->GetOrigin();
	typename itkImageType::SpacingType theSpacing = theImage->GetSpacing();
	img->setOrigin( 0, theOrigin[0] );
	img->setOrigin( 1, theOrigin[1] );
	img->setOrigin( 2, theOrigin[2] );
	img->setBaseElementDimension( 0, theSpacing[0] );
	img->setBaseElementDimension( 1, theSpacing[1] );
	img->setBaseElementDimension( 2, theSpacing[2] );
	return img;
}

} /* namespace aips */

#endif
