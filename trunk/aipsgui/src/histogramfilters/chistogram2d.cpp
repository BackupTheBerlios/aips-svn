/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Belitz                                  *
 *   h.belitz@fz-juelich.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "chistogram2d.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
/** \param ulID unique module ID */
CHistogram2D::CHistogram2D( ulong ulID ) throw()
  : CFilter ( ulID, "2D Histogram", 3, 1,
	"CHistogram2D", "0.1", "CFilter" )
{
  setModuleID( sLibID );
  
  sDocumentation = "Determines a 2D histogram based on the image intensity and gradient\n"
                   "** Input ports:\n"
                   "0: A scalar multidimensional, multichannel data set (the intensity image)\n"
                   "1: A scalar multidimensional, multichannel data set (the classified image)\n"
                   "2: A scalar multidimensional, multichannel data set (the gradient image)\n"
                   "**Output ports:\n"
                   "0: An 256x256 intensity profile. This is also written to disk";

	parameters.initUnsignedLong( "Class1", 3, 0, 65535 );
	parameters.initUnsignedLong( "Class2", 4, 0, 65535 );
	parameters.initDouble( "HistoSmooth", 0.5, 0.0, 100.0 );
	parameters.initUnsignedLong( "Shift", 5, 1, 65535 );
                   
  inputsVec[0].portType = CPipelineItem::IOInteger;
  inputsVec[1].portType = CPipelineItem::IOInteger;
  inputsVec[2].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOInteger;
                   
}

