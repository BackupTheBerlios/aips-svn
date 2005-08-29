/************************************************************************
 * File: csynergeticmodel.cpp                                           *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 05.11.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "csynergeticmodel.h"

using namespace std;
using namespace boost;

CSynergeticModel::CSynergeticModel( ulong ulID ) throw()
  : CFilter( ulID, "Synergetic region growing", 2, 1, "CSynergeticModel", "0.2", "CFilter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  
  parameters.initUnsignedLong( "RegionThreshold", 0, 0, 65535 );
  parameters.initDouble( "ChaoticGrowChance", 0.0, 0.0, 100.0 );
  parameters.initUnsignedLong( "Stability", 8, 0, 9 );
  parameters.initUnsignedLong( "TimeToLive", 1, 1, 254 );
  parameters.initDouble( "ReactivationStability", 1.5, 0.0, 10.0 );
	parameters.initUnsignedLong( "ShowEveryIteration", 0, 0, 1000 );

  inputsVec[0].portType = IO2DInteger;
  inputsVec[1].portType = IO1DVector;
  outputsVec[0].portType = IO2DInteger;

/* HB 28-06-05 */
/*theDialog.reset( new CSynergeticModelDialog );
	setModuleDialog( theDialog );*/
}

CSynergeticModel::~CSynergeticModel() throw()
{
}

CPipelineItem* CSynergeticModel::newInstance( ulong ulID ) const throw()
{
  return new CSynergeticModel( ulID );
}

void CSynergeticModel::apply() throw()
{

FBEGIN;
  srand( time( NULL ) );
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput<TImage>(inputPtr, 2, 2, 1, 1 ) )
	{
    return;
	}
  TField2DPtr seedPointsPtr = static_pointer_cast<TField2D>( getInput(1) );
  if ( seedPointsPtr.get() == NULL )//|| seedPointsPtr->getDimension() > 1 )
	{
		alog << LWARN << "No legal seed list" << endl;
    return;
	}
	bModuleReady = true;
  // Initialize fields
  size_t dims[] = { inputPtr->getExtent(0), inputPtr->getExtent(1) };
  theDialog->initImage( dims[0], dims[1] );
  
  TImagePtr outputPtr ( new TImage( 2, dims ) );
  (*outputPtr) = 0;
  TImage next( 2, dims );
  next.setMaximum( 255 );
  next.setMinimum( 0 );

  // Set the seedPointsPtr points
  for ( TField2D::iterator it = seedPointsPtr->begin(); it != seedPointsPtr->end(); ++it )
    (*outputPtr)( static_cast<ushort>( (*it)[0] ), static_cast<ushort>( (*it)[1] ) ) = 255;

  dGrowChance = parameters.getDouble( "ChaoticGrowChance" );
	dInitialGrowChance = parameters.getDouble( "ChaoticGrowChance" );
	dReactivationStability = parameters.getDouble( "ReactivationStability" );
	ulong ulStability = parameters.getUnsignedLong( "Stability" );
 	lTTL = parameters.getUnsignedLong( "TimeToLive" );	
	ulRegionThreshold = parameters.getUnsignedLong( "RegionThreshold" );
	ulong showEvery = parameters.getUnsignedLong( "ShowEveryIteration" );
  ulong ulTimestep = 0; 
	bool bFinished = false;
  ulong ulNoOfIterations = 0;
