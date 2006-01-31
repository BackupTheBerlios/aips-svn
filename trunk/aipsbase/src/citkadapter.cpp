//
// C++ Implementation: citkadapter
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
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
	if ( externalData.IsNull() )
		throw( NullException() );
	
	if ( typeid( *externalData ) == typeid( itk::Image<short,2> ) )
		return actualInternalConversion< itk::Image<short,2> >();
	else if ( typeid( *externalData ) == typeid(itk::Image<TFloatType,2>) )
		return actualInternalConversion< itk::Image<TFloatType,2> >();
	else if ( typeid( *externalData ) == typeid( itk::Image<short,3> ) )
		return actualInternalConversion< itk::Image<short,3> >();
	else if ( typeid( *externalData ) == typeid(itk::Image<TFloatType,3>) )
		return actualInternalConversion< itk::Image<TFloatType,3> >();
	else
	{
		cerr << "Data is neither short nor double." << endl;
		return anImage;
	}		
}

template<typename itkImageType>
TDataSetPtr CITKAdapter::actualInternalConversion()
{
	typename itkImageType::Pointer theImage = static_cast<itkImageType*>( externalData.GetPointer() );
	typename itkImageType::RegionType theRegion = theImage->GetLargestPossibleRegion();
	typename itkImageType::SizeType theSize = theRegion.GetSize();
	std::vector<size_t> dimensions;
	for( uint i = 0; i < itkImageType::SizeType::GetSizeDimension(); ++i )
		dimensions.push_back( theSize[i] );
	itk::ImageRegionConstIterator<itkImageType> it ( theImage, theRegion );
	boost::shared_ptr<CTypedData<typename itkImageType::PixelType> > img( 
		new CTypedData<typename itkImageType::PixelType>( itkImageType::SizeType::GetSizeDimension(), dimensions ) );
	typename CTypedData<typename itkImageType::PixelType>::iterator ot = img->begin();
	for( it.GoToBegin(); !it.IsAtEnd(); ++it, ++ot )
		*ot = it.Get();			
	return img;
}

} /* namespace aips */

#endif
