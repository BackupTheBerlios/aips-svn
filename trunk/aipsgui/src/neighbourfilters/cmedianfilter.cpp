/***********************************************************************
 * File: cmedianfilter.h                                               *
 * Project: AIPS neighbourhood filters plugin library                  *
 * Description: Median filter with variable window size                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 26.08.03                                                   *
 ***********************************************************************/

#include "cmedianfilter.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CMedianFilter::CMedianFilter( ulong ulID ) throw()
  : CFilter ( ulID, "Median filter", 1, 1, "CMedianFilter", "0.4", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Filters the input image with a median filter\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";

  parameters.initUnsignedLong( "Radius", 1UL, 1UL, 1000UL );
}

CMedianFilter::~CMedianFilter() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CMedianFilter::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[3];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  dimensionSize[2] = ( inputPtr->getDimension() == 3 ) ? inputPtr->getExtent(2) : 1;
  
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  (*outputPtr) = (*inputPtr);

  uint uiMaskSize = static_cast<uint>( ( 2 * parameters.getUnsignedLong("Radius") ) + 1 );  
  vector<ushort> usMask( uiMaskSize * uiMaskSize * uiMaskSize );

  ulong ulRadius =  parameters.getUnsignedLong("Radius");
  PROG_MAX( inputPtr->getDataDimension() * dimensionSize[2] );
  for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
  if ( inputPtr->getDimension() == 3 )
  {	
    for ( ushort z = ulRadius; z < ( dimensionSize[2] - ulRadius ); ++z )
    {
      APP_PROC();
			PROG_VAL( z * ( usChannel + 1 ) );
      for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
        for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
        {
          ushort usMaxIndex = 0;
          for ( ushort k =  x - ulRadius; k < ( x + ulRadius + 1 ); ++k )
            for ( ushort l =  y - ulRadius; l < ( y + ulRadius + 1 ); ++l )
              for ( ushort m =  z - ulRadius; m < ( z + ulRadius + 1 ); ++m )
              {
                usMask[usMaxIndex] = (*inputPtr)( k, l, m, usChannel );
                usMaxIndex++;
              }
          vector<ushort>::iterator it = usMask.begin() + usMaxIndex + 1;
          sort( usMask.begin(), it );
          (*outputPtr)( x, y, z, usChannel ) = usMask[usMaxIndex / 2 + 1];
        }
    }
  }
  else
  {
    for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
    {
      APP_PROC();
			PROG_VAL( usChannel + 1 );
      for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
        for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
          {
            ushort usMaxIndex = 0;
            for ( ushort k =  x - ulRadius; k < ( x + ulRadius + 1 ); ++k )
              for ( ushort l =  y - ulRadius; l < ( y + ulRadius + 1 ); ++l )
                {
                  usMask[usMaxIndex] = (*inputPtr)( k, l, usChannel );
                  usMaxIndex++;
                }                    
            vector<ushort>::iterator it = usMask.begin() + usMaxIndex + 1;
            sort( usMask.begin(), it );
            (*outputPtr)( x, y, usChannel ) = usMask[usMaxIndex / 2 + 1];
          }
    }
  }
	PROG_RESET();
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CMedianFilter::newInstance( ulong ulID ) const throw()
{
  return new CMedianFilter( ulID );
}
