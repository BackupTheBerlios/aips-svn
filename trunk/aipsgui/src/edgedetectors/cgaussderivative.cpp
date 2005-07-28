/************************************************************************
 * File: cgaussderivative.cpp                                           *
 * Project: AIPS - Edge detectors plugin library                        *
 * Description: Edge detector using the 2nd derivative of the gaussian  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-06-23                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cgaussderivative.h"

using namespace std;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CGaussDerivative::CGaussDerivative( ulong ulID ) throw()
: CFilter( ulID, "Gauss gradient", 2, 1, "CGaussDerivative", "0.2", "CFilter" ) 
{
  setModuleID( sLibID );
  
  sDocumentation = "Creates a gradient field from the input image via a gaussian derivative\n"
                   "** Input ports:\n"
                   "0: A scalar single channel data set (image data, required)\n"
									 "1: A scalar single channel data set (region of interest, optional)\n"
                   "** Output ports:\n"
                   "0: A 2D or 3D vector field which is normalized\n"
									 "** Parameters:\n"
									 "Sigma: Width of gaussian";

	parameters.initDouble( "Sigma", 0.5, 0.0, 100.0 );
	
  inputsVec[0].portType = CPipelineItem::IOInteger;
	inputsVec[1].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOVector;
}

CGaussDerivative::~CGaussDerivative() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CGaussDerivative::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
cerr << "A" << endl;	
  TImage* inputPtr = static_cast<TImage*>( getInput().get() );
  if ( inputPtr == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
cerr << "B" << endl;
	bModuleReady = true;
	bRoiSelf = false;
	roiPtr = static_cast<TImage*>( getInput(1).get() );
	if ( roiPtr == NULL || roiPtr->getMaximum() > 1 )
	{
		bRoiSelf = true;
	}
cerr << "C" << endl;
  deleteOldOutput();	
	if ( inputPtr->getDimension() == 2 )
		gauss2D();
	else 
	{
		gauss3D();		
	}
cerr << "D" << endl;
BENCHSTOP;		
}

CPipelineItem* CGaussDerivative::newInstance( ulong ulID ) const throw()
{
  return new CGaussDerivative( ulID );
}

/*******************
 * Private methods *
 *******************/

/** 
 * \param iMaskSize reference to variable to store the computed mask size in
 * \returns 1D mask for the given Sigma 
 */
vector<double> CGaussDerivative::computeMask( int& iMaskSize ) throw()
{
	double dSigma = parameters.getDouble( "Sigma" );
	iMaskSize = static_cast<uint>( floor( 6.0 * dSigma + 0.5 ) ) + 1;
	double dDenominator = dSigma * dSigma;
	vector<double> dGaussMask;
	for( int i = -( iMaskSize / 2 ); i <= ( iMaskSize / 2 ); ++i )
	{
		double dDistance = static_cast<double>( i * i );
		double dExponential = exp( -dDistance / ( 2.0 * ( dSigma * dSigma ) ) );
		dGaussMask.push_back( ( dExponential * -i ) / dDenominator );
	}
	return dGaussMask;
}

