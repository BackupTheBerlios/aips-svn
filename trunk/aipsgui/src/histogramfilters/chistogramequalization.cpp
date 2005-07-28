/***********************************************************************
 * File: chistogramequalization.cpp                                    *
 * Project: QtVis                                                      *
 * Description: A histogram equalization filter                        *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 28.08.03                                                   *
 ***********************************************************************/
 
#include "chistogramequalization.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
/** \param ulID unique module ID */
CHistogramEqualization::CHistogramEqualization( ulong ulID ) throw()
  : CGrayLevelTransformation ( ulID, "Histogram equalization", 1, 1,
	"CHistogramEqualization", "0.5", "CHistogram" )
{
  setModuleID( sLibID );
  
  sDocumentation = "Equalizes the histogram of each channel in the given image or volume\n"
                   "** Input ports:\n"
                   "0: A scalar multidimensional, multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar multidimensional, multichannel data set";  
}

CHistogramEqualization::~CHistogramEqualization() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CHistogramEqualization::apply () throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << "Input type is no image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), inputPtr->getExtents(),
    inputPtr->getDataDimension() ) );
  outputPtr->setMinimum( inputPtr->getMinimum() );
  outputPtr->setMaximum( inputPtr->getMaximum() );
	
  calculateHistogram( inputPtr );
	
  ushort usMaxIntensity = ( inputPtr->getMaximum() );
  usNewGrayValuesVec.resize( inputPtr->getMaximum() + 1 );
	
	//CHistogram::PROG_MAX( inputPtr->getDataDimension() );
  for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); usChannel++ )
  {
	//	CHistogram::PROG_VAL( usChannel + 1 );
		APP_PROC();
    for ( ushort i = 0; i <= usMaxIntensity; i++ )
    {
      double dSum = 0.0;
      for ( int j = 0; j <= i; j++ )
        dSum += histogramVec[usChannel][j];
      ushort usVal = static_cast<ushort>( dSum * static_cast<double>(usMaxIntensity) );
      usNewGrayValuesVec[i] = usVal;
    }
    applyNewGrayValues( inputPtr, outputPtr, usChannel );
  }
  
  setOutput( outputPtr );
//	CHistogram::PROG_RESET();
BENCHSTOP;
}

CPipelineItem* CHistogramEqualization::newInstance( ulong ulID ) const throw()
{
  return new CHistogramEqualization( ulID );
}

