//
// C++ Implementation: citkhandler
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "citkhandler.h"

#ifdef HAVE_ITK
#include <itkImage.h>
#include <itkImportImageFilter.h>
#include <itkImageIOBase.h>
#include <itkImageIOFactory.h>
#include <itkImageFileWriter.h>

using namespace aips;
using namespace boost;
using namespace std;

CITKHandler::CITKHandler() throw()
  : CBinaryFileHandler( "CITKHandler", "0.4", "CBinaryFileHandler" )
{
  supportedFileTypesVec.push_back("PNG");
  supportedFileTypesVec.push_back("png");
  supportedFileTypesVec.push_back("hdr");
  supportedFileTypesVec.push_back("HDR");
  supportedFileTypesVec.push_back("bmp");
  supportedFileTypesVec.push_back("BMP");
  supportedFileTypesVec.push_back("jpeg");
  supportedFileTypesVec.push_back("JPEG");
  supportedFileTypesVec.push_back("jpg");
  supportedFileTypesVec.push_back("JPG");
  supportedFileTypesVec.push_back("pnm");
  supportedFileTypesVec.push_back("PNM");
  supportedFileTypesVec.push_back("tif");
  supportedFileTypesVec.push_back("TIF");
  supportedFileTypesVec.push_back("tiff");
  supportedFileTypesVec.push_back("TIFF");
}

CITKHandler::~CITKHandler() throw()
{
}

/**
 * \param sFilename Name of the volume file
 * \exception FileException on any file error
 */
