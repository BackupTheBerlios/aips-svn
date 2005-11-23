/************************************************************************
 * File: csliceselector.h                                               *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Takes a single orthogonal slice from a 3D volume and    *
 *              converts it to a 2D image                               *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 10.10.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "csliceselector.h"

using namespace std;
using namespace boost;
#ifdef USE_BLITZ
#include <blitz/array.h>
using namespace blitz;
#endif

/*************
 * Structors *
 *************/

template<unsigned int index> void CSliceSelector::call() throw()
{	
	if ( !slice<typename TypeAt<datasetTL, index>::Result>() )
		call<index-1>();
}

template<> void CSliceSelector::call<0>() throw()
{
	typedef TypeAt<datasetTL, 0>::Result TDataType;
	if ( !slice<TDataType>() )
		alog << LWARN << "Dataset type not supported by module" << endl;
}
 
/**
 * \param ulID unique module ID
 */
CSliceSelector::CSliceSelector( ulong ulID ) throw()
  : CConverter ( ulID, 3, 3, "CSliceSelector", "0.6", "CConverter" ), CObserver( "CSliceSelector", "0.6", "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Orthogonal slicer");
  sDocumentation = "Creates an orthogonal 2D slice from a 3D data set.\n"
									 "** Input ports:\n"
                   " 0: A multi-channel 2D or 3D data set\n"
                   " 1: Slice number (optional)\n"
                   " 2: Slice orientation (optional)\n"
                   "** Output ports:\n"
                   " 0: A multi-channel 2D data set\n"
                   " 1: Slice number\n"
                   " 2: Slice orientation\n"
                   "** Parameters:\n"
                   " 0: <SliceNumber>: Number of resulting slice\n"
                   " 1: <SliceOrientation>: Orientation of resulting slice";

  parameters.initUnsignedLong( "SliceNumber", 0UL, 0UL, 0UL );
  parameters.initUnsignedLong( "SliceOrientation", 0UL, 0UL, 2UL );
  inputsVec[0].portType = CPipelineItem::IOOther;
  inputsVec[1].portType = CPipelineItem::IOPoint;
  inputsVec[2].portType = CPipelineItem::IOPoint;
  outputsVec[0].portType = CPipelineItem::IOOther;
  outputsVec[1].portType = CPipelineItem::IOPoint;
  outputsVec[2].portType = CPipelineItem::IOPoint;
/* HB 28-06-05 */	
  
	myDialog.reset( new CSlicerDialog( this ) );
  myDialog->setCaption( getModuleName() );
	myDialog->attachObserver( this, ESliceChangedEvent );
	myDialog->attachObserver( this, EOrientationChangedEvent );
  setModuleDialog( myDialog );
}

CSliceSelector::~CSliceSelector() throw()
{
  
}

/*****************
 * Other methods *
 *****************/

void CSliceSelector::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	call<Length<datasetTL>::value - 1>();
BENCHSTOP;
}

CPipelineItem* CSliceSelector::newInstance( ulong ulID ) const throw()
{
  return ( new CSliceSelector( ulID ) );
}

const std::string CSliceSelector::dump() const throw()
{
  std::ostringstream os;
  os << "current slice: " << parameters.getUnsignedLong( "Slice number" )
    << " current orientation: " << parameters.getUnsignedLong( "Slice orientation" ) << "\n";
  return CPipelineItem::dump() + os.str();
}

/***************
 * Other slots *
 ***************/

/** \param iSlice slice number to use */
void CSliceSelector::updateSlice( int iSlice )
{
  parameters.setUnsignedLong( "SliceNumber", iSlice );  
  forceRecomputation();
	notify( shared_ptr<CDataChangedEvent>( new CDataChangedEvent( this ) ) );
}

/** \param iOrientation orientation to use */
void CSliceSelector::updateOrientation ( int iOrientation )
{
  parameters.setUnsignedLong( "SliceOrientation", iOrientation );
  forceRecomputation();
	notify( shared_ptr<CDataChangedEvent>( new CDataChangedEvent( this ) ) );
}

