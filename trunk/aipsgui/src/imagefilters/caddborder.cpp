/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: $DATE                                                       *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "caddborder.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CAddBorder::CAddBorder( ulong ulID ) throw()
   : CFilter ( ulID, "Add border", 1, 1, "CAddBorder","0.1","CFilter" )
{
	parameters.initUnsignedLong("Radius",0,0,100);
  setModuleID( sLibID );
}

CAddBorder::~CAddBorder() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CAddBorder::apply() throw()
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
	size_t extents[3];
	ulong ulRadius = parameters.getUnsignedLong("Radius");
	extents[0] = inputPtr->getExtent(0)+2*ulRadius;
	extents[1] = inputPtr->getExtent(1)+2*ulRadius;
	if ( inputPtr->getDimension() == 3 )
		extents[2] = inputPtr->getExtent(2)+2*ulRadius;
	else
		extents[2] = 1;
		
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), extents, 
		inputPtr->getDataDimension() ) );
		
	(*outputPtr) = 0;
	if ( extents[2]>1 )
	for( uint z = ulRadius; z < extents[2]-ulRadius; ++z )
		for( uint y = ulRadius; y < extents[1]-ulRadius; ++y )
			for( uint x = ulRadius; x < extents[0]-ulRadius; ++x )
				{					
					(*outputPtr)(x,y,z) = (*inputPtr)(x-ulRadius,y-ulRadius,z-ulRadius);
				}
	else
		for( uint y = ulRadius; y < extents[1]-ulRadius; ++y )
			for( uint x = ulRadius; x < extents[0]-ulRadius; ++x )
				{					
					(*outputPtr)(x,y) = (*inputPtr)(x-ulRadius,y-ulRadius);
				}
	
	outputPtr->setDataRange( inputPtr->getDataRange() );
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CAddBorder::newInstance( ulong ulID ) const throw()
{
  return new CAddBorder( ulID );
}



