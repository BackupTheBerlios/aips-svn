/***********************************************************************
 * File: cgrayleveltransformation.cpp                                  *
 * Project: AIPS                                                       *
 * Description: Base class for histogram-based transformations         *
 *                                                                     *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                   *
 *                                                                     *
 * Created: 2003-08-27                                                 *
 ***********************************************************************/

#include "cgrayleveltransformation.h"

using namespace std;
using namespace aips;
using namespace boost;

/**
 * Constructor
 * \param ulID_ Unique module ID (in processing graph)
 * \param sName_ Module name
 * \param usNoOfParameters_ Number of module parameters
 * \param usNoOfInputs_ Number of input ports
 * \param usNoOfOutputs_ Number of output ports
 */
CGrayLevelTransformation::CGrayLevelTransformation( ulong ulID_,
	const string &sName_,
  const ushort usNoOfInputs,
  const ushort usNoOfOutputs,
  const string &sClassName_, 
	const string &sClassVersion_,
	const string &sDerivedFrom_ ) throw()
	: CFilter ( ulID_, sName_, usNoOfInputs, usNoOfOutputs, sClassName_, sClassVersion_, sDerivedFrom_ )
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
void CGrayLevelTransformation::applyNewGrayValues ( TDataSetPtr aSourceSPtr,
  TDataSetPtr anOutputSPtr, ushort usChannel ) throw( NullException )
{
  if ( !anOutputSPtr || !aSourceSPtr )
    throw ( NullException( SERROR( "Got NULL reference from caller" ), CException::RECOVER ) );
	if ( aSourceSPtr->getType() == typeid ( TSmallImage::TDataType )
		&& anOutputSPtr->getType() == typeid ( TSmallImage::TDataType ) )
	{
		TSmallImagePtr aSmallInputImage = static_pointer_cast<TSmallImage>( aSourceSPtr );
		TSmallImagePtr aSmallOutputImage = static_pointer_cast<TSmallImage>( anOutputSPtr );
		compute<TSmallImage>( aSmallInputImage, aSmallOutputImage, usChannel );
	}
	else if ( aSourceSPtr->getType() == typeid ( TImage::TDataType )
		&& anOutputSPtr->getType() == typeid ( TImage::TDataType ) )
	{
		TImagePtr anInputImage = static_pointer_cast<TImage>( aSourceSPtr );
		TImagePtr anOutputImage = static_pointer_cast<TImage>( anOutputSPtr );
		compute<TImage>( anInputImage, anOutputImage, usChannel );
	}
	else
	{
		alog << LWARN << "Illegal input" << endl;
		return;
	}
}

template <typename ImageType>
void CGrayLevelTransformation::compute( boost::shared_ptr<ImageType> aSourceSPtr,
	boost::shared_ptr<ImageType> anOutputSPtr, ushort usChannel ) throw()
{
  size_t theImageSize = aSourceSPtr->getSize() / aSourceSPtr->getDataDimension();
	typename ImageType::iterator imageBeginIt = aSourceSPtr->begin();
	imageBeginIt += ( theImageSize * usChannel );
	typename ImageType::iterator imageEndIt = aSourceSPtr->begin();
	imageEndIt += ( theImageSize * ( usChannel + 1 ) );
  typename ImageType::iterator jt = anOutputSPtr->begin();
	jt += ( theImageSize * usChannel );
  
  for ( typename ImageType::iterator it = imageBeginIt; it != imageEndIt; ++it, ++jt )
  {
    *jt = usNewGrayValuesVec[(*it)];
  }
}
