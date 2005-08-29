/************************************************************************
 * File: cchannelsplitter.h                                             *
 * Project: AIPS                                                        *
 * Description: Splits a multichannel image into several singlechannels *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 09-07-2004                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cchannelsplitter.h"

using namespace std;
using namespace boost;

/** 
 * \param ulID unique module ID
 */
CChannelSplitter::CChannelSplitter( ulong ulID ) throw()
  : CConverter ( ulID, 1, 4, "CChannelSplitter", "0.2", "CConverter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  setModuleName("Channel splitter");

  sDocumentation = "Splits up a multi-channel image into several single-channel images\n"
									 "** Input ports:\n"
                   " 0: A multi-channel 2D or 3D data set\n"
                   "** Output ports:\n"
                   " 0: A single-channel 2D or 3D data set (first channel)\n"
									 " 1: A single-channel 2D or 3D data set (second channel)\n"
									 " 2: A single-channel 2D or 3D data set (third channel)\n"
									 " 3: A multi-channel 2D or 3D data set (remaining channels)";
	
  inputsVec[0].portType = IOOther;
  outputsVec[0].portType = IOOther;
	outputsVec[1].portType = IOOther;
	outputsVec[2].portType = IOOther;
	outputsVec[3].portType = IOOther;
}

CChannelSplitter::~CChannelSplitter() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CChannelSplitter::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	if ( typeid( *(getInput()) ) == typeid( TImage ) )
		splitup<TImage>();
	else if ( typeid( *(getInput()) ) == typeid( TField ) )
		splitup<TField>();		
	else if ( typeid( *(getInput()) ) == typeid( TField2D ) )
		splitup<TField2D>();		
	else if ( typeid( *(getInput()) ) == typeid( TField3D ) )
		splitup<TField3D>();		
	else
		alog << LWARN << SERROR( "No input or wrong data type" ) << endl;
BENCHSTOP;
}

CPipelineItem* CChannelSplitter::newInstance( ulong ulID ) const throw()
{
  return new CChannelSplitter( ulID );
}

template<typename T> void CChannelSplitter::splitup() throw()
{
	if ( !getInput() || getInput()->getType() != typeid( typename dataTraits<T>::dataType ) )
		return;
	shared_ptr<T> inputPtr = static_pointer_cast<T>( getInput() );
	if ( !checkInput<T>( inputPtr, 2, 3 ) )
		return;
 	
	bModuleReady = true;
	deleteOldOutput(0);
	deleteOldOutput(1);
	deleteOldOutput(2);
	deleteOldOutput(3);
	ushort usDims = inputPtr->getDataDimension();		
	if ( usDims > 3 )
		usDims = 3;
	vector<shared_ptr<T> > output;
	for( ushort i = 0; i < usDims; ++i )
	{
		shared_ptr<T> outputPtr ( new T( inputPtr->getDimension(), inputPtr->getExtents(), 1 ) );
		outputPtr->setMinimum( inputPtr->getMinimum() );
		outputPtr->setMaximum( inputPtr->getMaximum() );
		output.push_back( outputPtr );
	}
	if ( inputPtr->getDataDimension() > 3 )
	{
		shared_ptr<T> outputPtr ( new T( inputPtr->getDimension(), 
			inputPtr->getExtents(),	inputPtr->getDataDimension() - 3 ) );
		outputPtr->setMinimum( inputPtr->getMinimum() );
		outputPtr->setMaximum( inputPtr->getMaximum() );
		output.push_back( outputPtr );
	}
	
	for( ushort usChannels = 0; usChannels < inputPtr->getDataDimension(); ++usChannels )
	{
		if ( inputPtr->getDimension() == 2 )
		{
			for( ushort y = 0; y < inputPtr->getExtent(1); ++y )
			for( ushort x = 0; x < inputPtr->getExtent(0); ++x )
			{
				if ( usChannels < 3 )
				{
					(*output[usChannels])( x, y ) = (*inputPtr)( x, y, usChannels );
				}
				else
				{
					(*output[3])( x, y, usChannels - 3 ) = (*inputPtr)( x, y, usChannels );
				}
			}
		}
		else
		{
			for( ushort z = 0; z < inputPtr->getExtent(2); ++z )
			for( ushort y = 0; y < inputPtr->getExtent(1); ++y )
			for( ushort x = 0; x < inputPtr->getExtent(0); ++x )
			{
				if ( usChannels < 3 )
				{
					(*output[usChannels])( x, y, z ) = (*inputPtr)( x, y, z, usChannels );
				}
				else
				{
					(*output[3])( x, y, z, usChannels - 3 ) = (*inputPtr)( x, y, z, usChannels );
				}
			}
		}
	}
  setOutput( output[0], 0 );
	if ( output.size() > 1 ) setOutput( output[1], 1 );
	if ( output.size() > 2 ) setOutput( output[2], 2 );
	if ( output.size() > 3 ) setOutput( output[3], 3 );
}
