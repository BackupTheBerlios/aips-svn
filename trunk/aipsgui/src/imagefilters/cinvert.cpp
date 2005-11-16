/***********************************************************************
 * File: cinvert.cpp                                                   *
 * Project: AIPS - Image filters plugin library                        *
 * Description: Inverts the whole image                                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 28.08.03                                                   *
 ***********************************************************************/
#include "cinvert.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CInvert::CInvert( ulong ulID ) throw()
   : CFilter ( ulID, "Invert image", 1, 1, "CInvert","0.4","CFilter" )
{
  setModuleID( sLibID );
}

CInvert::~CInvert() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CInvert::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << "Input type is no 2D/3D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), inputPtr->getExtents(), 
		inputPtr->getDataDimension() ) );
  outputPtr->setDataRange( inputPtr->getDataRange() );

  ushort usMaxIntensity = inputPtr->getDataRange().getMaximum();
	TImage::iterator inputIt = inputPtr->begin();
	TImage::iterator outputIt = outputPtr->begin();
	while( inputIt != inputPtr->end() )
	{
  	(*outputIt) = usMaxIntensity - (*inputIt);
		++outputIt;
		++inputIt;
	}
    
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CInvert::newInstance( ulong ulID ) const throw()
{
  return new CInvert( ulID );
}