//  ulong ulNoOfPixels = 1; double dHomogenity = 0.0;

  // Computation loop
  while( !bFinished )
  {
    ulNoOfIterations++;
    if ( ulNoOfIterations % 10 == 0 )
      APP_PROC();
    ulChangedCells = 0;
    ulChaoticCells = 0;
    for ( uint y = 0; y < dims[1]; y++ )
      for ( uint x = 0; x < dims[0]; x ++ )
      {
        // 1st case: Cell belongs to region and is stable
        if ( (*outputPtr)( x, y ) == 255 )
        {
          next( x, y ) = 255; // Cell will stay stable in the next step
          // Add all homogenous neighbors that have cell state zero or initiate dendritic growing
          addHomogenousNeighbors( x, y, outputPtr, inputPtr, &next );
        }
        // 2nd case: Chaotic growing
        else if ( (*outputPtr)( x, y ) > (255 - lTTL ) )
        {
          ushort val = validNeigh(x,y,1,outputPtr,255);

          if ( val > ulStability )
          {
            next( x, y ) = 255;
            (*outputPtr)( x, y ) = 255;
            ulChangedCells++;
          }

          addChaoticNeighbors( x, y, outputPtr, inputPtr, &next );

          // If still unstable decrease time to live
          if ( (*outputPtr)( x, y ) != 255
            && (*outputPtr)(x,y) > (255 - lTTL ) )
          {
            next( x, y ) = (*outputPtr)( x, y ) - 1;
            ulChaoticCells++;
          }
        }
        // 4th case: Simply copy the cell input
        else if ( (*outputPtr)( x, y ) == (255 - lTTL)
          && next(x,y) == 0 )
        {
          next( x, y ) = (*outputPtr)( x, y );
          // Check for stability
          ulong stable = validNeigh( x, y, 2, outputPtr, 255 );
          if ( stable >= ulStability * dReactivationStability )
          {
            next( x, y ) = 255;
            (*outputPtr)( x, y ) = 255;
            // Reactivate all dead neighbors
            if ( x > 0 && (*outputPtr)( x - 1, y ) == (255 -lTTL) )
              next( x - 1, y ) = 254;
            if ( x+1 < dims[0] && (*outputPtr)( x + 1, y ) == (255 -lTTL)
)
              next( x + 1, y ) = 254;
            if ( y > 0 && (*outputPtr)( x, y - 1 ) == (255 -lTTL) )
              next( x, y - 1 ) = 254;
            if ( y+1 < dims[1] && (*outputPtr)( x, y + 1 ) == (255 -lTTL)
)
              next( x, y + 1 ) = 254;
            ulChangedCells++;
          }
        }
      }
    // View actual lattice and clear for next timestep
    (*outputPtr) = next;
    next = 0;
		if ( showEvery > 0 && ( ulNoOfIterations % showEvery ) == 0 )
    	theDialog->setImage( inputPtr, outputPtr, lTTL );
    // Test for convergence
    if ( ulChangedCells == 0 )
    {
      if ( ulChaoticCells == 0 )
      {
        ulTimestep++;
      }
    }
    else
    {
      ulTimestep = 0;
    }
    if ( ulTimestep > static_cast<ulong>( lTTL ) )
      bFinished = true;

  }
  outputPtr->setMaximum( 1 );
  for ( uint y = 0; y < dims[1]; ++y )
    for ( uint x = 0; x < dims[0]; ++x )
      if ( (*outputPtr)( x, y ) == 255 )
        (*outputPtr)( x, y ) = 1;
      else
        (*outputPtr)( x, y ) = 0;
  setOutput( outputPtr );
FEND;

}

bool CSynergeticModel::staticGrowth( short src, short tgt, short &tgtnext, short& ) throw()
{
  if ( isInRange( src, tgt, ulRegionThreshold ) )
  {
    tgtnext = 255;
    ulChangedCells++;
    return true;
  }
  return false;
}

void CSynergeticModel::chaoticGrowth( short source, short target, short &tgtnext,
  short amount ) throw()
{
  if ( tgtnext == 0 )
  {
    dGrowChance = static_cast<float>( 255 - ( source - target ) ) / 255.0
      * dGrowChance;
    float r = (static_cast<float>(rand())/static_cast<float>(RAND_MAX));
    if (  r <= dInitialGrowChance )
    {
      if ( amount < (255 - lTTL) )
        amount = (255 - lTTL);
      tgtnext = amount;
      ulChaoticCells++;
    }
  }
}

ushort CSynergeticModel::validNeigh( short x, short y, short radius, TImagePtr inputPtr,
  ushort value )
{
  short ax = max( x-radius, 0 );
  short ex = min( x+radius, inputPtr->getExtent(0)-1 );
  short ay = max( y-radius, 0 );
  short ey = min( y+radius, inputPtr->getExtent(1)-1 );
  ushort val = 0; ushort z = 0;
  for ( int i = ax; i <= ex; i++ )
    for ( int j = ay; j <= ey; j++ )
    {
      if ( (*inputPtr)(i,j) == value ) val++;
      z++;
    }
  if (val>0) val--;
  return val;
}

