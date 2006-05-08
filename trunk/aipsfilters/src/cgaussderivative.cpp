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
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CGaussDerivative::CGaussDerivative( ulong ulID ) throw()
: CFilter( ulID, "Gauss gradient", 2, 1, "CGaussDerivative", CGAUSSDERIVATIVE_VERSION, "CFilter" ) 
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

DEFINE_CALL_MACRO( CGaussDerivative::call2D, CGaussDerivative::gauss2D, imageTL )
DEFINE_CALL_MACRO( CGaussDerivative::call3D, CGaussDerivative::gauss3D, imageTL )

void CGaussDerivative::apply() throw()
{
FBEGIN;
BENCHSTART;
	bModuleReady = false;
	if ( getInput() && getInput()->getDimension() == 2 )
		call2D<Length<imageTL>::value - 1>();
	else if ( getInput() && getInput()->getDimension() == 3 )
		call3D<Length<imageTL>::value - 1>();
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
BENCHSTOP;
FEND;
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

template<typename ImageType>
bool CGaussDerivative::gauss2D() throw()
{
FBEGIN;
	// Initialisation
  shared_ptr<ImageType> inputSPtr = static_pointer_cast<ImageType>( getInput() );
  if ( !checkInput<ImageType>( inputSPtr, 2, 2 ) )
  	return false;
	bModuleReady = true;
	bRoiSelf = false; // By default, a ROI is given by the user
	TSmallImagePtr roiSPtr = static_pointer_cast<TSmallImage>( getInput(1) );
	if ( !checkInput<TSmallImage>( roiSPtr, 2, 2 ) || roiSPtr->getDataRange().getMaximum() < 1 )
	{
		bRoiSelf = true;
	}
  deleteOldOutput();	

  double dMaxGradient = 0.0;

	TField2DPtr outputSPtr ( 
		new TField2D( 2, inputSPtr->getExtents(), inputSPtr->getDataDimension() ) );	
  outputSPtr->setMaximum( 1.0 );	
	outputSPtr->setMinimum( -1.0 );	
 	(*outputSPtr) = VEC_ZERO2D;
	
	int iMaskSize = 0;
	vector<double> dGaussMask = computeMask( iMaskSize );
	
	// Determine gradient field
	for( ushort d = 0; d < inputSPtr->getDataDimension(); ++d )
		for( ushort y = ( iMaskSize / 2 ); y < inputSPtr->getExtent( 1 ) - ( iMaskSize / 2 ); ++y )
			for( ushort x = ( iMaskSize / 2 ); x < inputSPtr->getExtent( 0 ) - ( iMaskSize / 2 ); ++x )
				if ( bRoiSelf || (*roiSPtr)( x, y ) )	
				{
					TVector2D gradVec ( 0.0, 0.0 );
					for( short usMaskX = -( iMaskSize / 2 ); usMaskX <= ( iMaskSize / 2 ); ++usMaskX  )
						gradVec[0] += (*inputSPtr)( x - usMaskX, y, d ) * dGaussMask[ usMaskX + ( iMaskSize / 2 ) ];
					for( short usMaskY = -( iMaskSize / 2 ); usMaskY <= ( iMaskSize / 2 ); ++usMaskY  )
						gradVec[1] += (*inputSPtr)( x, y - usMaskY, d ) * dGaussMask[ usMaskY + ( iMaskSize / 2 ) ];			
					(*outputSPtr)( x, y, d ) = gradVec;
					dMaxGradient = std::max( dMaxGradient, norm( gradVec ) );
				}
					
	// Normalize field	
	for( TField2D::iterator outputIt = outputSPtr->begin(); 
		outputIt != outputSPtr->end();	++outputIt )
	{
		(*outputIt) /= dMaxGradient;
		if ( norm(*outputIt) < numeric_limits<double>::epsilon() )
			(*outputIt) = 0.0;	
	}
  setOutput( outputSPtr );  
FEND;
	return true;
}

template<typename ImageType>
bool CGaussDerivative::gauss3D() throw()
{
	// Initialisation
	shared_ptr<ImageType> inputSPtr = static_pointer_cast<ImageType>( getInput() );
  if ( !checkInput<ImageType>( inputSPtr, 3, 3 ) )
  	return false;
	bModuleReady = true;
	bRoiSelf = false; // By default, a ROI is given by the user
	TSmallImagePtr roiSPtr = static_pointer_cast<TSmallImage>( getInput(1) );
	if ( !roiSPtr || !checkInput<TSmallImage>( roiSPtr, 3, 3 ) || roiSPtr->getDataRange().getMaximum() < 1 )
	{
		bRoiSelf = true;
	}
  deleteOldOutput();	

  double dMaxGradient = 0.0;

	TField3DPtr outputSPtr ( 
		new TField3D( 3, inputSPtr->getExtents(), inputSPtr->getDataDimension() ) );	
  outputSPtr->setMaximum( 1.0 );	
	outputSPtr->setMinimum( -1.0 );	
 	(*outputSPtr) = VEC_ZERO3D;
	
	int iMaskSize = 0;
	vector<double> dGaussMask = computeMask( iMaskSize );
	
	// Determine gradient field
	for( ushort d = 0; d < inputSPtr->getDataDimension(); ++d )
		for( ushort z = ( iMaskSize / 2 ); z < inputSPtr->getExtent( 2 ) - ( iMaskSize / 2 ); ++z )
			for( ushort y = ( iMaskSize / 2 ); y < inputSPtr->getExtent( 1 ) - ( iMaskSize / 2 ); ++y )
				for( ushort x = ( iMaskSize / 2 ); x < inputSPtr->getExtent( 0 ) - ( iMaskSize / 2 ); ++x )
					if ( bRoiSelf || (*roiSPtr)( x, y ) )	
					{
						TVector3D gradVec ( 0.0, 0.0, 0.0 );
						for( short usMaskX = -( iMaskSize / 2 ); usMaskX <= ( iMaskSize / 2 ); ++usMaskX  )
							gradVec[0] += (*inputSPtr)( x - usMaskX, y, z, d ) * dGaussMask[ usMaskX + ( iMaskSize / 2 ) ];
						for( short usMaskY = -( iMaskSize / 2 ); usMaskY <= ( iMaskSize / 2 ); ++usMaskY  )
							gradVec[1] += (*inputSPtr)( x, y - usMaskY, z, d ) * dGaussMask[ usMaskY + ( iMaskSize / 2 ) ];			
						for( short usMaskZ = -( iMaskSize / 2 ); usMaskZ <= ( iMaskSize / 2 ); ++usMaskZ  )
							gradVec[2] += (*inputSPtr)( x, y, z - usMaskZ, d ) * dGaussMask[ usMaskZ + ( iMaskSize / 2 ) ];			
						(*outputSPtr)( x, y, z, d ) = gradVec;
						dMaxGradient = std::max( dMaxGradient, norm( gradVec ) );
					}
	
	// Normalize field	
	for( TField3D::iterator outputIt = outputSPtr->begin(); 
		outputIt != outputSPtr->end();	++outputIt )
	{
		(*outputIt) /= dMaxGradient;
		if ( norm(*outputIt) < numeric_limits<double>::epsilon() )
			(*outputIt) = 0.0;	
	}
  setOutput( outputSPtr );  
FEND;
	return true;
}
