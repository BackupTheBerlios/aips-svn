
/**
 * \param anImage image to convert into internal data representation
 * \throws NullException if given image pointer is invalid
 */
template<typename itkImageType>
void CITKAdapter::setExternalData( typename itkImageType::Pointer anImage ) throw( NullException )
{
FBEGIN;
  if ( anImage.IsNull() )
  {
    FEND;
    throw( NullException( "Given image pointer is invalid", RECOVER, ERR_CALLERNULL ) );
  }
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
    boost::shared_ptr<FloatImage> aFField( new FloatImage( uiDimensions, dims, dataDimension ) );
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
  else if( typeid( double ) == anImage->PixelType )
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
  else
  {
    alog << LWARN << SSHORTERR( "Type ") << anImage->PixelType.name() << " is currently not supported" << endl;
  }
FEND;
}

/**
 * \param anImage Pointer to store the new ITK image in. Any given pointer will be overwritten!
 * \throws NullException if there is currently no internal data registered to this adapter
 */
template<typename itkImageType>
void CITKAdapter::convertToExternal( typename itkImageType::Pointer& anImage ) throw( NullException )
{
FBEGIN;
  if ( !internalDataSPtr )
  {
    throw( NullException( "Currently no interal data available", RECOVER, ERR_REQUESTNULL ) );
    FEND;
  }  
  // Determine dimension size
  size_t dimensionSize[3];
  dimensionSize[0] = internalDataSPtr->getExtent(0);
  dimensionSize[1] = internalDataSPtr->getExtent(1);
  if( internalDataSPtr->getDimension() > 2 )
      dimensionSize[2] = internalDataSPtr->getExtent(2);
  else
      dimensionSize[2] = 1;
  size_t dataSize = dimensionSize[0] * dimensionSize[1] * dimensionSize[2];
DBG3( dimensionSize[0] << " " << dimensionSize[1] << " " << dimensionSize[2] );      
  if ( internalDataSPtr->getType() == typeid( short ) )
    anImage = doActualConversion<TImage,itkImageType,short>();
  else if( internalDataSPtr->getType() == typeid( double ) )
    anImage = doActualConversion<TField,itkImageType,double>();
  else
    alog << SSHORTERR( "Conversion of given datatype is not supported yet" ) << endl;
FEND;
}

/**
 * \returns ITK image according to given types
 */
template<typename internalDataType, typename itkImageType, typename dataType>
typename itkImageType::Pointer CITKAdapter::doActualConversion() throw()
{
FBEGIN;
  using boost::shared_ptr;
  using boost::static_pointer_cast;
  typedef itk::Image<dataType, 3> TImageType;
  typedef itk::ImportImageFilter<dataType,3> TImportFilterType;
  shared_ptr<internalDataType> imageSPtr =
    static_pointer_cast<internalDataType>( internalDataSPtr );
  typename TImportFilterType::Pointer theImportFilter = TImportFilterType::New();
  typename TImportFilterType::SizeType actualSize;
  for( uint i = 0; i < 3; ++i )
    actualSize[i] = dimensionSize[i];
  typename TImportFilterType::IndexType startIndex;
  startIndex.Fill( 0 );
  typename TImportFilterType::RegionType theRegionType;
  theRegionType.SetIndex( startIndex );
  theRegionType.SetSize( actualSize );
  theImportFilter->SetRegion( theRegionType );
  double theImageOriginArr[3] = { imageSPtr->getOrigin( 0 ), imageSPtr->getOrigin( 2 ), 0.0 };
  double theImageSpacingArr[3] = { imageSPtr->getBaseElementDimension( 0 ),
    imageSPtr->getBaseElementDimension( 1 ), 1.0 };
  if ( imageSPtr->getDimension() == 3 )
  {
    theImageOriginArr[2] = imageSPtr->getOrigin( 2 );
    theImageSpacingArr[2] = imageSPtr->getBaseElementDimension( 2 );
  }
  theImportFilter->SetOrigin( theImageOriginArr );
  theImportFilter->SetSpacing( theImageSpacingArr );
  theImportFilter->SetImportPointer( imageSPtr->getArray(), dataSize, false );
  typedef itk::CastImageFilter<TImageType, itkImageType> TOutputConverter;
  typedef typename TOutputConverter::Pointer TOutputSPtr;
  TOutputSPtr theOutput = TOutputConverter::New();
  theOutput->SetInput( theImportFilter->GetOutput() );
  theOutput->Update();
FEND;
  return theImportFilter->GetOutput();
}