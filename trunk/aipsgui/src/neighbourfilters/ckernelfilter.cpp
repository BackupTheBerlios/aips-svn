/************************************************************************
 * File: ckernelfilter.cpp                                              *
 * Project: AIPS - Neighbourhood filters plugin library                 *
 * Description: Base class of all filters that use convolution          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2003-11-13                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "ckernelfilter.h"

using namespace std;
using namespace boost;
#ifdef USE_BLITZ
using namespace blitz;
#endif

/*************
 * Structors *
 *************/

/**
 * \param ulID Module id
 * \param sName Module name
 * \param usNoOfParameters Number of module parameters
 */
CKernelFilter::CKernelFilter( ulong ulID, const string& sName,
		const ushort usNoOfInputs, const ushort usNoOfOutputs,
		const std::string &sClassName_, const std::string &sClassVersion_, 
		const std::string &sDerivedFrom_  )
  throw() : CFilter ( ulID, sName, usNoOfInputs, usNoOfOutputs, sClassName_, sClassVersion_, sDerivedFrom_ )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
}

/** Destructor */
CKernelFilter::~CKernelFilter() throw()
{
}

/*****************
 * Other methods *
 *****************/

#ifdef USE_BLITZ
/**
 * This method will NOT check the type and dimension of the data field!
 * \param dataPtr data set ( Scalar volume )
 * \param kernel filter kernel 
 */
TImagePtr CKernelFilter::applyKernel( TImagePtr dataPtr, Array<double, 3>& kernel ) throw()
{
BENCHSTART;

  size_t usDims[] = { dataPtr->getExtent(0), dataPtr->getExtent(1), 
		dataPtr->getExtent(2) };
  TImagePtr outputPtr ( new TImage( 3, dataPtr->getExtents(), 
		dataPtr->getDataDimension() ) );
	outputPtr->setMinimum( dataPtr->getMinimum() );
	outputPtr->setMaximum( dataPtr->getMaximum() );

  ushort usRadius = ( kernel.cols() - 1 ) / 2; // Filter kernel size

DBG2("Starting filter BLITZ++");
	PROG_MAX( dataPtr->getDataDimension() );

  for ( ushort usChannels = 0; usChannels < dataPtr->getDataDimension(); 
		++usChannels )
  {
		PROG_VAL( usChannels + 1 );	
    Array<short, 3> work ( dataPtr->getArray( usChannels ),
      shape(usDims[0], usDims[1], usDims[2]), neverDeleteData, AIPSArray<3>() );

    Array<short, 3> output ( outputPtr->getArray( usChannels ),
      shape(usDims[0], usDims[1], usDims[2]), neverDeleteData, AIPSArray<3>() );

    for ( Array<short, 3>::iterator it = work.begin(); it != work.end(); ++it )
    {
      TinyVector<int, 3> position = it.position();
      double dTmp = 0;
      for ( int k = position[0] - usRadius; k < ( position[0] + usRadius + 1 ); ++k )
        for ( int l = position[1] - usRadius; l < ( position[1] + usRadius + 1 ); ++l )
          for ( int m = position[2] - usRadius; m < ( position[2] + usRadius + 1 ); ++m )
      {
        int i = k + usRadius - position[0];
        int j = l + usRadius - position[1];
        int n = m + usRadius - position[2];
        if ( k < work.rows() && k > -1 && l < work.cols()
          && l > -1 && m < work.depth() && m > -1 )
          dTmp += work( k, l, m ) * kernel( i, j, n );
      }
      output( position ) = static_cast<ushort>( std::abs( static_cast<long>( dTmp ) ) );
      if ( output( position ) > outputPtr->getMaximum() )
        outputPtr->setMaximum( output( position ) );
			else if ( output( position ) < outputPtr->getMinimum() )
        outputPtr->setMinimum( output( position ) );
    }
  }
	PROG_RESET();	
BENCHSTOP;
  return outputPtr;
}

/**
 * This method will NOT check the type and dimension of the data field!
 * \param dataPtr A given data set ( Scalar image )
 * \param theKernel Filter kernel
 */
TImagePtr CKernelFilter::applyKernel( TImagePtr dataPtr, Array<double, 2>& kernel ) throw()
{
BENCHSTART;

	size_t usDims[] = { dataPtr->getExtent(0), dataPtr->getExtent(1) };

	TImagePtr outputPtr ( new TImage( 2, dataPtr->getExtents(), dataPtr->getDataDimension() ) );
	outputPtr->setMinimum( dataPtr->getMinimum() );
	outputPtr->setMaximum( dataPtr->getMaximum() );

  ushort usRadius = ( kernel.cols() - 1 ) / 2; // Filter kernel size
	PROG_MAX( dataPtr->getDataDimension() );
  for ( ushort usChannels = 0; usChannels < dataPtr->getDataDimension(); ++usChannels )
  {		
		PROG_VAL( usChannels + 1 );
    Array<short,2> work ( dataPtr->getArray( usChannels ),
      shape(usDims[0], usDims[1]), neverDeleteData, AIPSArray<2>() );

    Array<short,2> output ( outputPtr->getArray( usChannels ),
      shape(usDims[0], usDims[1]), neverDeleteData, AIPSArray<2>() );

		Array<short, 2>::iterator oit = output.begin();
    for ( Array<short, 2>::iterator it = work.begin(); it != work.end(); ++it, ++oit )
    {
      TinyVector<int, 2> position = it.position();
      double dTmp = 0;
      for ( int k = position(0) - usRadius; k < ( position(0) + usRadius + 1 ); ++k )
        for ( int l = position(1) - usRadius; l < ( position(1) + usRadius + 1 ); ++l )
      {
        int i = k + usRadius - position(0);
        int j = l + usRadius - position(1);
        if ( k < work.rows() && k > -1 && l < work.cols() && l > -1 )
          dTmp += static_cast<double>( work( k, l ) ) * kernel( i, j );
      }
      (*oit) = static_cast<ushort>( std::abs( static_cast<long>( dTmp ) ) );
      if ( (*oit) > outputPtr->getMaximum() )
        outputPtr->setMaximum( (*oit) );
			else if ( (*oit) < outputPtr->getMinimum() )
        outputPtr->setMinimum( (*oit) );
    }
  }
PROG_RESET();	
BENCHSTOP;
  return outputPtr;
}