void CSynergeticModel::addHomogenousNeighbors( ushort x, ushort y, TImagePtr outputPtr,
TImagePtr inputPtr, TImage* next )
{
  if ( x > 0  && (*outputPtr)( x - 1, y ) != 255 )
    if ( !staticGrowth( (*inputPtr)( x - 1, y ), (*inputPtr)( x, y ),
      (*next)( x - 1, y ), (*outputPtr)( x - 1, y ) )
      && (*outputPtr)( x - 1, y ) == 0 && (*next)( x - 1, y ) == 0 )
      chaoticGrowth( 255, (*inputPtr)( x - 1, y ), (*next)( x - 1, y ), 254 );
  if ( ( x < inputPtr->getExtent(0) - 1 )
    && ( (*outputPtr)( x + 1, y ) != static_cast<ushort>(255) ) )
    if ( !staticGrowth( (*inputPtr)( x + 1, y ), (*inputPtr)( x, y ),
      (*next)( x + 1, y ), (*outputPtr)( x + 1, y ) )
      && (*outputPtr)( x + 1, y ) == 0 && (*next)( x + 1, y ) == 0 )
      chaoticGrowth( 255, (*inputPtr)( x + 1, y ), (*next)( x + 1, y ), 254 );
  if ( y > 0 && (*outputPtr)( x, y - 1 ) != 255 )
    if ( !staticGrowth( (*inputPtr)( x, y - 1 ), (*inputPtr)( x, y ),
      (*next)( x, y - 1 ), (*outputPtr)( x, y - 1 ) )
      && (*outputPtr)( x, y - 1 ) == 0 && (*next)( x, y - 1 ) == 0 )
      chaoticGrowth( 255, (*inputPtr)( x, y - 1 ), (*next)( x, y - 1 ), 254 );
  if ( ( y < inputPtr->getExtent(1) - 1 )
    && (*outputPtr)( x, y + 1 ) != static_cast<ushort>(255) )
    if ( !staticGrowth( (*inputPtr)( x, y + 1 ), (*inputPtr)( x, y ),
      (*next)( x, y + 1 ), (*outputPtr)( x, y + 1 ) )
      && (*outputPtr)( x, y + 1 ) == 0 && (*next)( x, y + 1 ) == 0 )
      chaoticGrowth( 255, (*inputPtr)( x, y + 1 ), (*next)( x, y + 1 ), 254 );
}

void CSynergeticModel::addChaoticNeighbors( ushort x, ushort y, TImagePtr outputPtr,
TImagePtr inputPtr, TImage* next )
{
  if ( x > 0 && (*outputPtr)( x - 1, y ) == 0 && (*next)( x - 1, y ) == 0 )
    chaoticGrowth( (*inputPtr)( x, y ), (*inputPtr)( x - 1, y ), (*next)( x - 1, y ),
      (*outputPtr)( x, y ) - 1 );
  if ( ( x < inputPtr->getExtent(0) - 1 )
    && (*outputPtr)( x + 1, y ) == 0
    && (*next)( x + 1, y ) == 0 )
    chaoticGrowth( (*inputPtr)( x, y ), (*inputPtr)( x + 1, y ), (*next)( x + 1, y ),
      (*outputPtr)( x, y ) - 1 );
  if ( y > 0 && (*outputPtr)( x, y - 1 ) == 0 && (*next)( x, y - 1 ) == 0 )
    chaoticGrowth( (*inputPtr)( x, y ), (*inputPtr)( x, y - 1 ), (*next)( x, y - 1 ),
      (*outputPtr)( x, y ) - 1 );
  if ( ( y < inputPtr->getExtent(1) - 1 )
    && (*outputPtr)( x, y + 1 ) == 0
    && (*next)( x, y + 1 ) == 0 )
    chaoticGrowth( (*inputPtr)( x, y ), (*inputPtr)( x, y + 1 ), (*next)( x, y + 1 ),
      (*outputPtr)( x, y ) - 1 );
}

bool CSynergeticModel::isInRange( short intensity, short target, short range ) const throw()
{
  return ( ( intensity >= ( target - range ) ) && ( intensity <= ( target + range ) ) );
}
