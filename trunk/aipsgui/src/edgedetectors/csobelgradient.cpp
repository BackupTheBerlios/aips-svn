/************************************************************************
 * File: csobelgradient.cpp                                             *
 * Project: AIPS basic filters                                          *
 * Description: Sobel operator for gradient field generation            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 03.12.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "csobelgradient.h"

using namespace std;
using namespace boost;

/* Structors */

/**
 * \param ulID unique module ID
 */
CSobelGradient::CSobelGradient( ulong ulID ) throw()
: CFilter( ulID, "Sobel gradient vector field", 2, 1, "CSobelGradient", "0.4", "CFilter" ) 
{
  setModuleID( sLibID );
  
  sDocumentation = "Creates a gradient field from the input image via a sobel filter\n"
                   "** Input ports:\n"
                   "0: A scalar single channel 2D or 3D data set (image data, required)\n"
									 "1: A scalar single channel 2D or 3D data set "
									 "(region of interest, optional)\n"
                   "** Output ports:\n"
                   "1: A 2D or 3D vector field which is normalized\n"
									 "** Parameters:\n"
									 "3D Method: Choose filter kernel ( see code )";

	parameters.initBool( "3D Method", false );
  inputsVec[0].portType = CPipelineItem::IOInteger;
	inputsVec[1].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOVector;
}

CSobelGradient::~CSobelGradient() throw()
{
}

/* Other methods */

void CSobelGradient::apply() throw()
{
FBEGIN;
BENCHSTART;
	bModuleReady = false;
	if ( !getInput() || !checkType<TImage>( *getInput() ) )
		return;
	shared_ptr<TImage> inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !inputPtr || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }
	bModuleReady = true;
	bRoiSelf = false;
	if ( !getInput(1) || !checkType<TImage>( *getInput(1) ) )
	{
		bRoiSelf = true;
	}
	else
	{
		roiPtr = static_cast<TImage*>( getInput(1).get() );
		if ( roiPtr == NULL || roiPtr->getMaximum() > 1 )
			bRoiSelf = true;
	}

  deleteOldOutput();	
	if ( inputPtr->getDimension() == 2 )
		sobel2D();
	else 
	{
		sobel3D();		
	}
BENCHSTOP;		
FEND;
}

CPipelineItem* CSobelGradient::newInstance( ulong ulID ) const throw()
{
  return new CSobelGradient( ulID );
}

/* Private methods */