#endif /* USE_BLITZ */

/**
 * This method will NOT check the type and dimension of the data field!
 * \param dataPtr A given data set ( Scalar image )
 * \param theKernel Filter kernel
 */
TImagePtr CKernelFilter::applyKernel ( TImagePtr dataPtr, CTypedData<double>& kernel ) throw()
{
BENCHSTART;
  size_t usDims[3];
  usDims[0] = dataPtr->getExtent(0);
  usDims[1] = dataPtr->getExtent(1);
  if ( dataPtr->getDimension() == 3 )
    usDims[2] = dataPtr->getExtent(2);
  else
    usDims[2] = 1;
  TImagePtr outputPtr ( new TImage( dataPtr->getDimension(), 
		dataPtr->getExtents(), dataPtr->getDataDimension() ) );
	outputPtr->setMinimum( dataPtr->getMinimum() );
	outputPtr->setMaximum( dataPtr->getMaximum() );		
	(*outputPtr) = 0;
  ushort usRadius[3] ={ ( kernel.getExtent(0) - 1 ) / 2,
		( kernel.getExtent(1) - 1 ) / 2 }; // Filter kernel size
	if ( kernel.getDimension() == 3 )
		usRadius[2] = ( kernel.getExtent(2) - 1 ) / 2;
	else
		usRadius[2] = 0;
	ulong lineSize = ( dataPtr->getExtent(0) );
	ulong sliceSize = ( dataPtr->getExtent(0) *  dataPtr->getExtent(1) );
	short x = 0;
	short y = 0;
	short z = 0;
	ushort posVecSize = kernel.getExtent(0) * kernel.getExtent(1);
	if ( dataPtr->getDimension() > 2 )
		posVecSize *= kernel.getExtent(2);
	vector<ptrdiff_t> kernelPositionsVec( posVecSize );	
	long lKernelWidth = static_cast<long>( kernel.getExtent(0) );
	long lKernelHeight = static_cast<long>( kernel.getExtent(1) );	
	if ( usRadius[2] == 0 )
	while( y < lKernelHeight )
	{
		kernelPositionsVec[ x + y * lKernelWidth
			+ z * lKernelWidth * lKernelHeight ] = 
			  x - usRadius[0] 
			+ ( y - usRadius[1] ) * usDims[0] 
			+	( z - usRadius[2] ) * usDims[0] * usDims[1];
		++x;
		if ( x == lKernelWidth )
		{
			x = 0; ++y;
		}
	}
	else
	{
		long lKernelDepth = static_cast<long>( kernel.getExtent(2) );
		while( z < lKernelDepth )
		{
			kernelPositionsVec[ x + y * lKernelWidth 
				+ z * lKernelWidth * lKernelHeight ] = 
				x - usRadius[0] 
				+ ( y - usRadius[1] ) * usDims[0] 
				+	( z - usRadius[2] ) * usDims[0] * usDims[1];
			++x;
			if ( x == lKernelWidth )
			{
				x = 0; ++y;
			}
			if ( y == lKernelHeight )
			{
				y = 0; ++z;
			}
		}
	}
	PROG_MAX( usDims[2] * dataPtr->getDataDimension() );
  for ( ushort usChannels = 0; usChannels < dataPtr->getDataDimension(); ++usChannels )
  {
		x = usRadius[0]; y = usRadius[1]; z = usRadius[2];
		TImage::iterator outputIt = outputPtr->begin() + usRadius[0] + usRadius[1] * lineSize
			+ usRadius[2] * sliceSize;
		TImage::iterator inputIt = dataPtr->begin()	+ usRadius[0] + usRadius[1] * lineSize
			+ usRadius[2] * sliceSize;
		while( z < static_cast<long>( usDims[2] - usRadius[2] ) )
		{
			vector<ptrdiff_t>::const_iterator it = kernelPositionsVec.begin();
			for( CTypedData<double>::iterator kernelIt = kernel.begin(); 
				kernelIt != kernel.end(); ++kernelIt, ++it )
			{
				(*outputIt) += static_cast<ushort>( 
					static_cast<double>(*(inputIt+*it)) * (*kernelIt) );
			}
			++outputIt; ++inputIt; ++x;
			if ( x == static_cast<long>( usDims[0] - usRadius[0] ) )
			{
				x = usRadius[0]; ++y; outputIt += 2 * usRadius[0]; inputIt += 2 * usRadius[0];
			}
			if ( y == static_cast<long>( usDims[1] - usRadius[1] ) )
			{
				y = usRadius[1]; ++z; 
				outputIt += 2 * usRadius[1] * lineSize; inputIt += 2 * usRadius[1] * lineSize;
PROG_VAL( z * ( usChannels + 1 ) );		
APP_PROC();		
			}	
		}
  }    
PROG_RESET();
BENCHSTOP;
  return outputPtr;
}
