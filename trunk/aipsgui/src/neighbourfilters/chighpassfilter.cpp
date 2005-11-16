/***********************************************************************
 * File: chighpassfilter.cpp                                           *
 * Project: AIPS - Neighbourhood filters plugin library                *
 * Description: A gaussian high pass filter in image space. Usage of   *
 *              the unsharp mask parameter is optional                 *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 2003-08-26                                                 *
 ***********************************************************************/

#include "chighpassfilter.h"

#ifdef USE_BLITZ
using namespace blitz;
#endif
using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CHighPassFilter::CHighPassFilter( ulong ulID ) throw()
  : CKernelFilter ( ulID, "Spatial highpass filter with unsharp mask", 1, 1, 
		"CHighPassFilter", "0.5", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Filters the input image with an unsharp mask filter\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Unsharp mask: Size of unsharp mask";

  parameters.initDouble( "UnsharpMask", 1.0, 1.0, 1000.0 );
}

CHighPassFilter::~CHighPassFilter() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CHighPassFilter::apply() throw()
{
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();
	
#ifdef USE_BLITZ
	TImagePtr outputPtr; outputPtr.reset();
  if ( inputPtr->getDimension() == 2 )
  {
    Array<double, 2> kernel( 3, 3 );
    kernel = -1.0 / 9.0;
    kernel( 1, 1 ) = ( 9.0 * parameters.getDouble( "Unsharp mask" ) - 1.0 ) / 9.0;
    outputPtr = applyKernel( inputPtr, kernel );
  }
  else
  {
    Array<double, 3> kernel( 3, 3, 3 );
    kernel = -1.0 / 27.0;
    kernel( 1, 1, 1 ) = ( 27.0 * parameters.getDouble( "Unsharp mask" ) - 1.0 ) / 27.0;
    outputPtr = applyKernel( inputPtr, kernel );
  }
#else
  size_t dims[] = { 3, 3, 3 };
  CTypedData<double> kernel( inputPtr->getDimension(), dims );
  if ( inputPtr->getDimension() == 2 )
  {
    kernel = -1.0/ 9.0;
    kernel( 1, 1 ) = ( 9.0 * parameters.getDouble( "Unsharp mask" ) - 1.0 ) / 9.0;
  }
  else
  {
    kernel = -1.0 / 27.0;
    kernel( 1, 1, 1 ) = ( 27.0 * parameters.getDouble( "Unsharp mask" ) - 1.0 ) / 27.0;
  }
  TImagePtr outputPtr = applyKernel( inputPtr, kernel );
#endif /* USE_STANDALONE */
  if ( outputPtr.get() == NULL )
  {
    alog << LWARN << SERROR( "invalid input data" ) << endl;
    return;
  }
  outputPtr->setDataRange( inputPtr->getDataRange() );
  setOutput( outputPtr );
FEND;
}

CPipelineItem* CHighPassFilter::newInstance( ulong ulID ) const throw()
{
  return new CHighPassFilter( ulID );
}
