/***********************************************************************
 * File: cbinarization.cpp                                             *
 * Project: AIPS                                                       *
 * Description: Creates a thresholded binary image from a graylevel    *
 *              input image                                            *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 27.08.03                                                   *
 ***********************************************************************/

#include "cbinarization.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CBinarization::CBinarization( ulong ulID ) throw()
  : CGrayLevelTransformation ( ulID, "Image binarization", 1, 1, "CBinarization", "0.4", "CGrayLevelTransformation" )
{
  setModuleID( sLibID );
  sDocumentation = "Creates a thresholded binary image\n"
                   "** Input ports:\n"
                   "0: A singlechannel image\n"
                   "** Output ports:\n"
                   "1: A singlechannel image\n"
                   "** Parameters:\n"
                   "Threshold: Binary intensity threshold";

  parameters.initUnsignedLong( "Threshold", 0UL, 0UL, 65535UL );

  inputsVec[0].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOInteger;  
}

CBinarization::~CBinarization() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CBinarization::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDataDimension() > 1 )
  {
    alog << LWARN << "Input type is no single-channel image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), inputPtr->getExtents(), 1 ) );
  outputPtr->setMaximum( 1 );
  outputPtr->setMinimum( 0 );
  usNewGrayValuesVec.resize( inputPtr->getMaximum()+1 );
  // First calculate the new gray values
  for ( ushort i = 0; i <= inputPtr->getMaximum(); i++ )
    if ( i < parameters.getUnsignedLong("Threshold") )
      usNewGrayValuesVec[i] = 0;
    else
      usNewGrayValuesVec[i] = 1;
  // Now apply the new gray values
  applyNewGrayValues( inputPtr, outputPtr );
    
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CBinarization::newInstance( ulong ulID ) const throw()
{
  return new CBinarization( ulID );
}

