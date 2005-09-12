 /************************************************************************
 * File: ccommonimagehandler.cpp                                        *
 * Project: AIPS                                                        *
 * Description: A file handler for most common 2D image formats         *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 2005-03-22                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "ccommonimagehandler.h"

#ifdef HAVE_IM

using namespace std;
using namespace boost;
using Magick::Image;
using Magick::CoderInfo;

/** Constructor */
CCommonImageHandler::CCommonImageHandler() throw()
  : CBinaryFileHandler( "CCommonImageHandler", "0.3", "CFileHandler" )
{  
  list<CoderInfo> coderList;
	coderInfoList( &coderList, CoderInfo::TrueMatch, CoderInfo::TrueMatch, CoderInfo::AnyMatch); 
	list<CoderInfo>::iterator entry = coderList.begin();
	while( entry != coderList.end() )
	{
		std::string s( entry->name() );				
		for( string::iterator it = s.begin(); it != s.end(); ++it )
				(*it) = toupper( (*it) );
    supportedFileTypesVec.push_back( s );
		for( string::iterator it = s.begin(); it != s.end(); ++it )
				(*it) = tolower( (*it) );
    supportedFileTypesVec.push_back( s );
		++entry;
	}
	Image::cacheThreshold( 64 );
}

/** Destructor */
CCommonImageHandler::~CCommonImageHandler() throw()
{
}

/**
 * Loads an unsigned short scalar image dataset from the given file
 * \param sFilename Name of the image file (Must be a portable network graphics)
 * \exception FileException on any file error
 */
TDataFile CCommonImageHandler::load( const std::string& sFilename ) const throw( FileException )
{
	// Check if given format supports multiframes. If so, load the image as a sequence instead of a single 2D image
	string::size_type extensionPos = sFilename.find_last_of( '.' );
	string sFileSuffix = sFilename.substr( extensionPos+1 );
	string sLargeSuffix = sFileSuffix;
	for ( uint i = 0; i < sLargeSuffix.length(); i++ )
    sLargeSuffix[i] = toupper( sLargeSuffix[i] );
	CoderInfo coderInfo( sLargeSuffix );
	if ( coderInfo.isMultiFrame() )
	{
		return loadImageSequence( sFilename );
	}
	
	// Check size of ImageMagick quantum to determine maximum intensity
	short sIntensityMaximum = 255;
	if ( sizeof( Magick::Quantum ) > 1 )
		sIntensityMaximum = numeric_limits<short>::max();
	
	// Load file, get type and check dimensions
	Image anImage;
	try
	{
		anImage.read( sFilename );
	}
	catch( std::exception& e )
	{
		alog << LWARN << e.what() << endl;
		throw( FileException( "Error on reading image file" ) );
	}
	size_t dimensionSize[] = { anImage.columns(), anImage.rows() };
	Magick::ImageType imageType = anImage.type();
	shared_ptr<TImage> aDataSet;
	// Import file from image magick
	if ( imageType == Magick::GrayscaleType || imageType == Magick::GrayscaleMatteType )
	{
		aDataSet.reset( new TImage( 2, dimensionSize ) );
		for( uint y = 0; y < anImage.rows(); ++y )
			for( uint x = 0; x < anImage.columns(); ++x )
			{
				Magick::ColorGray grayColor = anImage.pixelColor( x, y );
				short shade = static_cast<short>( round( grayColor.shade() * static_cast<double>( sIntensityMaximum ) ) );
				(*aDataSet)( dimensionSize[0] - 1 - x, y ) = shade;
			}
	}
	else if ( imageType == Magick::BilevelType )
	{
		sIntensityMaximum = 1;
		aDataSet.reset( new TImage( 2, dimensionSize ) );
		for( uint y = 0; y < anImage.rows(); ++y )
			for( uint x = 0; x < anImage.columns(); ++x )
			{
				Magick::ColorMono monoColor = anImage.pixelColor( x, y );
				(*aDataSet)( dimensionSize[0] - 1 - x, y ) = monoColor.mono();
			}
	}
	else 
	{
		aDataSet.reset( new TImage( 2, dimensionSize, 3 ) );
		for( uint y = 0; y < anImage.rows(); ++y )
			for( uint x = 0; x < anImage.columns(); ++x )
			{
				Magick::ColorRGB color = anImage.pixelColor( x, y );
				short sShadeRed = static_cast<short>( round( color.red() * static_cast<double>( sIntensityMaximum ) ) );
				short sShadeGreen = static_cast<short>( round( color.green() * static_cast<double>( sIntensityMaximum ) ) );
				short sShadeBlue = static_cast<short>( round( color.blue() * static_cast<double>( sIntensityMaximum ) ) );
				(*aDataSet)( dimensionSize[0] - 1 - x, y, 0 ) = sShadeRed;
				(*aDataSet)( dimensionSize[0] - 1 - x, y, 1 ) = sShadeGreen;
				(*aDataSet)( dimensionSize[0] - 1 - x, y, 2 ) = sShadeBlue;
			}
	}
	// Set data range
	aDataSet->setMinimum( 0 );
	aDataSet->setMaximum( sIntensityMaximum );
	// Set header
  CImageHeader* aHeader = new CImageHeader;
  aHeader->setLong( "ExtentX", static_cast<long>( dimensionSize[0] ) );
  aHeader->setLong( "ExtentY", static_cast<long>( dimensionSize[1] ) );
  aHeader->setLong( "VoxelSize", ( sizeof( Magick::Quantum ) > 1 ) ? 2 : 1 );
  aHeader->setLong( "Channels", static_cast<long>( aDataSet->getDataDimension() ) );
  return make_pair( aDataSet, aHeader );
}

