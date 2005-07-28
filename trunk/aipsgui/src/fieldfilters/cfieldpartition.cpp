/************************************************************************
 * File: cfieldpartition.cpp                                            *
 * Project: AIPS                                                        *
 * Description: Tries to determine a useful partition of a vector field *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 03.02.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cfieldpartition.h"

using namespace std;
using namespace boost;

/**
 * Consrtuctor
 * \param ulID unique module ID
 */
CFieldPartition::CFieldPartition( ulong ulID ) throw()
  : CFilter ( ulID, "Partition operator", 1, 1, "CFieldPartition", "0.2", "CFilter" )
{
  setModuleID( sLibID );
  sDocumentation = "Tries to generate a useful partition of a vector field\n"
                   "** Input ports:\n"
                   "0: A vector field\n"
                   "**outputPtr ports:\n"
                   "1: A single channel image"
                   "** Parameters:\n"
                   "Intensity Range: Intensity range of resulting scalar image";

  parameters.initUnsignedLong( "IntensityRange", 255, 1, 65535 );
  parameters.initUnsignedLong( "Neighbourhood", 4, 4, 9 );

  inputsVec[0].portType = CPipelineItem::IOVector;
  outputsVec[0].portType = CPipelineItem::IOInteger;
}

/** Destructor */
CFieldPartition::~CFieldPartition() throw()
{
}

/** Reimplemented from CPipelineItem */
void CFieldPartition::apply() throw()
{
	if( getInput().get() != NULL && getInput()->getDimension() == 3 )
	{
		part3D();
		return;
	}
BENCHSTART;	
	bModuleReady = false;
  shared_ptr<TField2D> inputPtr = static_pointer_cast<TField2D>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() != 2 )
  {
    alog << LWARN << "Input type is no 2D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();
  
  shared_ptr<TImage> outputPtr ( new TImage( 2, inputPtr->getExtents() ) );
  outputPtr->setMinimum( 0 );
	ulong ulIntensityRange;
	ulIntensityRange = parameters.getUnsignedLong( "IntensityRange" );
  outputPtr->setMaximum( ulIntensityRange );
  ushort usWidth = inputPtr->getExtent(0);
  ushort usHeight = inputPtr->getExtent(1);
alog << LFIXME << "Using inner products here would allow us to work also on higher dimensions"
 << endl;
  vector<double> angleVec;
  angleVec.resize(9);
  // Neighbrohood
  ushort NB = parameters.getUnsignedLong( "Neighbourhood" );
DS("B");	
  if ( ( NB > 5 && NB < 8 ) || ( NB > 9 ) )
  {
    NB = 5;
		parameters.setUnsignedLong( "Neighbourhood", 5 );
  }
  for ( ushort x = 1; x < usWidth - 1; x++ )
    for ( ushort y = 1; y < usHeight - 1; y++ )
    {
      // Für jeden Punkt werden der aktuelle Vektor sowie seine acht Nachbarn betrachtet
      // Es werden die Vektoren ausgewählt, deren Richtungsdifferenz maximal ist
      // Diese Differenz ergibt den letztendlichen Wert im Ausgabefeld
      // Ziel ist es, ein Maß für die Homogenität des Vektorfeldes zu bestimmen      
      double dValue = 0.0;
      angleVec[0] = angle((*inputPtr)( x, y - 1 ));
      angleVec[1] = angle((*inputPtr)( x - 1, y ));
      angleVec[2] = angle((*inputPtr)( x, y + 1 ));
      angleVec[3] = angle((*inputPtr)( x + 1, y ));
      if ( NB == 5 )
        angleVec[4] = angle((*inputPtr)( x, y ));
      if ( NB > 5 )
      {
        angleVec[4] = angle((*inputPtr)( x - 1, y - 1 ));
        angleVec[5] = angle((*inputPtr)( x - 1, y + 1 ));
        angleVec[6] = angle((*inputPtr)( x + 1, y + 1 ));
        angleVec[7] = angle((*inputPtr)( x + 1, y - 1 ));
        if ( NB == 9 )
          angleVec[8] = angle((*inputPtr)( x, y ));
      }
      for ( ushort i = 0; i < NB-1; i++ )
        for ( ushort j = i + 1; j < NB; j++ )
        {
          double dTmp = abs( angleVec[i] - angleVec[j] );
          if ( angleVec[i] == -1.0 || angleVec[j] == -1.0 )
            dTmp = 0.0;
          if ( dTmp > M_PI )
            dTmp = 2.0 * M_PI - dTmp;
          if ( dTmp > dValue )
            dValue = dTmp;
        }
      (*outputPtr)( x, y ) = static_cast<ushort>( round( dValue / M_PI
        * static_cast<double>( ulIntensityRange ) ) );
    }
  angleVec.clear();
  setOutput( outputPtr );
BENCHSTOP;
}