void CGaussDerivative::gauss2D() throw()
{
cerr << "Here" << endl;
	TImage* inputPtr = static_cast<TImage*>( getInput().get() );
  double dMaxGradient = 0.0;
 	ushort w = inputPtr->getExtent( 0 );
 	ushort h = inputPtr->getExtent( 1 );
	boost::shared_ptr<TField2D> outputPtr ( 
		new TField2D( 2, inputPtr->getExtents(), inputPtr->getDataDimension() ) );	
  outputPtr->setMaximum( TVector2D(  1.0,  1.0 ) );	
	outputPtr->setMinimum( TVector2D( -1.0, -1.0 ) );	
 	(*outputPtr) = VEC_ZERO2D;
	
	int iMaskSize = 0;
	vector<double> dGaussMask = computeMask( iMaskSize );
	
	// Determine gradient field
	PROG_MAX( h );	
	for( ushort y = ( iMaskSize / 2 ); y < h - ( iMaskSize / 2 ); ++y )
	{
		PROG_VAL( y );
		for( ushort x = ( iMaskSize / 2 ); x < w - ( iMaskSize / 2 ); ++x )
		{	
			if ( bRoiSelf || (*roiPtr)( x, y ) )	
			{
				TVector2D gradVec ( 0.0, 0.0 );
				for( short usMaskX = -( iMaskSize / 2 ); usMaskX <= ( iMaskSize / 2 ); ++usMaskX  )
					gradVec[0] += (*inputPtr)( x - usMaskX, y ) * dGaussMask[ usMaskX + ( iMaskSize / 2 ) ];
				for( short usMaskY = -( iMaskSize / 2 ); usMaskY <= ( iMaskSize / 2 ); ++usMaskY  )
					gradVec[1] += (*inputPtr)( x, y - usMaskY ) * dGaussMask[ usMaskY + ( iMaskSize / 2 ) ];			
				(*outputPtr)( x, y ) = gradVec;
				dMaxGradient = std::max( dMaxGradient, norm( gradVec ) );
			}
		}
	}
	APP_PROC();	
	
	// Normalize field	
	for( TField2D::iterator outputIt = outputPtr->begin(); 
		outputIt != outputPtr->end();	++outputIt )
	{
		(*outputIt) /= dMaxGradient;
		if ( norm(*outputIt) < numeric_limits<double>::epsilon() )
		{
			(*outputIt) = 0.0;	
		}
	}
	PROG_RESET();
  setOutput( outputPtr );
cerr << "Done" << endl;  
}

void CGaussDerivative::gauss3D() throw()
{
	TImage* inputPtr = static_cast<TImage*>( getInput().get() );
  double dMaxGradient = 0.0;
 	ushort w = inputPtr->getExtent( 0 );
 	ushort h = inputPtr->getExtent( 1 );
	ushort d = inputPtr->getExtent( 2 );
	boost::shared_ptr<TField3D> outputPtr ( new TField3D( 3, inputPtr->getExtents(), 
		inputPtr->getDataDimension() ) );
  outputPtr->setMaximum( TVector3D( 1.0, 1.0, 1.0 ) );
	outputPtr->setMinimum( TVector3D( -1.0, -1.0, -1.0 ) );
	(*outputPtr) = VEC_ZERO3D;
	
	int iMaskSize = 0;
	vector<double> dGaussMask = computeMask( iMaskSize );
	
	// Determine gradient field
	PROG_MAX( d );	
	for( ushort z = ( iMaskSize / 2 ); z < d - ( iMaskSize / 2 ); ++z )
	{
		PROG_VAL( z );
		APP_PROC();
		for( ushort y = ( iMaskSize / 2 ); y < h - ( iMaskSize / 2 ); ++y )
			for( ushort x = ( iMaskSize / 2 ); x < w - ( iMaskSize / 2 ); ++x )
			{	
				if ( bRoiSelf || (*roiPtr)( x, y, z ) )	
				{
					TVector3D gradVec ( 0.0, 0.0, 0.0 );
					for( short usMaskX = -( iMaskSize / 2 ); usMaskX <= ( iMaskSize / 2 ); ++usMaskX  )
						gradVec[0] += (*inputPtr)( x - usMaskX, y, z ) * dGaussMask[ usMaskX + ( iMaskSize / 2 ) ];
					for( short usMaskY = -( iMaskSize / 2 ); usMaskY <= ( iMaskSize / 2 ); ++usMaskY  )
						gradVec[1] += (*inputPtr)( x, y - usMaskY, z ) * dGaussMask[ usMaskY + ( iMaskSize / 2 ) ];			
					for( short usMaskZ = -( iMaskSize / 2 ); usMaskZ <= ( iMaskSize / 2 ); ++usMaskZ  )
						gradVec[2] += (*inputPtr)( x, y, z - usMaskZ ) * dGaussMask[ usMaskZ + ( iMaskSize / 2 ) ];			
					(*outputPtr)( x, y, z ) = gradVec;
					dMaxGradient = std::max( dMaxGradient, norm( gradVec ) );
				}
			}
	}
				
	// Normalize field
	for( TField3D::iterator outputIt = outputPtr->begin(); 
		outputIt != outputPtr->end();	++outputIt )
	{
		(*outputIt) /= dMaxGradient;
		if ( norm(*outputIt) < numeric_limits<double>::epsilon() )
		{
			(*outputIt) = 0.0;	
		}
	}
	
	PROG_RESET();		
	setOutput( outputPtr );
}