TDataFile CCommonImageHandler::loadImageSequence( const std::string& sFilename ) const
		throw( FileException )
{
	list<Image> imageList;
	try
	{
		readImages( &imageList, sFilename );
	}
	catch( std::exception& e )
	{
		alog << LWARN << e.what() << endl;
		throw( FileException( "Error on reading image file" ) );
	}
	// Check size of ImageMagick quantum to determine maximum intensity
	short sIntensityMaximum = 255;
	if ( sizeof( Magick::Quantum ) > 1 )
		sIntensityMaximum = numeric_limits<short>::max();
	// Check if we accidently loaded a single frame image
	shared_ptr<CImageHeader> aHeader( new CImageHeader() );
	shared_ptr<TImage> aDataSet;
	if ( imageList.size() == 1 ) 
	{
		Image anImage = *imageList.begin();
		size_t dimensionSize[] = { anImage.columns(), anImage.rows() };
	  aHeader->setLong( "ExtentX", static_cast<long>( dimensionSize[0] ) );
		aHeader->setLong( "ExtentY", static_cast<long>( dimensionSize[1] ) );
		Magick::ImageType imageType = anImage.type();

		// Import file from image magick
		if ( imageType == Magick::GrayscaleType || imageType == Magick::GrayscaleMatteType )
		{
			aDataSet.reset( new TImage( 2, dimensionSize ) );
			for( uint y = 0; y < anImage.rows(); ++y )
				for( uint x = 0; x < anImage.columns(); ++x )
				{
					Magick::ColorGray grayColor = anImage.pixelColor( x, y );
					short shade = static_cast<short>( round( grayColor.shade() * static_cast<double>( sIntensityMaximum ) ) );
					(*aDataSet)( dimensionSize[0] - 1 - x, y ) = shade;
				}
		}
		else if ( imageType == Magick::BilevelType )
		{
			sIntensityMaximum = 1;
			aDataSet.reset( new TImage( 2, dimensionSize ) );
			for( uint y = 0; y < anImage.rows(); ++y )
				for( uint x = 0; x < anImage.columns(); ++x )
				{
					Magick::ColorMono monoColor = anImage.pixelColor( x, y );
					(*aDataSet)( dimensionSize[0] - 1 - x, y ) = monoColor.mono();
				}
		}
		else 
		{
			aDataSet.reset( new TImage( 2, dimensionSize, 3 ) );
			for( uint y = 0; y < anImage.rows(); ++y )
				for( uint x = 0; x < anImage.columns(); ++x )
				{
					Magick::ColorRGB color = anImage.pixelColor( x, y );
					short sShadeRed = static_cast<short>( round( color.red() * static_cast<double>( sIntensityMaximum ) ) );
					short sShadeGreen = static_cast<short>( round( color.green() * static_cast<double>( sIntensityMaximum ) ) );
					short sShadeBlue = static_cast<short>( round( color.blue() * static_cast<double>( sIntensityMaximum ) ) );
					(*aDataSet)( dimensionSize[0] - 1 - x, y, 0 ) = sShadeRed;
					(*aDataSet)( dimensionSize[0] - 1 - x, y, 1 ) = sShadeGreen;
					(*aDataSet)( dimensionSize[0] - 1 - x, y, 2 ) = sShadeBlue;
				}
		}		
	}
	else
	{
		Image anImage = *imageList.begin();
		size_t dimensionSize[] = { anImage.columns(), anImage.rows(), imageList.size() };
		aHeader->setLong( "ExtentX", static_cast<long>( dimensionSize[0] ) );
		aHeader->setLong( "ExtentY", static_cast<long>( dimensionSize[1] ) );
		aHeader->setLong( "ExtentZ", static_cast<long>( dimensionSize[2] ) );
		Magick::ImageType imageType = anImage.type();
		if ( imageType == Magick::GrayscaleType || imageType == Magick::GrayscaleMatteType || imageType == Magick::BilevelType )
			aDataSet.reset( new TImage( 3, dimensionSize ) );
		else
			aDataSet.reset( new TImage( 3, dimensionSize, 3 ) );			
		ushort z = 0;
		for( list<Image>::iterator it = imageList.begin(); it != imageList.end(); ++it, ++z )
		{
			anImage = *it;
			// Import file from image magick
			if ( imageType == Magick::GrayscaleType || imageType == Magick::GrayscaleMatteType )
			{				
				for( uint y = 0; y < anImage.rows(); ++y )
					for( uint x = 0; x < anImage.columns(); ++x )
					{
						Magick::ColorGray grayColor = anImage.pixelColor( x, y );
						short shade = static_cast<short>( round( grayColor.shade() * static_cast<double>( sIntensityMaximum ) ) );
						(*aDataSet)( dimensionSize[0] - 1 - x, y, z ) = shade;
					}
			}
			else if ( imageType == Magick::BilevelType )
			{
				sIntensityMaximum = 1;
				for( uint y = 0; y < anImage.rows(); ++y )
					for( uint x = 0; x < anImage.columns(); ++x )
					{
						Magick::ColorMono monoColor = anImage.pixelColor( x, y );
						(*aDataSet)( dimensionSize[0] - 1 - x, y, z ) = monoColor.mono();
					}
			}
			else 
			{
				for( uint y = 0; y < anImage.rows(); ++y )
					for( uint x = 0; x < anImage.columns(); ++x )
					{
						Magick::ColorRGB color = anImage.pixelColor( x, y );
						short sShadeRed = static_cast<short>( round( color.red() * static_cast<double>( sIntensityMaximum ) ) );
						short sShadeGreen = static_cast<short>( round( color.green() * static_cast<double>( sIntensityMaximum ) ) );
						short sShadeBlue = static_cast<short>( round( color.blue() * static_cast<double>( sIntensityMaximum ) ) );
						(*aDataSet)( dimensionSize[0] - 1 - x, y, z, 0 ) = sShadeRed;
						(*aDataSet)( dimensionSize[0] - 1 - x, y, z, 1 ) = sShadeGreen;
						(*aDataSet)( dimensionSize[0] - 1 - x, y, z, 2 ) = sShadeBlue;
					}
			}		
		}
	}
	// Set data range
	aDataSet->setMinimum( 0 );
	aDataSet->setMaximum( sIntensityMaximum );
	aHeader->setLong( "VoxelSize", ( sizeof( Magick::Quantum ) > 1 ) ? 2 : 1 );
	aHeader->setLong( "Channels", static_cast<long>( aDataSet->getDataDimension() ) );
  return make_pair( aDataSet, aHeader );
}
/**
 * Saves an unsigned short scalar image dataset to the given file
 * \param sFilename Name of the image file
 * \param theData Pair of dataset and header (header isn't taken into account) 
 * \exception FileException on any file error
 */
