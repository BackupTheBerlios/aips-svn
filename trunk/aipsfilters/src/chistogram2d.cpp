/************************************************************************
 * File: chistogram2d.h                                                 *
 * Project: AIPS Histogram filter modules                               *
 * Description: Heuristic to compute hysteresis thresholds for a        *
 *               given image                                            *
 *                                                                      *
 * Author: Hendrik Belitz <hbelitz@users.berlios.de>                    *
 *                                                                      *
 * Created: 2005-03-09                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "chistogram2d.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::lambda;

namespace aips {
/*************
 * Structors *
 *************/

/** TODO AUfr�men! */

/** \param ulID unique module ID */
CHistogram2D::CHistogram2D( ulong ulID ) throw()
  : CFilter ( ulID, "2D Histogram", 3, 1,
	"CHistogram2D", CHISTOGRAM2D_VERSION, "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Determines a 2D histogram based on the image intensity and gradient\n"
                   "** Input ports:\n"
                   "0: A scalar multidimensional, multichannel data set (the intensity image)\n"
                   "1: A scalar multidimensional, multichannel data set (the classified image)\n"
                   "2: A scalar multidimensional, multichannel data set (the gradient image)\n"
                   "** Parameters:\n"
 									 "1: Class1 - class number one in input 2 to examine\n"
 									 "2: Class2 - class number two in input 2 to examine\n"
 									 "3: HistoSmooth - Amount of smoothing for the resulting edge histogram (0.25)\n"
 									 "4: Shift - Minimum shift between the thresholds (2)\n"
 									 "5: T1 - First threshold (output)\n"
 									 "6: T2 - Second threshold (output)\n";

	parameters.initUnsignedLong( "Class1", 2, 0, 65535 ); // 3
	parameters.initUnsignedLong( "Class2", 3, 0, 65535 ); // 4
	parameters.initDouble( "HistoSmooth", 0.5, 0.0, 100.0 );
	parameters.initUnsignedLong( "Shift", 2, 1, 65535 );
	parameters.initUnsignedLong( "T1", 1, 0, 65535 );
	parameters.initUnsignedLong( "T2", 2, 0, 65535 );

  inputsVec[0].portType = CPipelineItem::IOInteger;
  inputsVec[1].portType = CPipelineItem::IOInteger;
  inputsVec[2].portType = CPipelineItem::IOFloat;
  outputsVec[0].portType = CPipelineItem::IOInteger;

}

CHistogram2D::~CHistogram2D() throw()
{

}

/*****************
 * Other methods *
 *****************/

DEFINE_CALL_MACRO( CHistogram2D::call, CHistogram2D::compute, imageTL );

void CHistogram2D::apply () throw()
{
BENCHSTART;
FBEGIN;
	bModuleReady = false;
	if ( getInput() )
		call<Length<imageTL>::value - 1>();
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D integer image!") << endl;
    return;
  }
FEND;
BENCHSTOP;
}

