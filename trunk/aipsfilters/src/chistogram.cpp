/***********************************************************************
 * File: chistogram.h                                                  *
 * Project: AIPS                                                       *
 * Description: A class for histogram calculation                      *
 *                                                                     *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de                    *
 *                                                                     *
 * Created: 28.08.03                                                   *
 ***********************************************************************/
 
#include "chistogram.h"

using namespace std;
using namespace boost;
using namespace boost::lambda;
using namespace aips;

/** Constructor */
CHistogram::CHistogram() throw()
  : CSubject( "CHistogram", CHISTOGRAM_VERSION, "CBase" )
{
}

/** Destructor */
CHistogram::~CHistogram() throw()
{
  for ( ushort i = 0; i < theHistogramVec.size(); i++ )
    theHistogramVec[i].clear();
  theHistogramVec.clear();
}

/**
 * Calculates the image histogram
 * \param theSourceSPtr the image to calculate the histogram from
 * \exception NullException if parameter theSourceSPtr is NULL
 */
void CHistogram::calculateHistogram( TDataSetPtr theSourceSPtr ) throw( NullException )
{
FBEGIN;
BENCHSTART;
	if ( !theSourceSPtr )
    throw ( NullException( SERROR( "Got NULL reference from caller" ), CException::RECOVER ) );
	if ( theSourceSPtr->getType() == typeid ( TSmallImage::TDataType ) )
	{
	  TSmallImagePtr aSmallImage = static_pointer_cast<TSmallImage>( theSourceSPtr );
		compute<TSmallImage>( aSmallImage );
	}
	else if ( theSourceSPtr->getType() == typeid ( TImage::TDataType ) )
	{
		TImagePtr anImage = static_pointer_cast<TImage>( theSourceSPtr );
		compute<TImage>( anImage );
	}
	else if ( theSourceSPtr->getType() == typeid ( TField::TDataType ) )
	{
		TFieldPtr aField = static_pointer_cast<TField>( theSourceSPtr );
		compute<TField>( aField );
	}
	else
	{
		alog << LWARN << "Illegal input" << endl;
		return;
	}
BENCHSTOP;
FEND;
}

template<typename ImageType>
void CHistogram::compute( shared_ptr<ImageType> theImageSPtr ) throw()
{
	dMaxValue = -1.0;
  size_t theIntensitySize = static_cast<size_t>( ceil(
  	static_cast<double>( theImageSPtr->getDataRange().getMaximum() ) ) );
  std::vector<ulong> theNoOfPixelsVec( theIntensitySize + 1 );
  theHistogramVec.resize( theImageSPtr->getDataDimension() );
  size_t theImageSize = theImageSPtr->getSize();
DBG3("Image max. intensity " << theIntensitySize
	<< ", size " << theImageSize << ", histovecsize " << theHistogramVec.size() );
  
  for ( ushort usChannels = 0; usChannels < theImageSPtr->getDataDimension(); usChannels++ )
  {
   	theHistogramVec[usChannels].resize( theIntensitySize + 1 );
    for_each( theNoOfPixelsVec.begin(), theNoOfPixelsVec.end(), _1 = 0 );
		typename ImageType::iterator channelBeginIt = theImageSPtr->begin();
		channelBeginIt += ( theImageSize * usChannels );
		typename ImageType::iterator channelEndIt = theImageSPtr->begin();
		channelEndIt += ( theImageSize * ( usChannels + 1 ) );
    for ( typename ImageType::iterator it = channelBeginIt; it != channelEndIt; ++it )
    {
      theNoOfPixelsVec[static_cast<uint>( round(*it) )]++;
    }
    for ( ulong i = 0; i <= theIntensitySize; i++ )
    {
      theHistogramVec[usChannels][i] = static_cast<double>( theNoOfPixelsVec[i] )
        	/ static_cast<double>( theImageSize );
      if ( theHistogramVec[usChannels][i] > dMaxValue )
        dMaxValue = theHistogramVec[usChannels][i];
    }
  }
  theNoOfPixelsVec.clear();
}