void CCommonImageHandler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
	// Check input image and initialize image magick classes
	if ( !( theData.first->getType() == typeid( dataTraits<TImage>::dataType ) ) )
	{
		alog << LWARN << "Illegal input type for CommonImage writer" << endl;
		return;
	}
  shared_ptr<TImage> aDataSet = static_pointer_cast<TImage>( theData.first );
	if ( aDataSet.get() == NULL || aDataSet->getDimension() < 2 || aDataSet->getDimension() > 3 )
	{
		alog << LWARN << "Illegal input for image writer" << endl;
		return;
	}
	if ( aDataSet->getDimension() == 3 )
	{
		saveImageSequence( sFilename, aDataSet );
		return;
	}
	Image anImage( Magick::Geometry( aDataSet->getExtent( 0 ), aDataSet->getExtent( 1 ) ), "black" );
	anImage.classType( Magick::DirectClass );
	anImage.type( Magick::TrueColorType );
	Magick::PixelPacket* pixel_cache = anImage.setPixels( 0, 0, aDataSet->getExtent(0), aDataSet->getExtent(1) );
	anImage.syncPixels();

	// Do the actual transformation
	for( uint y = 0; y < aDataSet->getExtent(1); ++y )
		for( uint x = 0; x < aDataSet->getExtent(0); ++x )
		{
			if ( aDataSet->getDataDimension() == 2 )
			{
				Magick::PixelPacket *pixel = pixel_cache + y * aDataSet->getExtent( 0 ) + (aDataSet->getExtent( 0 ) - 1 - x);
				Magick::ColorRGB actColor(
					static_cast<double>( (*aDataSet)( x, y, 0 ) ) / static_cast<double>( aDataSet->getMaximum() ),
					static_cast<double>( (*aDataSet)( x, y, 1 ) ) / static_cast<double>( aDataSet->getMaximum() ), 0.0 );
				*pixel = actColor;
			}
			else if ( aDataSet->getDataDimension() > 2 )
			{
				Magick::PixelPacket *pixel = pixel_cache + y * aDataSet->getExtent( 0 ) + (aDataSet->getExtent( 0 ) - 1 - x);
				Magick::ColorRGB actColor(
					static_cast<double>( (*aDataSet)( x, y, 0 ) ) / static_cast<double>( aDataSet->getMaximum() ),
					static_cast<double>( (*aDataSet)( x, y, 1 ) ) / static_cast<double>( aDataSet->getMaximum() ),
					static_cast<double>( (*aDataSet)( x, y, 2 ) ) / static_cast<double>( aDataSet->getMaximum() ) );
				*pixel = actColor;
			}
			else
			{
				Magick::PixelPacket *pixel = pixel_cache + y * aDataSet->getExtent( 0 ) + (aDataSet->getExtent( 0 ) - 1 - x);
				Magick::ColorGray actColor( static_cast<double>( (*aDataSet)( x, y ) ) / static_cast<double>( aDataSet->getMaximum() ) );
				*pixel = actColor;
			}
		}
	anImage.syncPixels();

	// Write image
	try
	{
		anImage.write( sFilename );
	}
	catch( std::exception& e )
	{
		alog << LWARN << e.what() << endl;
		throw( FileException( "Error on writing image file" ) );
	}
}