void CFieldPartition::part3D()
{
BENCHSTART;
	bModuleReady = false;
  shared_ptr<TField3D> inputPtr = static_pointer_cast<TField3D>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() != 3 )
  {
    alog << LWARN << "Input type is no 3D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();
  
  shared_ptr<TImage> outputPtr ( new TImage( 3, inputPtr->getExtents() ) );
  outputPtr->setMinimum( 0 );
  outputPtr->setMaximum( parameters.getUnsignedLong( "IntensityRange" ) );
	double dIntensityRange = parameters.getDouble( "IntensityRange" );

  ushort usWidth = inputPtr->getExtent(0);
  ushort usHeight = inputPtr->getExtent(1);
	ushort usDepth = inputPtr->getExtent(2);
  vector<TVector3D> angleVec;
  angleVec.resize(6);
	PROG_MAX( usDepth - 2 );
  for ( ushort z = 1; z < usDepth - 1; ++z )
	{
		PROG_VAL( z );
		APP_PROC();
    for ( ushort y = 1; y < usHeight - 1; ++y )
  		for ( ushort x = 1; x < usWidth - 1; ++x )
    	{
	      // Fr jeden Punkt wird  die 6er-Nachbarschafts des aktuelle Vektors betrachtet
  	    // Es werden die Vektoren ausgewï¿½lt, deren Richtungsdifferenz maximal ist
    	  // Diese Differenz ergibt den letztendlichen Wert im Ausgabefeld
      	// Ziel ist es, ein Maï¿½fr die Homogenitï¿½ des Vektorfeldes zu bestimmen      
      	double dValue = 0.0;
	      angleVec[0] = (*inputPtr)( x - 1, y, z );
	      angleVec[1] = (*inputPtr)( x + 1, y, z );
	      angleVec[2] = (*inputPtr)( x, y - 1, z );
	      angleVec[3] = (*inputPtr)( x, y + 1, z );
	      angleVec[4] = (*inputPtr)( x, y, z - 1 );
	      angleVec[5] = (*inputPtr)( x, y, z + 1 );
				
	      for ( ushort i = 0; i < 5; i++ )
        for ( ushort j = i + 1; j < 6; j++ )
        {
          double dTmp = abs( acos( dot( angleVec[i], angleVec[j] ) 
						/ ( norm( angleVec[i] ) * norm( angleVec[j] ) ) ) );
          if ( dTmp > dValue )
            dValue = dTmp;
        }
      (*outputPtr)( x, y, z ) = static_cast<ushort>( round( dValue / M_PI
        * dIntensityRange ) );
    }
	}
  angleVec.clear();
  setOutput( outputPtr );
PROG_RESET();
BENCHSTOP;
}

/** Reimplemented from CPipelineItem */
CPipelineItem* CFieldPartition::newInstance( ulong ulID ) const throw()
{
  return new CFieldPartition( ulID );
}

/**
 * Determine the angle of a 2D vector
 * \param  aVector a 2D vector
 * \return angle
 */
double CFieldPartition::angle( TVector2D aVector ) throw()
{
  if ( aVector[0] == 0.0 && aVector[1] == 0.0 )
    return -1.0;
  if ( aVector[0] == 0.0 )
  {
    if ( aVector[1] > 0.0 ) return M_PI / 2.0;
    else return 3.0 * M_PI / 2.0;
  }
  if ( aVector[0] > 0.0 && aVector[1] >= 0.0 )
    return atan( aVector[1] / aVector[0] );
  if ( aVector[0] < 0.0 )
    return atan( aVector[1] / aVector[0] ) + M_PI ;
  return atan( aVector[1] / aVector[0] ) + 2.0 * M_PI ;
}