TDataFile CITKHandler::load( const string& sFilename )
  const throw( FileException )
{
FBEGIN;
		// Initialize reader and load image
    shared_ptr<CImageHeader> aHeader( new CImageHeader );
		TDataSetPtr aDataSet;
		
		itk::ImageIOBase::Pointer imageIO 
			= itk::ImageIOFactory::CreateImageIO( sFilename.c_str(), itk::ImageIOFactory::ReadMode );
		if ( imageIO.IsNull() )
		{
			alog << LWARN << "File type not supported (this should never happen!)" << std::endl;
			return make_pair( aDataSet, aHeader );
		}
		
		imageIO->SetFileName( sFilename.c_str() );
		imageIO->ReadImageInformation();
		
		uint uiDimensions = imageIO->GetNumberOfDimensions();
		if ( uiDimensions > 4 )
		{
			alog << LWARN << "We only support up to 4 dimensions yet!" << std::endl;
			uiDimensions = 4;
		}
		itk::ImageIORegion ioRegion( uiDimensions );
		itk::ImageIORegion::SizeType ioSize = ioRegion.GetSize();
		itk::ImageIORegion::IndexType ioStart = ioRegion.GetIndex();
		uint dims[4] = {0};
		float spacing[4] = {1.0,1.0,1.0,1.0};
		float origin[4] = {1.0,1.0,1.0,1.0};
		for( uint i=0; i < uiDimensions; ++i )
		{
			ioStart[i] = 0;
			ioSize[i] = imageIO->GetDimensions( i );
			dims[i] = imageIO->GetDimensions( i );
			spacing[i] = imageIO->GetSpacing( i );
			origin[i] = imageIO->GetSpacing( i );
			cerr << "### " << i << " Dim: " << dims[i] << " Spac " << spacing[i] << std::endl;
		}
		
		ioRegion.SetSize( ioSize );
		ioRegion.SetIndex( ioStart );
		
		cerr << "ioRegion: " << ioRegion << std::endl;
		imageIO->SetIORegion( ioRegion );
		void* buffer = malloc( imageIO->GetImageSizeInBytes() );
		imageIO->Read( buffer );
		if ( uiDimensions==4 && dims[3] == 1 )
			uiDimensions = 3;
		if ( uiDimensions==3 && dims[2] == 1 )
			uiDimensions = 2;
		size_t dataDimension = imageIO->GetNumberOfComponents();
		
		cerr << "Resulting dimension " << uiDimensions << " " << dataDimension << std::endl;
		
		// Now we must take this buffer and convert it into a CTypedData<> structure
		
		if ( typeid( short ) == imageIO->GetComponentTypeInfo() )
		{
			TImagePtr anImage( new TImage( uiDimensions, dims, dataDimension ) );
			memcpy( anImage->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
			for( TImage::iterator imageIt = anImage->begin(); imageIt != anImage->end(); ++imageIt )
			{
				anImage->adjustDataRange( *imageIt );
			}
			aDataSet = anImage;			
		}
    else if( typeid( unsigned char ) == imageIO->GetComponentTypeInfo() )
    {
      shared_ptr<CTypedData<unsigned char> > 
        aFField( new CTypedData<unsigned char>( uiDimensions, dims, dataDimension ) );
      memcpy( aFField->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
      CTypedData<unsigned char>::iterator ffieldIt = aFField->begin();
      TImagePtr aField( new TImage( uiDimensions, dims, dataDimension ) );
      for( TImage::iterator fieldIt = aField->begin(); fieldIt != aField->end();
        ++fieldIt, ++ffieldIt )
      {
        *fieldIt = *ffieldIt;
        aField->adjustDataRange( *fieldIt );
      }
      aDataSet = aField;
    }
    else if( typeid( int8_t ) == imageIO->GetComponentTypeInfo() )
    {
      shared_ptr<CTypedData<int8_t> >
        aFField( new CTypedData<int8_t>( uiDimensions, dims, dataDimension ) );
      memcpy( aFField->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
      CTypedData<int8_t>::iterator ffieldIt = aFField->begin();
      TImagePtr aField( new TImage( uiDimensions, dims, dataDimension ) );
      for( TImage::iterator fieldIt = aField->begin(); fieldIt != aField->end();
        ++fieldIt, ++ffieldIt )
      {
        *fieldIt = *ffieldIt;
        aField->adjustDataRange( *fieldIt );
      }
      aDataSet = aField;
    }
		else if( typeid( float ) == imageIO->GetComponentTypeInfo() )
		{
			shared_ptr<CTypedData<float> > 
				aFField( new CTypedData<float>( uiDimensions, dims, dataDimension ) );
			memcpy( aFField->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
			CTypedData<float>::iterator ffieldIt = aFField->begin();
			TFieldPtr aField( new TField( uiDimensions, dims, dataDimension ) );
			for( TField::iterator fieldIt = aField->begin(); fieldIt != aField->end(); 
				++fieldIt, ++ffieldIt )
			{
				*fieldIt = *ffieldIt;
				aField->adjustDataRange( *fieldIt );
			}
			aDataSet = aField;
		}
		else
		{
			TFieldPtr aField( new TField( uiDimensions, dims, dataDimension ) );
			memcpy( aField->getVoidArray(), buffer, imageIO->GetImageSizeInBytes() );
			for( TField::iterator fieldIt = aField->begin(); fieldIt != aField->end(); ++fieldIt )
			{
				aField->adjustDataRange( *fieldIt );
			}
			aDataSet = aField;
		}
		
		free( buffer );
		buffer = NULL;
		
		aDataSet->setBaseElementDimension( 0, spacing[0] );
		aDataSet->setBaseElementDimension( 1, spacing[1] );
		if ( aDataSet->getDimension() == 3 )
			aDataSet->setBaseElementDimension( 2, spacing[2] );

		aDataSet->setOrigin( 0, origin[0] );
		aDataSet->setOrigin( 1, origin[1] );
		if ( aDataSet->getDimension() == 3 )
			aDataSet->setOrigin( 2, origin[2] );
			
    return make_pair( aDataSet, aHeader );
}

/**
 * \param sFilename Name of the volume file
 * \param theData Pair of data set and header (will be ignored for this file type) to be saved
 * \exception FileException on any file error
 */
void CITKHandler::save( const string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
  // Determine dimension size
  size_t dimensionSize[3];
  dimensionSize[0] = theData.first->getExtent(0);
  dimensionSize[1] = theData.first->getExtent(1);
  if( theData.first->getDimension() > 2 )
      dimensionSize[2] = theData.first->getExtent(2);
  else
      dimensionSize[2] = 1;			
	if ( theData.first->getType() == typeid( short ) )
	{		    
    TImagePtr image = static_pointer_cast<TImage>( theData.first );
//     if ( image->getDataRange().getMaximum() < 256 )
//     {
//     	cerr << "Image is byte" << endl;
//     	TSmallImagePtr sm( new TSmallImage( image->getDimension(), image->getExtents() ) );
//     	sm->setOrigin( image->getOrigin() );
//     	sm->setBaseElementDimensions( image->getBaseElementDimensions() );
//     	TImage::iterator ot = image->begin();
//     	for( TSmallImage::iterator it = sm->begin(); it != sm->end(); ++it, ++ot )
//     		(*it)=static_cast<uint8_t>( (*ot) );
// 	    CITKAdapter myAdapter( sm );
// 			typedef itk::Image<uint8_t,3> ImageType;
//   	  typedef itk::ImageFileWriter<ImageType> FileWriter;
//     	ImageType::Pointer myimage;
// 	    myimage = myAdapter.convertToExternal<ImageType>();
// 	    FileWriter::Pointer writer = FileWriter::New();
// 			writer->SetFileName( sFilename.c_str() );
//   		writer->SetInput( myimage );
// 			try
// 			{
// 				writer->Update();
// 			}
// 			catch( itk::ExceptionObject& err )
// 			{
// 				cout << err << endl;
// 				throw( FileException( 
// 					"CITKHandler - Unknown image format in dataset. Image was not saved" ) );
// 			}
// 		}
//     else
//     {
	    CITKAdapter myAdapter( image );
			typedef itk::Image<short,3> ImageType;
  	  typedef itk::ImageFileWriter<ImageType> FileWriter;
    	ImageType::Pointer myimage;
	    myimage = myAdapter.convertToExternal<ImageType>();
	//    myimage = importFilter->GetOutput();
	    FileWriter::Pointer writer = FileWriter::New();
			writer->SetFileName( sFilename.c_str() );
  		writer->SetInput( myimage );
			try
			{
				writer->Update();
			}
			catch( itk::ExceptionObject& err )
			{
				cout << err << endl;
				throw( FileException( 
					"CITKHandler - Unknown image format in dataset. Image was not saved" ) );
			}
//		}
	}
#ifdef USE_DOUBLE
	else if( theData.first->getType() == typeid( double ) )
	{
		typedef itk::Image<double,3> ImageType;
		typedef itk::ImportImageFilter<double,3> ImportFilterType;
#else
  else if( theData.first->getType() == typeid( float ) )
  {
    typedef itk::Image<float,3> ImageType;
    typedef itk::ImportImageFilter<float,3> ImportFilterType;
#endif    
		typedef itk::ImageFileWriter<ImageType> FileWriter;
    
		FileWriter::Pointer writer = FileWriter::New();
		TFieldPtr image = static_pointer_cast<TField>( theData.first );
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
		importFilter->SetImportPointer( image->getArray(), image->getSize(), false );
		writer->SetFileName( sFilename.c_str() );
		writer->SetInput( importFilter->GetOutput() );
		try
		{
			writer->Update();
		}
		catch( itk::ExceptionObject& err )
		{
			cout << err << endl;
			throw( FileException( 
				"CITKHandler - Unknown image format in dataset. Image was not saved" ) );
		}
	}
 else if( theData.first->getType() == typeid( uint8_t ) )
 {
    typedef itk::Image<uint8_t,3> ImageType;
    typedef itk::ImportImageFilter<uint8_t,3> ImportFilterType;
    typedef itk::ImageFileWriter<ImageType> FileWriter;
    
    FileWriter::Pointer writer = FileWriter::New();
    TSmallImagePtr image = static_pointer_cast<TSmallImage>( theData.first );
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
    importFilter->SetImportPointer( image->getArray(), image->getSize(), false );
    writer->SetFileName( sFilename.c_str() );
    writer->SetInput( importFilter->GetOutput() );
    try
    {
      writer->Update();
    }
    catch( itk::ExceptionObject& err )
    {
      cout << err << endl;
      throw( FileException( 
        "CITKHandler - Unknown image format in dataset. Image was not saved" ) );
    }
  } 
	else
	{
		alog << "Unknown datatype. Nothing saved" << endl;
	}
}
#endif
