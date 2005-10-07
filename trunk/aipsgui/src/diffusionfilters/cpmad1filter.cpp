/***********************************************************************
 * File: cpmad1filter.cpp                                              *
 * Project: AIPS - Diffusion filter plugin library                     *
 * Description: Smooting Filter based on the 1st Perona-Malik function *
 *              with variable number of iterations, diffusion factor   *
 *              and Integration constant (lambda)                      *
 *                                                                     *
 * Author: Joaquin Castellanos                                         *                                                                     *
 * Created: 2004-04-23                                                 *
 ***********************************************************************/
 
#include "cpmad1filter.h"
#include "cmath"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/* \param ulID unique module ID */
CPMAD1Filter::CPMAD1Filter( ulong ulID ) throw()
  : CFilter ( ulID, "PMAD1 filter", 1, 1, "CPMAD1Filter", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Filters the input image with the 2nd. \n"  
                   " Perona Malik filter\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Iterations: number of iterations\n"
                   "k: diffusion factor\n"
                   "lambda: Integration constant\n"
                   "       2D Max_lambda = 0.2    @ 4N  \n"
                   "       2D Max_lambda = 0.142  @ 8N  \n"
                   "       3D Max_lambda = 0.142  @ 6N  \n"
                   "       3D Max_lambda = 0.0769 @ 18N \n"
                   "       3D Max_lambda = 0.068  @ 26N \n"   
                   "Neighbour: Type of elements to be consider\n"
                   "       0 = Orthogonal Neigbours (N,S,W,...)\n"
                   "       1 = Parallel/Ortogonal Planes \n"
                   "       2 = Complete neighbourhood \n";
 
  parameters.initUnsignedLong( "Iterations", 1UL, 1UL, 10000UL );
  parameters.initDouble( "k", 10.0, 0.01, 1000000.0 );
  parameters.initDouble( "lambda", 0.125, 0.001, 0.2 );
  parameters.initUnsignedLong( "Neighbourhood", 0UL, 0UL, 2UL );
}

CPMAD1Filter::~CPMAD1Filter() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CPMAD1Filter::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  if ( typeid( *(getInput()) ) == typeid( TImage ) )
		filter<TImage>();
	else if ( typeid( *(getInput()) ) == typeid( TField ) )
		filter<TField>();		
	else
		alog << LWARN << SERROR( "No input or wrong data type" ) << endl;
BENCHSTOP;
}

CPipelineItem* CPMAD1Filter::newInstance( ulong ulID ) const throw()
{
  return new CPMAD1Filter( ulID );
}