CHistogram2D::~CHistogram2D() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CHistogram2D::apply () throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr imagePtr = static_pointer_cast<TImage>( getInput() );
  TImagePtr classPtr = static_pointer_cast<TImage>( getInput(1) );
  TImagePtr gradientPtr = static_pointer_cast<TImage>( getInput(2) );
  if ( imagePtr.get() == NULL || classPtr.get() == NULL || gradientPtr.get() == NULL )
  {
    alog << LWARN << "Input type is no image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  double classOneMean = 0.0;
  double classOneVariance = 0.0;
  vector<short> classOneValues;
  double classTwoMean = 0.0;
  double classTwoVariance = 0.0;
  vector<short> classTwoValues;

  uint classOne = parameters.getUnsignedLong( "Class1" );
  uint classTwo = parameters.getUnsignedLong( "Class2" );
	// Calculate intensity mean and variance of the selected classes
	TImage::iterator classIt = classPtr->begin();
	TImage::iterator imageIt = imagePtr->begin();
	while( classIt != classPtr->end() )
	{
		if ( *classIt == static_cast<short>(classOne) )
			classOneValues.push_back( *imageIt );
		else if ( *classIt == static_cast<short>(classTwo) )
			classTwoValues.push_back( *imageIt );
		++classIt;
		++imageIt;
	}
	
  for ( vector<short>::iterator it = classOneValues.begin(); it != classOneValues.end(); ++it )
  	classOneMean += static_cast<double>( *it );
  classOneMean /= static_cast<double>( classOneValues.size() );
  for ( vector<short>::iterator it = classOneValues.begin(); it != classOneValues.end(); ++it )
  	classOneVariance += (static_cast<double>(*it) - classOneMean)*(static_cast<double>(*it) - classOneMean);
  classOneVariance /= static_cast<double>( classOneValues.size() );
  classOneVariance = sqrt( classOneVariance );

  for ( vector<short>::iterator it = classTwoValues.begin(); it != classTwoValues.end(); ++it )
  	classTwoMean += static_cast<double>( *it );
  classTwoMean /= static_cast<double>( classTwoValues.size() );
  for ( vector<short>::iterator it = classTwoValues.begin(); it != classTwoValues.end(); ++it )
  	classTwoVariance += (static_cast<double>(*it) - classTwoMean)*(static_cast<double>(*it) - classTwoMean);
  classTwoVariance /= static_cast<double>( classTwoValues.size() );
  classTwoVariance = sqrt( classTwoVariance );

	cerr << "Values are " << endl;
	cerr << " Class 1 (" << classOne << ") : Mean " << classOneMean << " Variance " << classOneVariance << endl;
	cerr << " Class 2 (" << classTwo << ") : Mean " << classTwoMean << " Variance " << classTwoVariance << endl;

	int rangeMin, rangeMax;
	rangeMin = static_cast<int>( std::min( classOneMean-classOneVariance, classTwoMean-classTwoVariance ) );
	rangeMax = static_cast<int>( std::max( classOneMean+classOneVariance, classTwoMean+classTwoVariance ) );
	cerr << "Extraction range is " << rangeMin << " - " << rangeMax << endl;
	TImagePtr tmp( new TImage( *gradientPtr ) );
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
	ushort usMin = 1;
	ushort usMax = ( tmp->getDataRange().getMaximum() );
	// Do a gaussian smoothing on the histogram
	// Compute gaussian mask
	double dSigma = parameters.getDouble("HistoSmooth");
	int iMaskSize = static_cast<uint>( floor( 6.0 * dSigma + 0.5 ) );
	vector<double> dGaussMask;
	for( int i = -( iMaskSize / 2 ); i <= ( iMaskSize / 2 ); ++i )
	{
		double dExponential = (1.0/sqrt(dSigma*dSigma*2.0*M_PI))*exp( static_cast<double>(-i*i) / ( 2.0 * ( dSigma * dSigma ) ) );
		dGaussMask.push_back( dExponential );
	}

	cerr << "Kernel: " << endl;
	for( int i = 0; i < dGaussMask.size(); ++i )
		cerr << dGaussMask[i] << endl;
	// Shift histogram to attend smoothing
	vector<double> shiftedHisto;
	for( int i = 0; i < iMaskSize/2; ++i )
		shiftedHisto.push_back(0.0);
	for( int i = 1; i < histogramVec[0].size(); ++i )
	{
		shiftedHisto.push_back( histogramVec[0][i] );
	}
	for( int i = 0; i <= iMaskSize/2; ++i )
		shiftedHisto.push_back(0.0);
	vector<double> smoothedHisto( histogramVec[0].size() );		
	for( int i = iMaskSize/2+1; i < shiftedHisto.size() - iMaskSize/2; ++i )
	{
		double smoothed = 0.0;
		for( int j = -iMaskSize/2; j <= iMaskSize/2; ++j )
		{
			smoothed += shiftedHisto[i+j]*dGaussMask[j+(iMaskSize/2)];
		}
		smoothedHisto[i-(iMaskSize/2+1)] = smoothed;
	}
	
	// Smooth
	// Reshift histogram
	// Determine the global maximum g1
	uint g1 = 0;
	double gv1 = 0.0;
	for( int i = 1; i < smoothedHisto.size(); ++i )
	{
		if ( smoothedHisto[i] >= smoothedHisto[i-1] && smoothedHisto[i] >= gv1 )
		{
			gv1 = smoothedHisto[i]; g1 = i;
		}
		else if ( smoothedHisto[i] < smoothedHisto[i-1] )
			break;
	}
	// Determine next global maximum g2 which is at least 10 points from g1
	uint t = g1+parameters.getUnsignedLong("Shift");
	while( smoothedHisto[t] <= smoothedHisto[t-1] )
		++t;
	uint g2 = t+1;
	double gv2 = 0;
	for( int i = t+1; i < smoothedHisto.size(); ++i )
	{
		if ( smoothedHisto[i] > smoothedHisto[i-1] && smoothedHisto[i] > gv2 )
		{
			gv2 = smoothedHisto[i]; g2 = i;
		}
		else if ( smoothedHisto[i] < smoothedHisto[i-1] )
			break;
	}
	
	cerr << "Threshold values are " << g1 << " and " << g2;
/*	ofstream file( "/home/belitz/histogram.data" );
	for( uint i = 0; i <= smoothedHisto.size(); ++i )
	{
		file << i << " " << smoothedHisto[i] << endl;
	}
	file.close();*/
  size_t dims[] = {256,256};
  TFieldPtr outputPtr ( new TField( 2, dims ) );
  outputPtr->setMinimum( 0.0 );
  outputPtr->setMaximum( 1.0 );
  (*outputPtr)=0.0;
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CHistogram2D::newInstance( ulong ulID ) const throw()
{
  return new CHistogram2D( ulID );
}