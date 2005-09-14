/***********************************************************************
 * File: clowpassfilter.h                                              *
 * Project: AIPS neighbourhood filters plugin library                  *
 * Description: Uniform smoothing filter in image space                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 2003-08-26                                                 *
 ***********************************************************************/

#include "clowpassfilter.h"

using namespace std;
using namespace boost;
#ifdef USE_BLITZ
using namespace blitz;
#endif

/**
 * Constructor
 * \param ulID unique module ID
 */
CLowPassFilter::CLowPassFilter( ulong ulID ) throw()
  : CKernelFilter ( ulID, "Uniform smoothing filter in image space", 1, 1,
		"CLowPassFilter", "0.5", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Filters the input image with uniform smoothing filter\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";

  parameters.initUnsignedLong( "Radius", 1UL, 1UL, 1000UL );
}

/** Destructor */
CLowPassFilter::~CLowPassFilter() throw()
{
}

/** Reimplemented from CPipelineItem */
void CLowPassFilter::apply() throw()
{
FBEGIN;
  ushort usRadius = 1 + 2 * parameters.getUnsignedLong("Radius"); // Determine filter radius
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D image or 3D volume!") << endl;
FEND;    
    return;
  }
	bModuleReady = true;
  deleteOldOutput();
	TImagePtr outputPtr; 
#ifdef USE_BLITZ
  if ( getInput()->getDimension() == 2 ) // Got 2D image
  {
    Array<double, 2> kernel( usRadius, usRadius );
    kernel = 1.0 / ( usRadius * usRadius );
    outputPtr = applyKernel( inputPtr, kernel );
  }
  else if ( getInput()->getDimension() == 3 )
  {
    Array<double, 3> kernel( usRadius, usRadius, usRadius );
    kernel = 1.0 / ( usRadius * usRadius * usRadius );
    outputPtr = applyKernel( inputPtr, kernel );
  }
#else /* USE_STANDALONE */
  size_t dims[] = { usRadius, usRadius, usRadius };
  CTypedData<double> kernel( inputPtr->getDimension(), dims );
  if ( inputPtr->getDimension() == 2 )
    kernel = 1.0 / ( usRadius * usRadius );
  else
    kernel = 1.0 / ( usRadius * usRadius * usRadius );
  outputPtr = applyKernel( inputPtr, kernel );
#endif /* USE_STANDALONE */
  if ( !outputPtr )
  {
    alog << LWARN << SERROR("invalid input data") << endl;
FEND;    
    return;
  }
  setOutput( outputPtr );
FEND;  
}

/** Reimplemented from CPipelineItem */
CPipelineItem* CLowPassFilter::newInstance( ulong ulID ) const throw()
{
  return new CLowPassFilter( ulID );
}