template<typename T> void CPMAD1Filter::filter() throw()
{
	if ( getInput()->getType() != typeid( typename dataTraits<T>::dataType ) )
		return;
  shared_ptr<T> inputPtr = static_pointer_cast<T>( getInput() );

/*  if ( !checkInput( inputPtr, 2, 3 ) )
    return;*/
	bModuleReady = true;
  deleteOldOutput();

	typedef typename dataTraits<T>::dataType TVoxel;
	typedef typename dataTraits<T>::increasedRangeType TInc;

  size_t dimensionSize[3];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  if ( inputPtr->getDimension() == 3 )
    dimensionSize[2] = inputPtr->getExtent(2);
  else
    dimensionSize[2] = 1;
  
	typename T::iterator auxIterator = inputPtr->begin();
	
/* Output Pointer Creation  */
  shared_ptr<T> outputPtr ( new T( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
/* Output Range Definition  */
  outputPtr->setMaximum( inputPtr->getMaximum() );
  outputPtr->setMinimum( inputPtr->getMinimum() );  

/* Generation of an Image data copie */
  T ImageCopie = (*inputPtr);
 

// Filter Parameters 
  ulong ulRadius =  1;    

	ulong  ulIterations = parameters.getUnsignedLong( "Iterations" );
  double dK_factor    = parameters.getDouble( "k" );
  double dlambda      = parameters.getDouble( "lambda" );
  ulong  ulNeighbour  = parameters.getUnsignedLong( "Neighbourhood" );


// Integration factor (lambda) 
  if ( inputPtr->getDimension() == 3 )
   {
    if( (ulNeighbour == 0 ) && (dlambda > 0.142  ))   dlambda = 0.142;   
    if( (ulNeighbour == 1 ) && (dlambda > 0.0769 ))   dlambda = 0.0769;
    if( (ulNeighbour >  1 ) && (dlambda > 0.068  ))   dlambda = 0.068;
   }
   else
   {
    if( (ulNeighbour  > 0 ) && (dlambda > 0.142  ))   dlambda = 0.142;
   }

   TVectorKE dDeltaN;
   double TotalFlux = 0;
   ulong ulLayer = dimensionSize[1] * dimensionSize[0];
   ulong slice = dimensionSize[0] * dimensionSize[1];
   ulong line = dimensionSize[0];
  
   ptrdiff_t nb[] = {-slice, -line, line, -1, 1, slice, 
                     -slice-line, -slice+line, -slice-1, -slice+1,
                     -line-1, line-1, -line+1, line+1, 
                      slice-line, slice+line, slice-1, slice+1,
                     -slice-line-1, -slice+line-1, -slice-line+1,-slice+line+1,
                      slice-line-1, slice+line-1, slice-line+1, slice+line+1,};
               
// Case 3D 6 Neighbours 
  if ( ( inputPtr->getDimension() == 3 ) &&  ( ulNeighbour == 0) )
	{
		PROG_MAX( ulIterations * inputPtr->getDataDimension() );
    for ( ulong ulitera = 0; ulitera < ulIterations; ++ulitera )    // Iterations 
    {
     for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
       for ( ushort z = ulRadius; z < ( dimensionSize[2] - ulRadius ); ++z )
       {
        APP_PROC();
				PROG_VAL( ( usChannel + 1 ) * ( ulitera + 1 ) );
        for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
         	for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
         	{
          	TotalFlux = 0;

            auxIterator = ImageCopie.begin() + x + ( y * dimensionSize[0] ) + ( z * ulLayer );

            dDeltaN(0) = *auxIterator -  *(auxIterator + nb[0]);            /* deltaUC */
            dDeltaN(1) = *auxIterator -  *(auxIterator + nb[1]);            /* deltaCN */
            dDeltaN(2) = *auxIterator -  *(auxIterator + nb[2]);            /* deltaCS */
            dDeltaN(3) = *auxIterator -  *(auxIterator + nb[3]);            /* deltaCE */
            dDeltaN(4) = *auxIterator -  *(auxIterator + nb[4]);            /* deltaCW */
            dDeltaN(5) = *auxIterator -  *(auxIterator + nb[5]);            /* deltaUC */

            for (ushort indice=0; indice < 6; ++indice)
              TotalFlux = TotalFlux - ( dDeltaN( indice ) 
								* ( exp( -pow( ( std::abs( static_cast<double>( dDeltaN( indice ) ) / dK_factor ) ), 2 ) ) ) );

            TInc flow_value = static_cast<TInc>( TotalFlux * dlambda );
            TInc pixelResult = (ImageCopie)( x, y, z, usChannel ) + flow_value;
						if ( pixelResult > numeric_limits<TVoxel>::max() )
							pixelResult = numeric_limits<TVoxel>::max();
						else if ( pixelResult < numeric_limits<TVoxel>::min() )
							pixelResult = numeric_limits<TVoxel>::min();

            if( outputPtr->getMaximum() < pixelResult )
                  outputPtr->setMaximum( pixelResult );
            if( outputPtr->getMinimum() > pixelResult )
                  outputPtr->setMinimum( pixelResult );

            (*outputPtr)( x, y, z, usChannel ) = pixelResult;
         }
       }
       // Input refresh
       ImageCopie.swap(*outputPtr);
    }
  } 
	
// Case 3D 18 Neighbours 
  if ( ( inputPtr->getDimension() == 3 ) &&  ( ulNeighbour == 1 ) )
	{
		PROG_MAX( (ulIterations) * (inputPtr->getDataDimension()) );	
    for ( ulong ulitera = 0; ulitera < ulIterations; ++ulitera )    // Iterations 
    {
     for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
       for ( ushort z = ulRadius; z < ( dimensionSize[2] - ulRadius ); ++z )
       {
        APP_PROC();
				PROG_VAL( ( usChannel + 1 ) * ( ulitera + 1 ) );
        for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
         	for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
         	{
          	TotalFlux = 0;

            auxIterator = ImageCopie.begin() + x + ( y * dimensionSize[0] ) + ( z * ulLayer );

            dDeltaN(0) = *auxIterator -  *(auxIterator + nb[0]);        /* deltaUC */
            dDeltaN(1) = *auxIterator -  *(auxIterator + nb[1]);        /* deltaCN */
            dDeltaN(2) = *auxIterator -  *(auxIterator + nb[2]);        /* deltaCS */
            dDeltaN(3) = *auxIterator -  *(auxIterator + nb[3]);        /* deltaCE */
            dDeltaN(4) = *auxIterator -  *(auxIterator + nb[4]);        /* deltaCW */
            dDeltaN(5) = *auxIterator -  *(auxIterator + nb[5]);        /* deltaUC */

            dDeltaN(6) = *auxIterator -  *(auxIterator + nb[6]);        /* deltaUN */
            dDeltaN(7) = *auxIterator -  *(auxIterator + nb[7]);        /* deltaUS */
            dDeltaN(8) = *auxIterator -  *(auxIterator + nb[8]);        /* deltaUE */
            dDeltaN(9) = *auxIterator -  *(auxIterator + nb[9]);        /* deltaUW */

            dDeltaN(10) = *auxIterator -  *(auxIterator + nb[10]);      /* deltaCNE */
            dDeltaN(11) = *auxIterator -  *(auxIterator + nb[11]);      /* deltaCSE */
            dDeltaN(12) = *auxIterator -  *(auxIterator + nb[12]);      /* deltaCNW */
            dDeltaN(13) = *auxIterator -  *(auxIterator + nb[13]);      /* deltaCSW */

            dDeltaN(14) = *auxIterator -  *(auxIterator + nb[14]);      /* deltaUN */
            dDeltaN(15) = *auxIterator -  *(auxIterator + nb[15]);      /* deltaUS */
            dDeltaN(16) = *auxIterator -  *(auxIterator + nb[16]);      /* deltaUE */
            dDeltaN(17) = *auxIterator -  *(auxIterator + nb[17]);      /* deltaUW */

            for (ushort indice = 0; indice < 6; ++indice )
              TotalFlux = TotalFlux - ( dDeltaN( indice ) 
								* ( exp( -pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );
            for (ushort indice = 6; indice < 18; ++indice )
              TotalFlux = TotalFlux - 0.5 * ( dDeltaN( indice ) 
								* ( exp( -pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );

            TInc flow_value = static_cast<TInc>( TotalFlux * dlambda );
            TInc pixelResult = (ImageCopie)( x, y, z, usChannel ) + flow_value;
						if ( pixelResult > numeric_limits<TVoxel>::max() )
							pixelResult = numeric_limits<TVoxel>::max();
						else if ( pixelResult < numeric_limits<TVoxel>::min() )
							pixelResult = numeric_limits<TVoxel>::min();

            if( outputPtr->getMaximum() < pixelResult )
                  outputPtr->setMaximum( pixelResult );
            if( outputPtr->getMinimum() > pixelResult )
                  outputPtr->setMinimum( pixelResult );

            (*outputPtr)( x, y, z, usChannel ) = pixelResult;
         }
       }
       // Input refresh
       ImageCopie.swap(*outputPtr);
    }
	}

// Case 3D 26 Neighbours 
  if ( ( inputPtr->getDimension() == 3 ) &&  ( ulNeighbour == 2 ) )
	{
		PROG_MAX( (ulIterations) * (inputPtr->getDataDimension()) );
    for ( ulong ulitera = 0; ulitera < ulIterations; ++ulitera )    // Iterations 
    {
     for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
       for ( ushort z = ulRadius; z < ( dimensionSize[2] - ulRadius ); ++z )
       {
        APP_PROC();
				PROG_VAL( ( usChannel + 1 ) * ( ulitera + 1 ) );
        for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
         	for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
         	{
         		TotalFlux = 0;

            auxIterator = ImageCopie.begin() + x + ( y * dimensionSize[0] ) + ( z * ulLayer );

            dDeltaN(0) = *auxIterator -  *(auxIterator + nb[0]);        /* deltaUC */
            dDeltaN(1) = *auxIterator -  *(auxIterator + nb[1]);        /* deltaCN */
            dDeltaN(2) = *auxIterator -  *(auxIterator + nb[2]);        /* deltaCS */
            dDeltaN(3) = *auxIterator -  *(auxIterator + nb[3]);        /* deltaCE */
            dDeltaN(4) = *auxIterator -  *(auxIterator + nb[4]);        /* deltaCW */
            dDeltaN(5) = *auxIterator -  *(auxIterator + nb[5]);        /* deltaUC */

            dDeltaN(6) = *auxIterator -  *(auxIterator + nb[6]);        /* deltaUN */
            dDeltaN(7) = *auxIterator -  *(auxIterator + nb[7]);        /* deltaUS */
            dDeltaN(8) = *auxIterator -  *(auxIterator + nb[8]);        /* deltaUE */
            dDeltaN(9) = *auxIterator -  *(auxIterator + nb[9]);        /* deltaUW */

            dDeltaN(10) = *auxIterator -  *(auxIterator + nb[10]);      /* deltaCNE */
            dDeltaN(11) = *auxIterator -  *(auxIterator + nb[11]);      /* deltaCSE */
            dDeltaN(12) = *auxIterator -  *(auxIterator + nb[12]);      /* deltaCNW */
            dDeltaN(13) = *auxIterator -  *(auxIterator + nb[13]);      /* deltaCSW */

            dDeltaN(14) = *auxIterator -  *(auxIterator + nb[14]);      /* deltaUN */
            dDeltaN(15) = *auxIterator -  *(auxIterator + nb[15]);      /* deltaUS */
            dDeltaN(16) = *auxIterator -  *(auxIterator + nb[16]);      /* deltaUE */
            dDeltaN(17) = *auxIterator -  *(auxIterator + nb[17]);      /* deltaUW */

            dDeltaN(18) = *auxIterator -  *(auxIterator + nb[18]);      /* deltaUNE */
            dDeltaN(19) = *auxIterator -  *(auxIterator + nb[19]);      /* deltaUSE */
            dDeltaN(20) = *auxIterator -  *(auxIterator + nb[20]);      /* deltaUNW */
            dDeltaN(21) = *auxIterator -  *(auxIterator + nb[21]);      /* deltaUSW */

            dDeltaN(22) = *auxIterator -  *(auxIterator + nb[22]);      /* deltaLNE */
            dDeltaN(23) = *auxIterator -  *(auxIterator + nb[23]);      /* deltaLSE */
            dDeltaN(24) = *auxIterator -  *(auxIterator + nb[24]);      /* deltaLNW */
            dDeltaN(25) = *auxIterator -  *(auxIterator + nb[25]);      /* deltaLSW */            

            for ( ushort indice = 0; indice < 6; ++indice )
              TotalFlux = TotalFlux - ( dDeltaN( indice ) 
								* ( exp( -pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );
            for ( ushort indice = 6; indice < 18; ++indice )
              TotalFlux = TotalFlux - 0.5 * ( dDeltaN( indice ) 
								* ( exp( -pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );
            for ( ushort indice = 18; indice < 26; ++indice )
              TotalFlux = TotalFlux - 0.333 * ( dDeltaN( indice) 
								* ( exp( -pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );

            TInc flow_value = static_cast<TInc>( TotalFlux * dlambda );
            TInc pixelResult = (ImageCopie)( x, y, z, usChannel ) + flow_value;
						if ( pixelResult > numeric_limits<TVoxel>::max() )
							pixelResult = numeric_limits<TVoxel>::max();
						else if ( pixelResult < numeric_limits<TVoxel>::min() )
							pixelResult = numeric_limits<TVoxel>::min();

            if( outputPtr->getMaximum() < pixelResult )
                  outputPtr->setMaximum( pixelResult );
            if( outputPtr->getMinimum() > pixelResult )
                  outputPtr->setMinimum( pixelResult );

            (*outputPtr)( x, y, z, usChannel ) = pixelResult;
         }
       }
       // Input refresh
       ImageCopie.swap(*outputPtr);
    }
	}
	
// Case 2D 4 Neighbours 
  if ( ( inputPtr->getDimension() == 2 ) &&  ( ulNeighbour == 0) )
	{
		PROG_MAX( ulIterations * inputPtr->getDataDimension() );	
    for ( ulong ulitera = 0; ulitera < ulIterations; ++ulitera )    // Iterations 
    {
     for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
     {
      APP_PROC();
			PROG_VAL( ( usChannel + 1 ) * ( ulitera + 1 ) );	
      for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
        for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
         {
            TotalFlux = 0;
            auxIterator = ImageCopie.begin() + x + ( y * dimensionSize[0] );

            dDeltaN(0) = *auxIterator -  *(auxIterator - ( dimensionSize[0] ) );/* deltaN */
            dDeltaN(1) = *auxIterator -  *(auxIterator + ( dimensionSize[0] ) );/* deltaS */
            dDeltaN(2) = *auxIterator -  *(auxIterator - 1 );                  /* deltaE */
            dDeltaN(3) = *auxIterator -  *(auxIterator + 1 );                  /* deltaW */

            for ( ushort indice = 0; indice < 4; ++indice )
              TotalFlux = TotalFlux - ( dDeltaN( indice ) 
								* ( exp( -pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );

            TInc flow_value = static_cast<TInc>( TotalFlux * dlambda );
            TInc pixelResult = (ImageCopie)( x, y, usChannel ) + flow_value;
						if ( pixelResult > numeric_limits<TVoxel>::max() )
							pixelResult = numeric_limits<TVoxel>::max();
						else if ( pixelResult < numeric_limits<TVoxel>::min() )
							pixelResult = numeric_limits<TVoxel>::min();

            if( outputPtr->getMaximum() < pixelResult )
                  outputPtr->setMaximum( pixelResult );
            if( outputPtr->getMinimum() > pixelResult )
                  outputPtr->setMinimum( pixelResult );

            (*outputPtr)( x, y, usChannel ) = pixelResult;
         }
     }
     // Input refresh
     ImageCopie.swap(*outputPtr);
    }
	}
	
// Case 2D 8 Neighbours 
  if ( ( inputPtr->getDimension() == 2 ) &&  ( ulNeighbour > 0) )
	{
		PROG_MAX( (ulIterations) * (inputPtr->getDataDimension()) );	
    for ( ulong ulitera = 0; ulitera < ulIterations; ++ulitera )    // Iterations 
    {
     for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
     {
      APP_PROC();
			PROG_VAL( ( usChannel + 1 ) * ( ulitera + 1 ) );	
      for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
        for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
         {
            TotalFlux = 0;
            auxIterator = ImageCopie.begin() + x + ( y * dimensionSize[0]);

            dDeltaN(0) = *auxIterator -  *(auxIterator - ( dimensionSize[0]));/* deltaN */
            dDeltaN(1) = *auxIterator -  *(auxIterator + ( dimensionSize[0]));/* deltaS */
            dDeltaN(2) = *auxIterator -  *(auxIterator - 1);                  /* deltaE */
            dDeltaN(3) = *auxIterator -  *(auxIterator + 1);                  /* deltaW */

            dDeltaN(4) = *auxIterator -  *(auxIterator - ( dimensionSize[0]) - 1);/* deltaNE */
            dDeltaN(5) = *auxIterator -  *(auxIterator - ( dimensionSize[0]) + 1);/* deltaSE */
            dDeltaN(6) = *auxIterator -  *(auxIterator + ( dimensionSize[0]) - 1);/* deltaNW */
            dDeltaN(7) = *auxIterator -  *(auxIterator + ( dimensionSize[0]) + 1);/* deltaSW */
            
						/* HB 2004-06-16: Changed fabs to abs, since we use double values here */
            for (ushort indice=0; indice <4; ++indice)
              TotalFlux = TotalFlux - ( dDeltaN( indice ) 
								* ( exp( - pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );
            for (ushort indice=4; indice < 8; ++indice)
              TotalFlux = TotalFlux - 0.5*( dDeltaN(indice) 
								* ( exp( - pow( ( std::abs( dDeltaN( indice ) ) / dK_factor ), 2 ) ) ) );

            TInc flow_value = static_cast<TInc>( TotalFlux * dlambda );
            TInc pixelResult = ImageCopie( x, y, usChannel )+ flow_value;
						/* HB 2004-06-16: Inserted check for correct values */
						if ( pixelResult > numeric_limits<TVoxel>::max() )
							pixelResult = numeric_limits<TVoxel>::max();
						else if ( pixelResult < numeric_limits<TVoxel>::min() )
							pixelResult = numeric_limits<TVoxel>::min();

            if( outputPtr->getMaximum() < pixelResult )
                  outputPtr->setMaximum( pixelResult );

            if( outputPtr->getMinimum() > pixelResult )
                  outputPtr->setMinimum( pixelResult );

            (*outputPtr)( x,y, usChannel ) = pixelResult;						
         }
     }
     // Input refresh
		 /* HB 2004-06-16: Changed = to swap */
     ImageCopie.swap( *outputPtr );
    }
	}

/* Output Generation */

  setOutput( outputPtr );
	
PROG_RESET();
}