/** \param volumePtr smart pointer to dataset that should be sliced */
template<typename T> bool CSliceSelector::slice() throw()
{
	if ( !getInput() || getInput()->getType() != typeid( typename T::TDataType ) )
	{
		return false;
	}
	
	shared_ptr<T> inputVolumePtr = static_pointer_cast<T>( getInput() );
  if ( !checkInput<T>( inputVolumePtr, 2, 3 ) )
    return false;
		
	typedef typename T::TDataType TVoxel;
	
	bModuleReady = true;
	// Check if dataset is already 2D. We don't need to do anything in this case...
	if ( inputVolumePtr->getDimension() == 2 )
  {
    myDialog->hideGUIElements();
		parameters.setUnsignedLong( "SliceNumber", 0 );
		parameters.setUnsignedLong( "SliceOrientation", 0 );
		parameters.setUnsignedLong( "SliceNumber_Maximum", 0 );
		parameters.setUnsignedLong( "SliceOrientation_Maximum", 0 );
		myDialog->setNumberDisplay( 0 );
  	boost::shared_ptr<T> outputPtr ( new T( *inputVolumePtr ) );			
		setOutput( outputPtr, 0 );
  	boost::shared_ptr<TInteger> sliceOutputPtr ( 
			new TInteger( 0 ) );
  	setOutput( sliceOutputPtr, 1 );		
  	boost::shared_ptr<TInteger> orientationOutputPtr  ( 
			new TInteger( 0 ) );
	  setOutput( orientationOutputPtr, 2 );	
		return true;
  }
  if ( getInput(1) && getInput(1)->getType() == typeid( TInteger::TDataType ) )
  {
		TIntegerPtr curSlice = static_pointer_cast<TInteger>( getInput(1) );
		if ( curSlice != NULL )
			parameters.setUnsignedLong("SliceNumber", (*curSlice)(0) );
	}
	if ( getInput(2) && getInput(2)->getType() == typeid( TInteger::TDataType ) )
	{
		TIntegerPtr curOri = static_pointer_cast<TInteger>( getInput(2) );
		if ( curOri != NULL )
			parameters.setUnsignedLong("SliceOrientation", (*curOri)(0) );
  }
	// Clear all output ports
  deleteOldOutput(0);
  deleteOldOutput(1);
  deleteOldOutput(2);
  // Get current parameter values
  ushort usCurrentSlice = parameters.getUnsignedLong( "SliceNumber" );
  unsigned char ucCurrentOrientation = parameters.getUnsignedLong( "SliceOrientation" );
	parameters.setUnsignedLong( "SliceOrientation_Maximum", 2 );
	myDialog->showGUIElements();
	myDialog->setSlicerMax( inputVolumePtr->getExtent( 2 - ucCurrentOrientation ) - 1 );
/*	parameters.setUnsignedLong( "SliceNumber", 
		inputVolumePtr->getExtent( 2 - ucCurrentOrientation ) - 1 );*/
	parameters.setUnsignedLong( "SliceNumber_Maximum", 
		inputVolumePtr->getExtent( 2 - ucCurrentOrientation ) - 1 );
  if ( usCurrentSlice > inputVolumePtr->getExtent( 2 - ucCurrentOrientation ) - 1 )
    usCurrentSlice = inputVolumePtr->getExtent( 2 - ucCurrentOrientation ) - 1;
	parameters.setUnsignedLong( "SliceNumber", usCurrentSlice );
	myDialog->setSlicerValue( usCurrentSlice );
	myDialog->setOrientation( ucCurrentOrientation ); 	
	myDialog->setNumberDisplay( usCurrentSlice );
  // Determine dimensions
  size_t dimensionSize[2];
  switch ( ucCurrentOrientation )
  {
    case 0:
      dimensionSize[0] = inputVolumePtr->getExtent( 0 );
      dimensionSize[1] = inputVolumePtr->getExtent( 1 );
      break;
    case 1:
      dimensionSize[0] = inputVolumePtr->getExtent( 0 );
      dimensionSize[1] = inputVolumePtr->getExtent( 2 );
      break;
    case 2:
      dimensionSize[0] = inputVolumePtr->getExtent( 1 );
      dimensionSize[1] = inputVolumePtr->getExtent( 2 );
      break;
  }
	parameters.setUnsignedLong( "SliceOrientation", ucCurrentOrientation );
  // Create slice
  boost::shared_ptr<T> outputPtr ( new T( 2, dimensionSize, inputVolumePtr->getDataDimension() ) );
  outputPtr->setDataRange( inputVolumePtr->getDataRange() );

#ifdef USE_BLITZ				
	Array<TVoxel, 3> slice ( outputPtr->getArray(), shape( dimensionSize[0], dimensionSize[1], 
		outputPtr->getDataDimension() ), neverDeleteData, AIPSArray<3>() );
	Array<TVoxel, 4> data ( inputVolumePtr->getArray(), shape( inputVolumePtr->getExtent(0),
		inputVolumePtr->getExtent(1), inputVolumePtr->getExtent(2), inputVolumePtr->getDataDimension() ),
    neverDeleteData, AIPSArray<4>() );
		
	switch ( ucCurrentOrientation )
	{
		case 0:
			slice = data( Range::all(), Range( inputVolumePtr->getExtent(1), 0, -1), usCurrentSlice, Range::all() );
			break;
		case 1:
			slice = data( Range::all(), inputVolumePtr->getExtent(1) - 1 - usCurrentSlice, Range::all(), Range::all() );
			break;
		case 2:
			slice = data( usCurrentSlice, Range( inputVolumePtr->getExtent(1), 0, -1), Range::all(), Range::all() );
			break;
	}
#else
  if ( ucCurrentOrientation == 0 )
  {
		for ( ushort usChannel = 0; usChannel < inputVolumePtr->getDataDimension(); usChannel++ )
	    for ( uint x = 0; x < inputVolumePtr->getExtent(0); x++ )
				for ( uint y = 0; y < inputVolumePtr->getExtent(1); y++ )
    	    (*outputPtr)( x, inputVolumePtr->getExtent(1) - 1 - y, usChannel ) = (*inputVolumePtr)( x, y, usCurrentSlice, usChannel );
  }
  else if ( ucCurrentOrientation == 1 )
  {
		for ( ushort usChannel = 0; usChannel < inputVolumePtr->getDataDimension(); usChannel++ )
	    for ( uint x = 0; x < inputVolumePtr->getExtent(0); x++ )
				for ( uint z = 0; z < inputVolumePtr->getExtent(2); z++ )
    	    (*outputPtr)( x, z, usChannel ) = (*inputVolumePtr)( x, inputVolumePtr->getExtent(1) - 1 - usCurrentSlice, z, usChannel );
  }
  else if ( ucCurrentOrientation == 2 )
  {
		for ( ushort usChannel = 0; usChannel < inputVolumePtr->getDataDimension(); usChannel++ )
	    for ( uint y = 0; y < inputVolumePtr->getExtent(1); y++ )
				for ( uint z = 0; z < inputVolumePtr->getExtent(2); z++ )
    	    (*outputPtr)( inputVolumePtr->getExtent(1) - 1 - y, z, usChannel ) = (*inputVolumePtr)( usCurrentSlice, y, z, usChannel );
  }
#endif
  // Set output ports
  setOutput( outputPtr, 0 );  
  TIntegerPtr sliceOutputPtr ( new TInteger( parameters.getUnsignedLong( "SliceNumber" ) ) );
  TIntegerPtr orientationOutputPtr  (	new TInteger( parameters.getUnsignedLong( "SliceOrientation" ) ) );
  setOutput( sliceOutputPtr, 1 );
  setOutput( orientationOutputPtr, 2 );  
	return true;
}

void CSliceSelector::execute( shared_ptr<CEvent> anEvent )
{
DS("+++ CSliceSelector::execute");
	if ( anEvent->getType() == ESliceChangedEvent )
	{
		CSliceChangedEvent* ptr = static_cast<CSliceChangedEvent*>( anEvent.get() );
		if( ptr )
			updateSlice( ptr->getSlice() );
	}
	else if ( anEvent->getType() == EOrientationChangedEvent )
	{
		COrientationChangedEvent* ptr = static_cast<COrientationChangedEvent*>( anEvent.get() );
		if( ptr )
			updateOrientation( ptr->getOrientation() );
	}
DS("--- CSliceSelector::execute");
}

template<> bool CSliceSelector::slice<TStringField>() throw()
{
	return false;
}
