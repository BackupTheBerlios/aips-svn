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
	if ( internalDataSPtr->getType() == typeid( TImage::TDataType ) )
	{
		if ( internalDataSPtr->getDimension() == 2 )
			return actualExternalConversion<itkImageType, TImage::TDataType, 2>();
		else if ( internalDataSPtr->getDimension() == 3 )
			return actualExternalConversion<itkImageType, TImage::TDataType, 3>();			
		else 
			alog << LWARN << "Could only convert 2D or 3D data" << std::endl;
	}
	else if ( internalDataSPtr->getType() == typeid( TField::TDataType ) )
	{
		if ( internalDataSPtr->getDimension() == 2 )
			return actualExternalConversion<itkImageType, TField::TDataType, 2>();
		else if ( internalDataSPtr->getDimension() == 3 )
			return actualExternalConversion<itkImageType, TField::TDataType, 3>();			
		else 
			alog << LWARN << "Could only convert 2D or 3D data" << std::endl;
	}
	else 
		alog << LWARN << "Could only convert short or double data" << std::endl;
FEND;
}

template<typename itkImageType, typename TDataType, int Dimension>
typename itkImageType::Pointer CITKAdapter::actualExternalConversion()
{
	typename itk::ImportImageFilter<TDataType, Dimension>::Pointer importContainer
		= itk::ImportImageFilter<TDataType, Dimension>::New();
	size_t imageSize = 1;
	for( uint i = 0; i < internalDataSPtr->getDimension(); ++i )
		imageSize *= internalDataSPtr->getExtent( i );
	importContainer->SetImportPointer( static_cast<TDataType*>( internalDataSPtr->getVoidArray() ), 
		imageSize, false );
	typename itk::CastImageFilter<itk::Image<TDataType, Dimension> , itkImageType>::Pointer castFilter = 
		itk::CastImageFilter<itk::Image<TDataType, Dimension> , itkImageType>::New();
	castFilter->SetInput( importContainer->GetOutput() );
	castFilter->Update();
	typename itkImageType::Pointer resultImage = castFilter->GetOutput();
	return resultImage;
}
