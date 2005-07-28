/************************************************************************
 * File: ccentraldifference                                             *
 * Project: AIPS                                                        *
 * Description: Central difference gradient operator                    *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-05-10                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "ccentraldifference.h"

using namespace std;

CCentralDifference::CCentralDifference(ulong ulID_ ) throw()
  : CFilter(ulID_, "Gradient (Central-Difference)", 1, 1, "CCentralDifference", "0.1", "CFilter") 
{
  setModuleID( sLibID );
  
  sDocumentation = "Creates a gradient field from the input image via a sobel filter\n"
                   "** Input ports:\n"
                   "0: A scalar single channel 2D or 3D data set (image data, required)\n"
                   "** Output ports:\n"
                   "1: A 2D or 3D vector field which is normalized";

  inputsVec[0].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOVector;
}

CCentralDifference::~CCentralDifference() throw()
{
}

void CCentralDifference::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImage* inputPtr = static_cast<TImage*>( getInput().get() );
  if ( inputPtr == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 2 )
  {
    alog << LWARN << "Input type is no 2D image!" << endl;
    return;
  }
	bModuleReady = true;
	TImage& image = *inputPtr;
  deleteOldOutput();	
	boost::shared_ptr<TField2D> output ( new TField2D( 2, image.getExtents() ) );
	output->setMinimum( TVector2D( 0.0, 0.0 ) );
	output->setMaximum( TVector2D( 1.0, 1.0 ) );
	double dMaxGradient = 0.0;
	for( ushort y = 1; y < inputPtr->getExtent(1)-1; ++y )
		for ( ushort x = 1; x < inputPtr->getExtent(0)-1; ++x )
		{
			TVector2D vec;
			vec[0] = image(x-1,y) - image(x+1,y);
			vec[1] = image(x,y-1) - image(x,y+1);
			dMaxGradient = std::max( dMaxGradient, norm(vec) );
			(*output)(x,y) = vec;
		}
	for( TField2D::iterator it = output->begin(); it != output->end(); ++it )
	{
		(*it)/=dMaxGradient;
	}	
	setOutput( output );
BENCHSTOP;		
}

CPipelineItem* CCentralDifference::newInstance( ulong ulID ) const throw()
{
	return new CCentralDifference( ulID );
}
