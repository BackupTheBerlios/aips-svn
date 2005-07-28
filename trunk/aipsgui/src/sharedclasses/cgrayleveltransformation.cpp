/***********************************************************************
 * File: cgrayleveltransformation.h                                    *
 * Project: AIPS                                                       *
 * Description: An abstract base class for all intensity based filters *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 27.08.03                                                   *
 ***********************************************************************/

#include "cgrayleveltransformation.h"

using namespace std;
using namespace aips;

/**
 * Constructor
 * \param ulID_ Unique module ID (in processing graph)
 * \param sName_ Module name
 * \param usNoOfParameters_ Number of module parameters
 * \param usNoOfInputs_ Number of input ports
 * \param usNoOfOutputs_ Number of output ports
 */
CGrayLevelTransformation::CGrayLevelTransformation( ulong ulID_, const string &sName_,
  const ushort usNoOfInputs, const ushort usNoOfOutputs, const string &sClassName_, 
	const string &sClassVersion_, const string &sDerivedFrom_ )
  throw() : CFilter ( ulID_, sName_, usNoOfInputs, usNoOfOutputs, sClassName_, sClassVersion_, sDerivedFrom_ )
{
  inputsVec[0].portType = IOInteger;
  outputsVec[0].portType = IOInteger;
}

/** Destuctor */
CGrayLevelTransformation::~CGrayLevelTransformation() throw()
{
  usNewGrayValuesVec.clear();
}

/**
 * Applies the new gray values to the given image. Each pixel will be transformed
 * according to usNewGrayValuesVec.
 * \param sourcePtr source image
 * \param outputPtr transformed target image (this must already have been instatiated!)
 * \param usChannel image channel to transform
 */
void CGrayLevelTransformation::applyNewGrayValues ( TImagePtr sourcePtr,
  TImagePtr outputPtr, ushort usChannel ) throw( NullException )
{
  if ( outputPtr == NULL || sourcePtr == NULL )
    throw ( NullException( SERROR( "Got NULL reference from caller" ), CException::RECOVER ) );

  size_t imageSize = sourcePtr->getArraySize() / sourcePtr->getDataDimension();
  TImage::iterator jt = outputPtr->begin();
	jt += (imageSize * usChannel);
	TImage::iterator ib = sourcePtr->begin();
	ib += (imageSize * usChannel);
	TImage::iterator ie = sourcePtr->begin();
	ie += (imageSize * ( usChannel + 1 ) );
  for ( TImage::iterator it = ib; it != ie; ++it )
  {
    *jt = usNewGrayValuesVec[(*it)];
    ++jt;
  }
}

