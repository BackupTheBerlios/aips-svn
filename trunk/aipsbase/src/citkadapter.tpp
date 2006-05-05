template<typename itkImageType>
void CITKAdapter::setExternalData( typename itkImageType::Pointer anImage )
    throw( NullException )
{  
	if ( anImage.IsNull() )
		throw( NullException() );
	externalData = anImage;
}

/**
 * \param anImage Pointer to store the new ITK image in. Any given pointer will be overwritten!
 * \throws NullException if there is currently no internal data registered to this adapter
 */
template<typename itkImageType>
typename itkImageType::Pointer CITKAdapter::convertToExternal() throw( NullException )
{
FBEGIN;
	if ( !internalDataSPtr )
		throw( NullException() );
	if ( internalDataSPtr->getDimension() != itkImageType::IndexType::GetIndexDimension() )
	{
		alog << LWARN << "Could not convert data between "
		<< internalDataSPtr->getDimension() << " and " 
		<< itkImageType::IndexType::GetIndexDimension() << std::endl;
	}
	else if ( internalDataSPtr->getType() == typeid( TImage::TDataType ) )
	{
/*		if ( internalDataSPtr->getDimension() == 2 )
			return actualExternalConversion2D<itkImageType, TImage::TDataType>();
		else */if ( internalDataSPtr->getDimension() == 3 )
			return actualExternalConversion3D<itkImageType, TImage::TDataType>();
		else 
			alog << LWARN << "Could only convert 3D data" << std::endl;
	}
	else if ( internalDataSPtr->getType() == typeid( TSmallImage::TDataType ) )
	{
/*		if ( internalDataSPtr->getDimension() == 2 )
			return actualExternalConversion2D<itkImageType, TField::TDataType>();
		else */if ( internalDataSPtr->getDimension() == 3 )
			return actualExternalConversion3D<itkImageType, TSmallImage::TDataType>();
		else 
			alog << LWARN << "Could only convert 3D data" << std::endl;
	}
	else if ( internalDataSPtr->getType() == typeid( TField::TDataType ) )
	{
/*		if ( internalDataSPtr->getDimension() == 2 )
			return actualExternalConversion2D<itkImageType, TField::TDataType>();
		else */if ( internalDataSPtr->getDimension() == 3 )
			return actualExternalConversion3D<itkImageType, TField::TDataType>();
		else 
			alog << LWARN << "Could only convert 3D data" << std::endl;
	}
	else 
		alog << LWARN << "Could only convert short or double data" << std::endl;
	typename itkImageType::Pointer nullSPtr;
	return nullSPtr;
FEND;
}

// template<typename itkImageType, typename TDataType>
// typename itkImageType::Pointer CITKAdapter::actualExternalConversion2D()
// {
// 	typename itk::ImportImageFilter<TDataType, 2>::Pointer importContainer
// 		= itk::ImportImageFilter<TDataType, 2>::New();
// 	size_t imageSize = 1;
// 	alog << LINFO << "Conversion from ";
// 	for( uint i = 0; i < internalDataSPtr->getDimension(); ++i )
// 	{
// 		imageSize *= internalDataSPtr->getExtent( i );
// 		alog << internalDataSPtr->getExtent( i ) << " x ";
// 	}
// 	alog << "1" << std::endl;
// 	typename itkImageType::SpacingType sp;
// 	typename itkImageType::PointType org;
// 	typename itkImageType::RegionType region;
// 	typename itkImageType::SizeType size;
// 	typename itkImageType::IndexType index;
// 	for( uint i = 0; i < internalDataSPtr->getDimension(); ++i )
// 	{
// 		sp[i] = internalDataSPtr->getBaseElementDimension(i);
// 		org[i] = internalDataSPtr->getOrigin(i);
// 		index[i] = 0;
// 		size[i] = internalDataSPtr->getExtent(i);
// 	}
// 	region.SetIndex( index );
// 	region.SetSize( size );
// 	importContainer->SetRegion( region );
// 	importContainer->SetSpacing( sp );
// 	importContainer->SetOrigin( org );
// 	importContainer->SetImportPointer( static_cast<TDataType*>( internalDataSPtr->getVoidArray() ), 
// 		imageSize, false );
// 	typename itk::CastImageFilter<itk::Image<TDataType, 2> , itkImageType>::Pointer castFilter = 
// 		itk::CastImageFilter<itk::Image<TDataType, 2> , itkImageType>::New();
// 	castFilter->SetInput( importContainer->GetOutput() );
// 	castFilter->Update();
// 	alog << LINFO << "Result is ";
// 	
// 	typename itkImageType::Pointer resultImage = castFilter->GetOutput();
// 	
// 	/* TODO */
// 	region = resultImage->GetLargestPossibleRegion();
// 	size = region.GetSize();
// 	alog << size[0] << " x " << size[1];
// 	if ( Dimension == 3 )
// 		alog << " x " << size[2];
// 	alog << std::endl;
// 	return resultImage;
// }

template<typename itkImageType, typename TDataType>
typename itkImageType::Pointer CITKAdapter::actualExternalConversion3D()
{
	typename itk::ImportImageFilter<TDataType, 3>::Pointer importContainer
		= itk::ImportImageFilter<TDataType, 3>::New();
	size_t imageSize = 1;
	alog << LINFO << "Conversion from ";
	for( uint i = 0; i < internalDataSPtr->getDimension(); ++i )
	{
		imageSize *= internalDataSPtr->getExtent( i );
		alog << internalDataSPtr->getExtent( i ) << " x ";
	}
	alog << "1" << std::endl;
	typename itkImageType::SpacingType sp;
	typename itkImageType::PointType org;
	typename itkImageType::RegionType region;
	typename itkImageType::SizeType size;
	typename itkImageType::IndexType index;
	for( uint i = 0; i < internalDataSPtr->getDimension(); ++i )
	{
		sp[i] = internalDataSPtr->getBaseElementDimension(i);
		org[i] = internalDataSPtr->getOrigin(i);
		index[i] = 0;
		size[i] = internalDataSPtr->getExtent(i);
	}
	region.SetIndex( index );
	region.SetSize( size );
	importContainer->SetRegion( region );
	importContainer->SetSpacing( sp );
	importContainer->SetOrigin( org );
	importContainer->SetImportPointer( static_cast<TDataType*>( internalDataSPtr->getVoidArray() ), 
		imageSize, false );
	typename itk::CastImageFilter<itk::Image<TDataType, 3> , itkImageType>::Pointer castFilter = 
		itk::CastImageFilter<itk::Image<TDataType, 3> , itkImageType>::New();
	castFilter->SetInput( importContainer->GetOutput() );
	castFilter->Update();
	alog << LINFO << "Result is ";
	
	typename itkImageType::Pointer resultImage = castFilter->GetOutput();
	
	/* TODO */
	region = resultImage->GetLargestPossibleRegion();
	size = region.GetSize();
	alog << size[0] << " x " << size[1];
	alog << " x " << size[2];
	alog << std::endl;
	return resultImage;
}