template<typename ImageType>
bool CHistogram2D::compute() throw()
{
	typedef boost::shared_ptr<ImageType> ImagePtr;
	typedef typename ImageType::TDataType ValueType;
  ImagePtr imagePtr = static_pointer_cast<ImageType>( getInput() );
  ImagePtr classPtr = static_pointer_cast<ImageType>( getInput(1) );
  ImagePtr gradientPtr = static_pointer_cast<ImageType>( getInput(2) );
  if ( !checkInput<ImageType>( imagePtr, 2, 3 )
  	|| !checkInput<ImageType>( classPtr, 2, 3 )
  	|| !checkInput<ImageType>( gradientPtr, 2, 3 ) )
  {
    return false;
  }
	bModuleReady = true;
  deleteOldOutput();

  double dClassOneMean = 0.0;
  double dClassOneVariance = 0.0;
  vector<ValueType> classOneValuesVec;
  double dClassTwoMean = 0.0;
  double dClassTwoVariance = 0.0;
  vector<ValueType> classTwoValuesVec;

  uint uiClassOne = parameters.getUnsignedLong( "Class1" );
  uint uiClassTwo = parameters.getUnsignedLong( "Class2" );
  cerr << uiClassOne << " " << uiClassTwo << endl;
	// Calculate intensity mean and variance of the selected classes
	typename ImageType::iterator classIt = classPtr->begin();
	typename ImageType::iterator imageIt = imagePtr->begin();
	while( classIt != classPtr->end() )
	{
		if ( *classIt == static_cast<ValueType>( uiClassOne ) )
			classOneValuesVec.push_back( *imageIt );
		else if ( *classIt == static_cast<ValueType>( uiClassTwo ) )
			classTwoValuesVec.push_back( *imageIt );
		++classIt;
		++imageIt;
	}

  for_each( classOneValuesVec.begin(), classOneValuesVec.end(), dClassOneMean += _1 );
  dClassOneMean /= static_cast<double>( classOneValuesVec.size() );
  for_each( classOneValuesVec.begin(), classOneValuesVec.end(),
  	dClassOneVariance += ( _1 - dClassOneMean ) * ( _1 - dClassOneMean ) );
  dClassOneVariance /= static_cast<double>( classOneValuesVec.size() );
  dClassOneVariance = sqrt( dClassOneVariance );

	for_each( classTwoValuesVec.begin(), classTwoValuesVec.end(), dClassTwoMean += _1 );
  dClassTwoMean /= static_cast<double>( classTwoValuesVec.size() );
  for_each( classTwoValuesVec.begin(), classTwoValuesVec.end(),
  	dClassTwoVariance += ( _1 - dClassTwoMean ) * ( _1 - dClassTwoMean ) );
  dClassTwoVariance /= static_cast<double>( classTwoValuesVec.size() );
  dClassTwoVariance = sqrt( dClassTwoVariance );

cerr << " C1 (" << uiClassOne << ") : Mean " << dClassOneMean << " SD " << dClassOneVariance << endl
<< " C2 (" << uiClassTwo << ") : Mean " << dClassTwoMean << " SD " << dClassTwoVariance << endl;

	int rangeMin, rangeMax;
	rangeMin = static_cast<int>(
		std::min( dClassOneMean - dClassOneVariance, dClassTwoMean - dClassTwoVariance ) );
	rangeMax = static_cast<int>(
		std::max( dClassOneMean - dClassOneVariance, dClassTwoMean - dClassTwoVariance ) );

	ImagePtr tmp( new ImageType( *gradientPtr ) );
	classIt = imagePtr->begin();
	imageIt = tmp->begin();
	while( classIt != imagePtr->end() )
	{
		if ( *classIt < rangeMin || *classIt > rangeMax )
			*imageIt = 0;
		++classIt;
		++imageIt;
	}
	calculateHistogram( tmp );
	// Do a gaussian smoothing on the histogram
	// Compute gaussian mask
	double dSigma = parameters.getDouble( "HistoSmooth" );
cerr << "HistoSmooth " << dSigma << endl;
	int iMaskSize = static_cast<uint>( floor( 6.0 * dSigma + 0.5 ) );
	vector<double> dGaussMask;
	for( int i = -( iMaskSize / 2 ); i <= ( iMaskSize / 2 ); ++i )
	{
		double dExponential = ( 1.0 / sqrt( dSigma * dSigma * 2.0 * M_PI ) )
			* exp( static_cast<double>( -i * i ) / ( 2.0 * ( dSigma * dSigma ) ) );
		dGaussMask.push_back( dExponential );
	}

cerr << "Mask size " << iMaskSize << endl;

	// Shift histogram to attend smoothing
	vector<double> shiftedHisto;
	for( int i = 0; i < iMaskSize/2; ++i )
		shiftedHisto.push_back(0.0);
	for( uint i = 0; i < theHistogramVec[0].size(); ++i )
	{
		shiftedHisto.push_back( theHistogramVec[0][i] );
	}
	for( int i = 0; i <= iMaskSize/2; ++i )
		shiftedHisto.push_back(0.0);
   const uint uiShift = iMaskSize/2;
	vector<double> smoothedHisto( shiftedHisto.size() + uiShift);

	for( uint i = uiShift+1; i < shiftedHisto.size() - uiShift; ++i )
	{
		double smoothed = 0.0;
		for( int j = -(uiShift+1); j <= uiShift; ++j )
		{
			smoothed += shiftedHisto[i+j]*dGaussMask[j+uiShift+1];
		}
		smoothedHisto[i] = smoothed;
	}
   smoothedHisto = shiftedHisto;
	// Smooth
	// Reshift histogram
	// Determine the global maximum g1
	uint g1 = 0;
	double gv1 = 0.0;
	for( uint i = uiShift+1; i < smoothedHisto.size(); ++i )
	{
		if ( smoothedHisto[i] >= smoothedHisto[i-1] && smoothedHisto[i] >= gv1 )
		{
			gv1 = smoothedHisto[i]; g1 = i;
		}
		else if ( smoothedHisto[i] < smoothedHisto[i-1] )
			break;
	}
	// Determine next global maximum g2
/*	if ( g1 == 0 ) g1 = parameters.getUnsignedLong("Shift");*/
	uint t = g1 + parameters.getUnsignedLong("Shift");
	while( smoothedHisto[t] <= smoothedHisto[t-1] )
		++t;
	uint g2 = t+1;
	double gv2 = 0;
	for( uint i = t+1; i < smoothedHisto.size(); ++i )
	{
		if ( smoothedHisto[i] > smoothedHisto[i-1] && smoothedHisto[i] > gv2 )
		{
			gv2 = smoothedHisto[i]; g2 = i;
		}
		else if ( smoothedHisto[i] < smoothedHisto[i-1] )
			break;
	}
	if ( g1 == 0 )
	{
		g1 = g2;
		t = g1+parameters.getUnsignedLong("Shift");
		while( smoothedHisto[t] <= smoothedHisto[t-1] )
			++t;
		g2 = t+1;
		gv2 = 0;
		for( uint i = t+1; i < smoothedHisto.size(); ++i )
		{
			if ( smoothedHisto[i] > smoothedHisto[i-1] && smoothedHisto[i] > gv2 )
			{
				gv2 = smoothedHisto[i]; g2 = i;
			}
			else if ( smoothedHisto[i] < smoothedHisto[i-1] )
				break;
		}
	}
	if ( g2-g1 > (2 * parameters.getUnsignedLong("Shift")) )
		g2 = g1 + 2 * parameters.getUnsignedLong("Shift");
   g1--;
   g2--;
 	cerr << "Threshold values are " << g1 << " and " << g2;
	parameters.setUnsignedLong("T1", static_cast<ulong>( g1 * 0.33)-1 );
	parameters.setUnsignedLong("T2",
		static_cast<ulong>( std::max( g1 * 0.66, g1 + parameters.getDouble("Shift") + 1) ) - 1 );
ofstream file( "/home/hendrik/orig_histogram.data" );
	for( uint i = 0; i <= theHistogramVec[0].size(); ++i )
	{
		file << i << " " << theHistogramVec[0][i] << endl;
	}
	file.close();
ofstream file2( "/home/hendrik/shifted_histogram.data" );
	for( uint i = 0; i <= shiftedHisto.size(); ++i )
	{
		file2 << i << " " << shiftedHisto[i] << endl;
	}
	file2.close();
ofstream file3( "/home/hendrik/smoothed_histogram.data" );
	for( uint i = 0; i <= smoothedHisto.size(); ++i )
	{
		file3 << i << " " << smoothedHisto[i] << endl;
	}
	file3.close();
	return true;
}

}
