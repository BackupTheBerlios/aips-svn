/***********************************************************************
 * File: chistogram.h                                                  *
 * Project: AIPS                                                       *
 * Description: A class for histogram calculation                      *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 28.08.03                                                   *
 ***********************************************************************/
 
#include "chistogram.h"

using namespace std;
using namespace aips;

/** Constructor */
CHistogram::CHistogram() throw()
  : CSubject( "CHistogram", "0.6", "CBase" )
{
}

/** Destructor */
CHistogram::~CHistogram() throw()
{
  for ( ushort i = 0; i < histogramVec.size(); i++ )
    histogramVec[i].clear();
  histogramVec.clear();
}

/**
 * Calculates the image histogram
 * \param sourcePtr the image to calculate the histogram from
 * \exception NullException if parameter sourcePtr is NULL
 */
void CHistogram::calculateHistogram( TImagePtr sourcePtr )
  throw( NullException )
{
FBEGIN;
BENCHSTART;
	if ( sourcePtr.get() == NULL )
    throw ( NullException( SERROR( "Got NULL reference from caller" ), CException::RECOVER ) );

  dMaxValue = -1.0;
  size_t intensitySize = sourcePtr->getDataRange().getMaximum();
  std::vector<ulong> noOfPixelsVec( intensitySize + 1 );
  histogramVec.resize( sourcePtr->getDataDimension() );
  size_t imageSize = sourcePtr->getExtent( 0 ) * sourcePtr->getExtent( 1 );
  if ( sourcePtr->getDimension() == 3 ) imageSize *= sourcePtr->getExtent( 2 );
  
	PROG_MAX( sourcePtr->getDataDimension() );
  for ( ushort usChannels = 0; usChannels < sourcePtr->getDataDimension(); usChannels++ )
  {
		PROG_VAL( usChannels + 1 );
    histogramVec[usChannels].resize( intensitySize + 1 );
    for ( ulong i = 0; i <= intensitySize; i++ )
     noOfPixelsVec[i] = 0;
		TImage::iterator ib = sourcePtr->begin();
		ib += (imageSize * usChannels);
		TImage::iterator ie = sourcePtr->begin();
		ie += (imageSize * ( usChannels + 1 ) );

    for ( TImage::iterator it = ib; it != ie; ++it )
    {		
      noOfPixelsVec[*it]++;
    }
    for ( ulong i = 0; i <= intensitySize; i++ )
    {
      histogramVec[usChannels][i] =
        static_cast<double>( noOfPixelsVec[i] ) / static_cast<double>( imageSize );
      if ( histogramVec[usChannels][i] > dMaxValue )
        dMaxValue = histogramVec[usChannels][i];
    }
  }
	PROG_RESET();
  noOfPixelsVec.clear();
BENCHSTOP;
FEND;
}
