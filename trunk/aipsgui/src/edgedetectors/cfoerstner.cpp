/************************************************************************
 * File: cfoerstner.cpp                                                 *
 * Project: AIPS                                                        *
 * Description: The Foerstner-Corner-Detector                           *
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
#include "cfoerstner.h"
#include <cfloat>

using namespace std;

/*************
 * Structors *
 *************/

CFoerstner::CFoerstner(ulong ulID_ ) throw()
  : CFilter(ulID_, "Foerstner-Operator", 1, 1, "CFoerstner", "0.1", "CFilter") 
{
  setModuleID( sLibID );
  
  sDocumentation = "Extracts regions of high curvature from the input image\n"
                   "** Input ports:\n"
                   "0: A scalar single channel data set\n"
                   "** Output ports:\n"
                   "1: A scalar field of detected corners\n"
									 "** Parameters:\n"
									 "Significant point threshold: Threshold to exclude unsignifant points";

	parameters.initDouble( "Significant point threshold", 0.5, 0.0, 10.0 );
									 
  inputsVec[0].portType = CPipelineItem::IOVector;
  outputsVec[0].portType = CPipelineItem::IOInteger;
}

CFoerstner::~CFoerstner() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CFoerstner::apply() throw()
{
	BENCHSTART;
	bModuleReady = false;
  TField2D* inputPtr = static_cast<TField2D*>( getInput().get() );
  if ( inputPtr == NULL || inputPtr->getDimension() != 2 )
  {
		TField3D* inputPtr3D = static_cast<TField3D*>( getInput().get() );
		if ( inputPtr3D == NULL || inputPtr3D->getDimension() != 3 )
		{
    	alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    	return;
		}
		else
			apply3D( inputPtr3D );
  }
	else
		apply2D( inputPtr );	
	BENCHSTOP;	
}

CPipelineItem* CFoerstner::newInstance( ulong ulID ) const throw()
{
	return new CFoerstner( ulID );
}

/** \param inputPtr a 2D gradient vector field */
void CFoerstner::apply2D( TField2D* inputPtr ) throw()
{
	bModuleReady = true;
	TField2D& image = *inputPtr;
  deleteOldOutput();	
	boost::shared_ptr<TImage> output ( new TImage( 2, image.getExtents() ) );
	output->setMinimum( 0 );
	output->setMaximum( 0 );
	PROG_MAX( inputPtr->getExtent( 1 ) - 1 );
	double dThreshold = parameters.getDouble( "Significant point threshold" );
	
	for( ushort y = 1; y < inputPtr->getExtent( 1 ) - 1; ++y )
	{
		PROG_VAL( y );
		APP_PROC();	
		for ( ushort x = 1; x < inputPtr->getExtent( 0 ) - 1; ++x )
		{
			/// Average of gx^2
			double gx = ( image(x,y)[0]*image(x,y)[0] + image(x+1,y)[0]*image(x+1,y)[0] 
			 + image(x-1,y)[0]*image(x-1,y)[0] + image(x,y-1)[0]*image(x,y-1)[0] 
			 + image(x+1,y-1)[0]*image(x+1,y-1)[0] + image(x-1,y-1)[0]*image(x-1,y-1)[0] 
			 + image(x,y+1)[0]*image(x,y+1)[0] + image(x+1,y+1)[0]*image(x+1,y+1)[0] 
			 + image(x-1,y+1)[0]*image(x-1,y+1)[0] ) / 9.0; 
			/// Average of gy^2
			double gy = ( image(x,y)[1]*image(x,y)[1] + image(x+1,y)[1]*image(x+1,y)[1] 
			 + image(x-1,y)[1]*image(x-1,y)[1] + image(x,y-1)[1]*image(x,y-1)[1] 
			 + image(x+1,y-1)[1]*image(x+1,y-1)[1] + image(x-1,y-1)[1]*image(x-1,y-1)[1] 
			 + image(x,y+1)[1]*image(x,y+1)[1] + image(x+1,y+1)[1]*image(x+1,y+1)[1] 
			 + image(x-1,y+1)[1]*image(x-1,y+1)[1] ) / 9.0; 			
			/// Average of gxgy
			double gxgy = ( image(x,y)[0]*image(x,y)[1] + image(x+1,y)[0]*image(x+1,y)[1] 
				+ image(x-1,y)[0]*image(x-1,y)[1] + image(x,y-1)[0]*image(x,y-1)[1] 
				+ image(x+1,y-1)[0]*image(x+1,y-1)[1] + image(x-1,y-1)[0]*image(x-1,y-1)[1] 
			  + image(x,y+1)[0]*image(x,y+1)[1] + image(x+1,y+1)[0]*image(x+1,y+1)[1] 
				+ image(x-1,y+1)[0] * image(x-1,y+1)[1]	)	/ 9.0;
			double operatorOutput = 0.0; 
			if ( std::abs(gx) > numeric_limits<double>::epsilon() 
				|| std::abs(gy) > numeric_limits<double>::epsilon() )
			{
				// Determine curvature
				double dNumerator = ( gx * gy )  - ( gxgy * gxgy );						
				double dDenominator = ( gx + gy ) / 2.0;				
				// Only set output if operator response is greater or equal to specified threshold
				if ( ( dNumerator / pow( dDenominator, 2 ) ) >= dThreshold )
					operatorOutput = round( std::abs( dNumerator / dDenominator ) * 255.0 );
			}
			if ( operatorOutput < numeric_limits<ushort>::max() )
				(*output)( x, y ) = static_cast<ushort>( operatorOutput );
			else
				(*output)( x, y ) = numeric_limits<ushort>::max();
			if ( (*output)( x, y ) > output->getMaximum() )
				output->setMaximum( (*output)( x, y ) );
		}
	}
	PROG_RESET();
	setOutput( output );
}

