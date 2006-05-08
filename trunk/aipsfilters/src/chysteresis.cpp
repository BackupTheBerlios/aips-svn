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
#include "chysteresis.h"

using namespace std;
using namespace boost;

/* Structors */

DEFINE_CALL_MACRO( CHysteresis::call, CHysteresis::hysteresis, imageTL )

/**
 * \param ulID unique module ID
 */
CHysteresis::CHysteresis( ulong ulID ) throw()
: CFilter( ulID, "Hysteresis thresholding", 2, 1, "CHysteresis", CHYSTERESIS_VERSION, "CFilter" ) 
{
  setModuleID( sLibID );
  
  sDocumentation = "Creates a gradient field from the input image via a sobel filter\n"
                   "** Input ports:\n"
                   "0: A scalar single channel 2D or 3D data set (image data, required)\n"
									 "(region of interest, optional)\n"
                   "** Output ports:\n"
                   "0: A scalar single channel 2D or 3D data set";

	parameters.initLong( "Low", 40, -32767, 32767 );
	parameters.initLong( "High", 120, -32767, 32767 );
  inputsVec[0].portType = IOInteger;
  outputsVec[0].portType = IOInteger;
}

CHysteresis::~CHysteresis() throw()
{
}

/* Other methods */

void CHysteresis::apply() throw()
{
FBEGIN;
	bModuleReady = false;
	if ( getInput() )
		call<Length<imageTL>::value - 1>();
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
FEND;	
}

template<typename ImageType> bool CHysteresis::hysteresis() throw()
{
FBEGIN;
	shared_ptr<ImageType> inputSPtr = static_pointer_cast<ImageType>( getInput() );
  if ( !checkInput<ImageType>( inputSPtr, 2, 3 ) )
    return false;
	bModuleReady = true;
  deleteOldOutput();	
	shared_ptr<ImageType> outputSPtr ( 
		new ImageType( inputSPtr->getDimension(), inputSPtr->getExtents(), inputSPtr->getDataDimension() ) );
	outputSPtr->setDataRange( inputSPtr->getDataRange() );
	(*outputSPtr) = 0;
	long lLowThres = parameters.getLong( "Low" );
	long lHighThres = parameters.getLong( "High" );
  if ( inputSPtr->getDimension() == 2 )
  {
  	for( ushort d = 0; d < outputSPtr->getDataDimension(); ++d )
			for( ushort y = 1; y < outputSPtr->getExtent(1); ++y )
				for( ushort x = 1; x < outputSPtr->getExtent(0); ++x )
				{
					if ( (*inputSPtr)(x,y,d) > lHighThres )
						(*outputSPtr)(x,y,d) = (*inputSPtr)(x,y,d);
					else if ( (*inputSPtr)(x,y,d) >= lLowThres )
					{
						ushort nb = 0;
						if ( (*inputSPtr)(x+1,y,d) > lHighThres ) nb++;
						if ( (*inputSPtr)(x-1,y,d) > lHighThres ) nb++;
						if ( (*inputSPtr)(x,y+1,d) > lHighThres ) nb++;
						if ( (*inputSPtr)(x,y-1,d) > lHighThres ) nb++;
						if ( (*inputSPtr)(x+1,y+1,d) > lHighThres ) nb++;
						if ( (*inputSPtr)(x+1,y-1,d) > lHighThres ) nb++;
						if ( (*inputSPtr)(x-1,y+1,d) > lHighThres ) nb++;
						if ( (*inputSPtr)(x-1,y-1,d) > lHighThres ) nb++;
						if ( nb )
							(*outputSPtr)(x,y,d) = (*inputSPtr)(x,y,d);
					}
				}
	}
	else
  {
  	for( ushort d = 0; d < outputSPtr->getDataDimension(); ++d )
	  	for( ushort z = 1; z < outputSPtr->getExtent(2)-1; ++z )
				for( ushort y = 1; y < outputSPtr->getExtent(1)-1; ++y )
					for( ushort x = 1; x < outputSPtr->getExtent(0)-1; ++x )
					{
						if ( (*inputSPtr)(x,y,z,d) > lHighThres )
							(*outputSPtr)(x,y,z,d) = (*inputSPtr)(x,y,z,d);
						else if ( (*inputSPtr)(x,y,z,d) >= lLowThres )
						{
							ushort nb = 0;
							if ( (*inputSPtr)(x+1,y,z,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y,z,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x,y+1,z,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x,y-1,z,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y+1,z,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y-1,z,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y+1,z,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y-1,z,d) > lHighThres ) nb++;
						
							if ( (*inputSPtr)(x,y,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x,y+1,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x,y-1,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y+1,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y-1,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y+1,z-1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y-1,z-1,d) > lHighThres ) nb++;
						
							if ( (*inputSPtr)(x,y,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x,y+1,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x,y-1,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y+1,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x+1,y-1,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y+1,z+1,d) > lHighThres ) nb++;
							if ( (*inputSPtr)(x-1,y-1,z+1,d) > lHighThres ) nb++;
						
							if ( nb )
								(*outputSPtr)(x,y,z,d) = (*inputSPtr)(x,y,z,d);
						}
					}
	}
  setOutput( outputSPtr );
FEND;
	return true;  
}