void CCommonImageHandler::saveImageSequence(  const std::string& sFilename, TImagePtr aDataSet )
	const throw( FileException )
{
	string actualFilename = sFilename;
	string::size_type extensionPos = sFilename.find_last_of( '.' );
	string sFileSuffix = sFilename.substr( extensionPos+1 );
	string sLargeSuffix = sFileSuffix;
	for ( uint i = 0; i < sLargeSuffix.length(); i++ )
    sLargeSuffix[i] = toupper( sLargeSuffix[i] );
  bool bAdjoinFrames = true;
	CoderInfo coderInfo( sLargeSuffix );
	if ( !coderInfo.isMultiFrame() )
	{
		cerr << LINFO << coderInfo.description()
			<< " doesn't support writing\n of image sequences. Writing images as single frames instead..." << endl;
		bAdjoinFrames = false;		
		actualFilename = sFilename.substr( 0, extensionPos );
		actualFilename += "%04d.";
		actualFilename += sFileSuffix;
	}
	list<Image> imageList;
	Image imageToWrite;
	for( uint z = aDataSet->getExtent( 2 ) - 1; z < aDataSet->getExtent( 2 ); --z )
	{
		Image anImage( Magick::Geometry( aDataSet->getExtent( 0 ), aDataSet->getExtent( 1 ) ), "black" );
		anImage.classType( Magick::DirectClass );
		anImage.type( Magick::TrueColorType );
		anImage.adjoin(true);
		anImage.quality( 100 );
		anImage.animationDelay( 10 );
		Magick::PixelPacket* pixel_cache = anImage.setPixels( 0, 0, aDataSet->getExtent(0), aDataSet->getExtent(1) );
		anImage.syncPixels();

		// Do the actual transformation
		for( uint y = 0; y < aDataSet->getExtent(1); ++y )
			for( uint x = 0; x < aDataSet->getExtent(0); ++x )
			{
				if ( aDataSet->getDataDimension() == 2 )
				{
					Magick::PixelPacket *pixel = pixel_cache + y * aDataSet->getExtent( 0 ) + (aDataSet->getExtent( 0 ) - 1 - x);
					Magick::ColorRGB actColor( 
						static_cast<double>( (*aDataSet)( x, y, z, 0 ) ) / static_cast<double>( aDataSet->getMaximum() ),
						static_cast<double>( (*aDataSet)( x, y, z, 1 ) ) / static_cast<double>( aDataSet->getMaximum() ), 0.0 );
					*pixel = actColor;
				}
				else if ( aDataSet->getDataDimension() > 2 )
				{
					Magick::PixelPacket *pixel = pixel_cache + y * aDataSet->getExtent( 0 ) + (aDataSet->getExtent( 0 ) - 1 - x);
					Magick::ColorRGB actColor( 
						static_cast<double>( (*aDataSet)( x, y, z, 0 ) ) / static_cast<double>( aDataSet->getMaximum() ),
						static_cast<double>( (*aDataSet)( x, y, z, 1 ) ) / static_cast<double>( aDataSet->getMaximum() ),
						static_cast<double>( (*aDataSet)( x, y, z, 2 ) ) / static_cast<double>( aDataSet->getMaximum() ) );
					*pixel = actColor;
				}			
				else
				{
					Magick::PixelPacket *pixel = pixel_cache + y * aDataSet->getExtent( 0 ) + (aDataSet->getExtent( 0 ) - 1 - x);
					Magick::ColorGray actColor( static_cast<double>( (*aDataSet)( x, y, z ) ) / static_cast<double>( aDataSet->getMaximum() ) );
					*pixel = actColor;
				}
			}
		anImage.syncPixels();
		imageList.push_back( anImage );		
	}
	try
	{				
		Magick::writeImages( imageList.begin(), imageList.end(), actualFilename, bAdjoinFrames );
	}
	catch( std::exception& e )
	{
		alog << LWARN << e.what() << endl;
		throw( FileException( "Error on writing image sequence file" ) );
	}
}

#else
#warning Image magick based common image formats handler not compiled
#endif