/** \param inputPtr a 3D gradient vector field */
void CFoerstner::apply3D( TField3D* inputPtr ) throw()
{
	bModuleReady = true;
	TField3D& image = *inputPtr;
  deleteOldOutput();	
	boost::shared_ptr<TImage> output ( new TImage( 3, image.getExtents() ) );
	output->setMinimum( 0 );
	output->setMaximum( 0 );
	PROG_MAX( inputPtr->getExtent( 2 ) - 1 );
	double dThreshold = parameters.getDouble( "Significant point threshold" );
	
	for( ushort z = 1; z < inputPtr->getExtent( 2 ) - 1; ++z )
	{
		PROG_VAL( z );
		APP_PROC();
		for( ushort y = 1; y < inputPtr->getExtent( 1 ) - 1; ++y )
		for ( ushort x = 1; x < inputPtr->getExtent( 0 ) - 1; ++x )
		{
			/// Average of gx^2
			double gx = ( image(x,y,z)[0]*image(x,y,z)[0] + image(x+1,y,z)[0]*image(x+1,y,z)[0] 
			 + image(x-1,y,z)[0]*image(x-1,y,z)[0] + image(x,y-1,z)[0]*image(x,y-1,z)[0] 
			 + image(x+1,y-1,z)[0]*image(x+1,y-1,z)[0] + image(x-1,y-1,z)[0]*image(x-1,y-1,z)[0] 
			 + image(x,y+1,z)[0]*image(x,y+1,z)[0] + image(x+1,y+1,z)[0]*image(x+1,y+1,z)[0] 
			 + image(x-1,y+1,z)[0]*image(x-1,y+1,z)[0] 
		   + image(x,y,z-1)[0]*image(x,y,z-1)[0] + image(x+1,y,z-1)[0]*image(x+1,y,z-1)[0] 
			 + image(x-1,y,z-1)[0]*image(x-1,y,z-1)[0] + image(x,y-1,z-1)[0]*image(x,y-1,z-1)[0] 
			 + image(x+1,y-1,z-1)[0]*image(x+1,y-1,z-1)[0] + image(x-1,y-1,z-1)[0]*image(x-1,y-1,z-1)[0] 
			 + image(x,y+1,z-1)[0]*image(x,y+1,z-1)[0] + image(x+1,y+1,z-1)[0]*image(x+1,y+1,z-1)[0] 
			 + image(x-1,y+1,z-1)[0]*image(x-1,y+1,z-1)[0] 
			 + image(x,y,z)[0]*image(x,y,z+1)[0] + image(x+1,y,z+1)[0]*image(x+1,y,z+1)[0] 
			 + image(x-1,y,z+1)[0]*image(x-1,y,z+1)[0] + image(x,y-1,z+1)[0]*image(x,y-1,z+1)[0] 
			 + image(x+1,y-1,z+1)[0]*image(x+1,y-1,z+1)[0] + image(x-1,y-1,z+1)[0]*image(x-1,y-1,z+1)[0] 
			 + image(x,y+1,z+1)[0]*image(x,y+1,z+1)[0] + image(x+1,y+1,z+1)[0]*image(x+1,y+1,z+1)[0] 
			 + image(x-1,y+1,z+1)[0]*image(x-1,y+1,z+1)[0] ) ; 
			/// Average of gy^2
			double gy = ( image(x,y,z)[1]*image(x,y,z)[1] + image(x+1,y,z)[1]*image(x+1,y,z)[1] 
			 + image(x-1,y,z)[1]*image(x-1,y,z)[1] + image(x,y-1,z)[1]*image(x,y-1,z)[1] 
			 + image(x+1,y-1,z)[1]*image(x+1,y-1,z)[1] + image(x-1,y-1,z)[1]*image(x-1,y-1,z)[1] 
			 + image(x,y+1,z)[1]*image(x,y+1,z)[1] + image(x+1,y+1,z)[1]*image(x+1,y+1,z)[1] 
			 + image(x-1,y+1,z)[1]*image(x-1,y+1,z)[1] 
		   + image(x,y,z-1)[1]*image(x,y,z-1)[1] + image(x+1,y,z-1)[1]*image(x+1,y,z-1)[1] 
			 + image(x-1,y,z-1)[1]*image(x-1,y,z-1)[1] + image(x,y-1,z-1)[1]*image(x,y-1,z-1)[1] 
			 + image(x+1,y-1,z-1)[1]*image(x+1,y-1,z-1)[1] + image(x-1,y-1,z-1)[1]*image(x-1,y-1,z-1)[1] 
			 + image(x,y+1,z-1)[1]*image(x,y+1,z-1)[1] + image(x+1,y+1,z-1)[1]*image(x+1,y+1,z-1)[1] 
			 + image(x-1,y+1,z-1)[1]*image(x-1,y+1,z-1)[1] 
			 + image(x,y,z)[1]*image(x,y,z+1)[1] + image(x+1,y,z+1)[1]*image(x+1,y,z+1)[1] 
			 + image(x-1,y,z+1)[1]*image(x-1,y,z+1)[1] + image(x,y-1,z+1)[1]*image(x,y-1,z+1)[1] 
			 + image(x+1,y-1,z+1)[1]*image(x+1,y-1,z+1)[1] + image(x-1,y-1,z+1)[1]*image(x-1,y-1,z+1)[1] 
			 + image(x,y+1,z+1)[1]*image(x,y+1,z+1)[1] + image(x+1,y+1,z+1)[1]*image(x+1,y+1,z+1)[1] 
			 + image(x-1,y+1,z+1)[1]*image(x-1,y+1,z+1)[1] ) ; 
			/// Average of gz^2
			double gz = ( image(x,y,z)[2]*image(x,y,z)[2] + image(x+1,y,z)[2]*image(x+1,y,z)[2] 
			 + image(x-1,y,z)[2]*image(x-1,y,z)[2] + image(x,y-1,z)[2]*image(x,y-1,z)[2] 
			 + image(x+1,y-1,z)[2]*image(x+1,y-1,z)[2] + image(x-1,y-1,z)[2]*image(x-1,y-1,z)[2] 
			 + image(x,y+1,z)[2]*image(x,y+1,z)[2] + image(x+1,y+1,z)[2]*image(x+1,y+1,z)[2] 
			 + image(x-1,y+1,z)[2]*image(x-1,y+1,z)[2] 
		   + image(x,y,z-1)[2]*image(x,y,z-1)[2] + image(x+1,y,z-1)[2]*image(x+1,y,z-1)[2] 
			 + image(x-1,y,z-1)[2]*image(x-1,y,z-1)[2] + image(x,y-1,z-1)[2]*image(x,y-1,z-1)[2] 
			 + image(x+1,y-1,z-1)[2]*image(x+1,y-1,z-1)[2] + image(x-1,y-1,z-1)[2]*image(x-1,y-1,z-1)[2] 
			 + image(x,y+1,z-1)[2]*image(x,y+1,z-1)[2] + image(x+1,y+1,z-1)[2]*image(x+1,y+1,z-1)[2] 
			 + image(x-1,y+1,z-1)[2]*image(x-1,y+1,z-1)[2] 
			 + image(x,y,z)[2]*image(x,y,z+1)[2] + image(x+1,y,z+1)[2]*image(x+1,y,z+1)[2] 
			 + image(x-1,y,z+1)[2]*image(x-1,y,z+1)[2] + image(x,y-1,z+1)[2]*image(x,y-1,z+1)[2] 
			 + image(x+1,y-1,z+1)[2]*image(x+1,y-1,z+1)[2] + image(x-1,y-1,z+1)[2]*image(x-1,y-1,z+1)[2] 
			 + image(x,y+1,z+1)[2]*image(x,y+1,z+1)[2] + image(x+1,y+1,z+1)[2]*image(x+1,y+1,z+1)[2] 
			 + image(x-1,y+1,z+1)[2]*image(x-1,y+1,z+1)[2] ) ;  						
			/// Average of gxgy
			double gxgy = ( image(x,y,z)[0]*image(x,y,z)[1] + image(x+1,y,z)[0]*image(x+1,y,z)[1] 
				+ image(x-1,y,z)[0]*image(x-1,y,z)[1] + image(x,y-1,z)[0]*image(x,y-1,z)[1] 
				+ image(x+1,y-1,z)[0]*image(x+1,y-1,z)[1] + image(x-1,y-1,z)[0]*image(x-1,y-1,z)[1] 
			  + image(x,y+1,z)[0]*image(x,y+1,z)[1] + image(x+1,y+1,z)[0]*image(x+1,y+1,z)[1] 
				+ image(x-1,y+1,z)[0] * image(x-1,y+1,z)[1]
				+ image(x,y,z-1)[0]*image(x,y,z-1)[1] + image(x+1,y,z-1)[0]*image(x+1,y,z-1)[1] 
				+ image(x-1,y,z-1)[0]*image(x-1,y,z-1)[1] + image(x,y-1,z-1)[0]*image(x,y-1,z-1)[1] 
				+ image(x+1,y-1,z-1)[0]*image(x+1,y-1,z-1)[1] + image(x-1,y-1,z-1)[0]*image(x-1,y-1,z-1)[1] 
			  + image(x,y+1,z-1)[0]*image(x,y+1,z-1)[1] + image(x+1,y+1,z-1)[0]*image(x+1,y+1,z-1)[1] 
				+ image(x-1,y+1,z-1)[0] * image(x-1,y+1,z-1)[1]
				+ image(x,y,z+1)[0]*image(x,y,z+1)[1] + image(x+1,y,z+1)[0]*image(x+1,y,z+1)[1] 
				+ image(x-1,y,z+1)[0]*image(x-1,y,z+1)[1] + image(x,y-1,z+1)[0]*image(x,y-1,z+1)[1] 
				+ image(x+1,y-1,z+1)[0]*image(x+1,y-1,z+1)[1] + image(x-1,y-1,z+1)[0]*image(x-1,y-1,z+1)[1] 
			  + image(x,y+1,z+1)[0]*image(x,y+1,z+1)[1] + image(x+1,y+1,z+1)[0]*image(x+1,y+1,z+1)[1] 
				+ image(x-1,y+1,z+1)[0] * image(x-1,y+1,z+1)[1]	)	;
			/// Average of gxgz
			double gxgz = ( image(x,y,z)[0]*image(x,y,z)[2] + image(x+1,y,z)[0]*image(x+1,y,z)[2] 
				+ image(x-1,y,z)[0]*image(x-1,y,z)[2] + image(x,y-1,z)[0]*image(x,y-1,z)[2] 
				+ image(x+1,y-1,z)[0]*image(x+1,y-1,z)[2] + image(x-1,y-1,z)[0]*image(x-1,y-1,z)[2] 
			  + image(x,y+1,z)[0]*image(x,y+1,z)[2] + image(x+1,y+1,z)[0]*image(x+1,y+1,z)[2] 
				+ image(x-1,y+1,z)[0] * image(x-1,y+1,z)[2]
				+ image(x,y,z-1)[0]*image(x,y,z-1)[2] + image(x+1,y,z-1)[0]*image(x+1,y,z-1)[2] 
				+ image(x-1,y,z-1)[0]*image(x-1,y,z-1)[2] + image(x,y-1,z-1)[0]*image(x,y-1,z-1)[2] 
				+ image(x+1,y-1,z-1)[0]*image(x+1,y-1,z-1)[2] + image(x-1,y-1,z-1)[0]*image(x-1,y-1,z-1)[2] 
			  + image(x,y+1,z-1)[0]*image(x,y+1,z-1)[2] + image(x+1,y+1,z-1)[0]*image(x+1,y+1,z-1)[2] 
				+ image(x-1,y+1,z-1)[0] * image(x-1,y+1,z-1)[2]
				+ image(x,y,z+1)[0]*image(x,y,z+1)[2] + image(x+1,y,z+1)[0]*image(x+1,y,z+1)[2] 
				+ image(x-1,y,z+1)[0]*image(x-1,y,z+1)[2] + image(x,y-1,z+1)[0]*image(x,y-1,z+1)[2] 
				+ image(x+1,y-1,z+1)[0]*image(x+1,y-1,z+1)[2] + image(x-1,y-1,z+1)[0]*image(x-1,y-1,z+1)[2] 
			  + image(x,y+1,z+1)[0]*image(x,y+1,z+1)[2] + image(x+1,y+1,z+1)[0]*image(x+1,y+1,z+1)[2] 
				+ image(x-1,y+1,z+1)[0] * image(x-1,y+1,z+1)[2]	)	;
			/// Average of gygz
			double gygz = ( image(x,y,z)[2]*image(x,y,z)[1] + image(x+1,y,z)[2]*image(x+1,y,z)[1] 
				+ image(x-1,y,z)[2]*image(x-1,y,z)[1] + image(x,y-1,z)[2]*image(x,y-1,z)[1] 
				+ image(x+1,y-1,z)[2]*image(x+1,y-1,z)[1] + image(x-1,y-1,z)[2]*image(x-1,y-1,z)[1] 
			  + image(x,y+1,z)[2]*image(x,y+1,z)[1] + image(x+1,y+1,z)[2]*image(x+1,y+1,z)[1] 
				+ image(x-1,y+1,z)[2] * image(x-1,y+1,z)[1]
				+ image(x,y,z-1)[2]*image(x,y,z-1)[1] + image(x+1,y,z-1)[2]*image(x+1,y,z-1)[1] 
				+ image(x-1,y,z-1)[2]*image(x-1,y,z-1)[1] + image(x,y-1,z-1)[2]*image(x,y-1,z-1)[1] 
				+ image(x+1,y-1,z-1)[2]*image(x+1,y-1,z-1)[1] + image(x-1,y-1,z-1)[2]*image(x-1,y-1,z-1)[1] 
			  + image(x,y+1,z-1)[2]*image(x,y+1,z-1)[1] + image(x+1,y+1,z-1)[2]*image(x+1,y+1,z-1)[1] 
				+ image(x-1,y+1,z-1)[2] * image(x-1,y+1,z-1)[1]
				+ image(x,y,z+1)[2]*image(x,y,z+1)[1] + image(x+1,y,z+1)[2]*image(x+1,y,z+1)[1] 
				+ image(x-1,y,z+1)[2]*image(x-1,y,z+1)[1] + image(x,y-1,z+1)[2]*image(x,y-1,z+1)[1] 
				+ image(x+1,y-1,z+1)[2]*image(x+1,y-1,z+1)[1] + image(x-1,y-1,z+1)[2]*image(x-1,y-1,z+1)[1] 
			  + image(x,y+1,z+1)[2]*image(x,y+1,z+1)[1] + image(x+1,y+1,z+1)[2]*image(x+1,y+1,z+1)[1] 
				+ image(x-1,y+1,z+1)[2] * image(x-1,y+1,z+1)[1]	)	;
			double operatorOutput = 0.0; 
			if ( std::abs(gx) > numeric_limits<double>::epsilon() 
				|| std::abs(gy) > numeric_limits<double>::epsilon() 
				|| std::abs(gz) > numeric_limits<double>::epsilon() )
			{
				// Determine curvature
				double dNumerator = ( gx * gy * gz )  - ( gx * gygz * gygz ) 
					+ ( gxgy * gygz * gxgz ) - ( gz * gxgy * gxgy )
					+ ( gxgz * gxgy * gygz ) - ( gy * gxgz * gxgz );
				double dDenominator = ( gx + gy + gz ) / 3.0;			
				// Only set output if operator response is greater or equal to specified threshold
				if ( ( dNumerator / pow( dDenominator, 3 ) ) >= dThreshold )
					operatorOutput = round( std::abs( dNumerator / dDenominator ) * 255.0 );
			}
			if ( operatorOutput < numeric_limits<ushort>::max() )
				(*output)( x, y, z ) = static_cast<ushort>( operatorOutput );
			else
				(*output)( x, y, z ) = numeric_limits<ushort>::max();
			if ( (*output)( x, y, z ) > output->getMaximum() )
				output->setMaximum( (*output)( x, y, z ) );
		}
	}
	setOutput( output );
	PROG_RESET();
}