void CSobelGradient::sobel2D() throw()
{
FBEGIN;
cerr << "BBBBBBB" << endl;
	shared_ptr<TImage> inputPtr = static_pointer_cast<TImage>( getInput() );
  double dMaxGradient = 0.0;
 	ushort w = inputPtr->getExtent(0);
 	ushort h = inputPtr->getExtent(1);
	shared_ptr<TField2D> outputPtr ( new TField2D( 2, inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  outputPtr->setMaximum( TVector2D(  1.0,  1.0 ) );	
	outputPtr->setMinimum( TVector2D( -1.0, -1.0 ) );	
 	(*outputPtr) = VEC_ZERO2D;
PROG_MAX( h - 1 );	
	ushort x = 1; 
	ushort y = 1;
	TImage::iterator inputIt = inputPtr->begin();
	inputIt += w + 1;
	TImage::iterator roiIt;
	if ( !bRoiSelf ) 
	{
		roiIt = roiPtr->begin();
		roiIt += w + 1;
	}	
	TField2D::iterator outputIt = outputPtr->begin();
	outputIt += w + 1;	
	while( y < ( h - 1 ) )
	{
		if ( bRoiSelf || *roiIt )
		{
			(*outputIt)[0] = (*( inputIt + 1 - w )) + 2 * (*( inputIt + 1 )) + (*( inputIt + 1 + w ))
				- (*( inputIt - 1 - w )) - 2 * (*( inputIt - 1 )) - *(( inputIt - 1 + w ));
			(*outputIt)[1] = (*( inputIt - 1 + w )) + 2 * (*( inputIt + w )) + *(( inputIt + 1 + w ))
				- (*( inputIt - 1 - w )) - 2 * (*( inputIt - w )) - (*( inputIt + 1 - w ));
			dMaxGradient = std::max( dMaxGradient, norm(*outputIt) );
		}
		++inputIt; ++outputIt; ++roiIt; ++x;
		if ( x == ( w - 1 ) )
		{
			++y; x = 1; inputIt += 2; outputIt += 2; roiIt += 2;
			PROG_VAL( y );
		}
	}
APP_PROC();	
	for( TField2D::iterator outputIt = outputPtr->begin(); outputIt != outputPtr->end();
		++outputIt )
	{
		(*outputIt) /= dMaxGradient;
	}
PROG_RESET();
  setOutput( outputPtr );
  static_cast<TField2D*>( getOutput().get() );
FEND;  
}

void CSobelGradient::sobel3D() throw()
{
	shared_ptr<TImage> inputPtr = static_pointer_cast<TImage>( getInput() );
  double dMaxGradient = 0.0;
 	ushort w = inputPtr->getExtent(0);
 	ushort h = inputPtr->getExtent(1);
	ushort d = inputPtr->getExtent(2);
	shared_ptr<TField3D> outputPtr ( new TField3D( 3, inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  outputPtr->setMaximum( TVector3D( 1.0, 1.0, 1.0 ) );
	outputPtr->setMinimum( TVector3D( -1.0, -1.0, -1.0 ) );
	(*outputPtr) = VEC_ZERO3D;
PROG_MAX( d - 1 );	
	ushort x = 1; 
	ushort y = 1;
	ushort z = 1;
	TImage::iterator inputIt = inputPtr->begin();
	inputIt += w * h + w + 1;
	TImage::iterator roiIt;
	if ( !bRoiSelf ) 
	{
		roiIt = roiPtr->begin();
		roiIt += w * h + w + 1;
	}
	TField3D::iterator outputIt = outputPtr->begin();
	outputIt += w * h + w + 1;
	int slice = w * h;
	if ( !parameters.getBool( "3D Method" ) )
		while( z < ( d - 1 ) )
		{
			if ( bRoiSelf || *roiIt )
			{
				(*outputIt)[0] = (*( inputIt + 1 - w )) 
					+ 2 * (*( inputIt + 1 )) 
					+ (*( inputIt + 1 + w ))
					- (*( inputIt - 1 - w )) 
					- 2 * (*( inputIt - 1 )) 
					- (*( inputIt - 1 + w ));
				dMaxGradient = std::max( dMaxGradient, abs( (*outputIt)[0] ) );
			
				(*outputIt)[1] = (*( inputIt + w - slice )) 
					+ 2 * (*( inputIt + w )) 
					+ (*( inputIt + w + slice )) 
					- (*( inputIt - w - slice )) 
					- 2 * (*( inputIt - w )) 
					- (*( inputIt - w + slice ));
				dMaxGradient = std::max( dMaxGradient, abs( (*outputIt)[1] ) );
			
				(*outputIt)[2] = (*( inputIt - 1 + slice )) 
					+ 2 * (*( inputIt + slice )) 
					+ (*( inputIt + 1 + slice) ) 
					- (*( inputIt - 1 - slice )) 
					- 2 * (*( inputIt - slice )) 
					- (*( inputIt + 1 - slice) );
				dMaxGradient = std::max( dMaxGradient, abs( (*outputIt)[2] ) );
			}
			++inputIt; ++outputIt; ++roiIt; ++x;
			if ( x == ( w - 1 ) )
			{
				++y; x = 1; inputIt += 2; outputIt += 2; roiIt += 2;
			}
			if ( y == ( h - 1 ) )
			{
				++z; y = 1; inputIt += 2 * w; outputIt += 2 * w; roiIt += 2 * w;
				PROG_VAL( z );
				APP_PROC();
			}
		}	/* END WHILE */
	else
		while( z < ( d - 1 ) )
		{
			if ( bRoiSelf || *roiIt )
			{
				(*outputIt)[0] = (*(inputIt + 1 - w - slice ) )
				+ 2 * (*(inputIt + 1 - slice ) )
				+ (*(inputIt + 1 + w - slice ) )
				- (*(inputIt - 1 - w - slice ) )
				- 2 *(*(inputIt - 1 - slice ) )
				- (*(inputIt - 1 + w - slice ) )
				+ (*(inputIt + 1 - w + slice ) )
				+ 2 * (*(inputIt + 1 + slice ) )
				+ (*(inputIt + 1 + w + slice ) )
				- (*(inputIt - 1 - w + slice ) )
				- 2 * (*(inputIt - 1 + slice ) )
				- (*(inputIt - 1 + w + slice ) )
				+ 2 * (*(inputIt + 1 - w ) )
				+ 4 * (*(inputIt + 1 ) )
				+ 2 * (*(inputIt + 1 + w ) )
				- 2 * (*(inputIt - 1 - w ) )
				- 4 * (*(inputIt - 1 ) )
				- 2 * (*(inputIt - 1 + w ) );
				
				dMaxGradient = std::max( dMaxGradient, abs( (*outputIt)[0] ) );
			
				(*outputIt)[1] = (*(inputIt + 1 + w - slice ) )
				+ 2 * (*(inputIt + 1 + w - slice ) )
				+ (*(inputIt + 1 + w - slice ) )
				- (*(inputIt + 1 - w - slice ) )
				- 2 * (*(inputIt + 1 - w + slice ) )
				- (*(inputIt + 1 - w - slice ) )
				+ (*(inputIt - 1 + w - slice ) )
				+ 2 * (*(inputIt - 1 + w ) )
				+ (*(inputIt - 1 + w + slice ) )
				- (*(inputIt - 1 - w - slice ) )
				- 2 * (*(inputIt - 1 - w ) )
				- (*(inputIt - 1 - w + slice ) )
				+ 2 * (*(inputIt + w - slice ) )
				+ 4 * (*(inputIt + w ) )
				+ 2 * (*(inputIt + w + slice ) )
				- 2 *(*(inputIt - w - slice ) )
				- 4 * (*(inputIt - w ) )
				- 2 * (*(inputIt - w + slice ) );
				
				dMaxGradient = std::max( dMaxGradient, abs( (*outputIt)[1] ) );
				
				(*outputIt)[2] = (*(inputIt - 1 - w - slice ) )
				+ 2 * (*(inputIt  - w - slice ) )
				+ (*(inputIt + 1 - w - slice ) )
				- (*(inputIt - 1 - w + slice ) )
				- 2 * (*(inputIt - w + slice ) )
				- (*(inputIt + 1 - w + slice ) )
				+ (*(inputIt - 1 + w - slice ) )
				+ 2 * (*(inputIt + w - slice ) )
				+ (*(inputIt + 1 + w - slice ) )
				- (*(inputIt - 1 + w + slice ) )
				- 2 * (*(inputIt + w + slice ) )
				- (*(inputIt + 1 + w + slice ) )
				+ 2 * (*(inputIt - 1 - slice ) )
				+ 4 * (*(inputIt - slice ) )
				+ 2 * (*(inputIt + 1 - slice ) )
				- 1 * (*(inputIt - 1 + slice ) )
				- 2 * (*(inputIt + slice ) )
				- 1 * (*(inputIt + 1 + slice ) );				
				
				dMaxGradient = std::max( dMaxGradient, abs( (*outputIt)[2] ) );
			}
			++inputIt; ++outputIt; ++roiIt; ++x;
			if ( x == ( w - 1 ) )
			{
				++y; x = 1; inputIt += 2; outputIt += 2; roiIt += 2;
			}
			if ( y == ( h - 1 ) )
			{
				++z; y = 1; inputIt += 2 * w; outputIt += 2 * w; roiIt += 2 * w;
				PROG_VAL( z );
				APP_PROC();
			}
		} /* end WHILE */	
			
	for( TField3D::iterator outputIt = outputPtr->begin(); outputIt != outputPtr->end();
		++outputIt )
	{
		(*outputIt) /= dMaxGradient;
	}
	PROG_RESET();	
	
	setOutput( outputPtr );
}

