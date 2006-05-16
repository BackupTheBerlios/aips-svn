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

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <fstream>

#include <boost/shared_ptr.hpp>

#include <cdatafileserver.h>
#include <canalyzehandler.h>
#include <csimpledathandler.h>
#include <cdatahandler.h>
/*#include <cadfhandler.h>*/
#include <cdf3handler.h>
#include <ccommonimagehandler.h>
#include <ctrasterhandler.h>
#include <cvtkhandler.h>
#include <cvinterfilehandler.h>
#include <vector>
#include <string>
#include <ctypedmap.h>
/*#include "cmeandistance.h"
#include "chausdorffdistance.h"
#include "cdicecoefficient.h"
#include "cregionsize.h"*/
#include "cdiscrepancymeasures.h"

using namespace std;
using namespace aips;
using namespace boost;

// void rg( std::queue<TPoint2D> work, TImagePtr inputPtr, TImagePtr outputPtr )
// {
// 	ulong ulRegionThreshold = 0;
// 	while( !work.empty() )
// 	{
// 	 	TPoint2D p = work.front(); work.pop();
// 		if ( ( p[0] > 0 )
// 		  && ( (*outputPtr)( p[0]-1, p[1] ) == 0 )
// 			&& ( static_cast<ulong>(abs((*inputPtr)( p[0]-1, p[1])-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
// 		{
// 			work.push( TPoint2D( p[0]-1, p[1] ) ); (*outputPtr)( p[0]-1, p[1] ) = 1;
// 		}
// 		if ( ( p[0] < static_cast<long>(inputPtr->getExtent(0)-1) )
// 		  && ( (*outputPtr)( p[0]+1, p[1] ) == 0 )
// 			&& ( static_cast<ulong>(abs((*inputPtr)( p[0]+1, p[1])-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
// 		{
// 			work.push( TPoint2D( p[0]+1, p[1] ) ); (*outputPtr)( p[0]+1, p[1] ) = 1;
// 		}
// 		if ( ( p[1] > 0 )
// 	  	&& ( (*outputPtr)( p[0], p[1]-1 ) == 0 )
// 			&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]-1)-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
// 		{
// 			work.push( TPoint2D( p[0], p[1]-1 ) ); (*outputPtr)( p[0], p[1]-1 ) = 1;
// 		}
// 		if ( ( p[1] < static_cast<long>(inputPtr->getExtent(1)-1) )
// 		  && ( (*outputPtr)( p[0], p[1]+1 ) == 0 )
// 			&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]+1)-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
// 		{
// 			work.push( TPoint2D( p[0], p[1]+1 ) ); (*outputPtr)( p[0], p[1]+1 ) = 1;
// 		}
// 	}
// }

int main(int argc, char *argv[])
{
	if ( argc < 3 || argc > 7 )
	{
		cout << "evaltool - slice-by-slice evaluation of two volume images" << endl;
		cout << "command syntax:" << endl;
		cout << "aipsconvert inputfile1.ext inputfile2.ext outputfile.txt" << endl;
		return EXIT_SUCCESS;
	}
  
  shared_ptr<CAnalyzeHandler> h1 ( new CAnalyzeHandler );	
	shared_ptr<CSimpleDatHandler> h2 ( new CSimpleDatHandler );	
	shared_ptr<CDataHandler> h3 ( new CDataHandler );
	
	getFileServer().addHandler( h1 );
	getFileServer().addHandler( h2 );
	getFileServer().addHandler( h3 );
  
//	uint force = 0;
	bool ipset = false;
	bool ipset2 = false;
	string input, input2, output;
	for( int i = 1; i < argc; ++i )
	{
		if (!ipset)
		{
			input = argv[i]; ipset = true;
		}
		else if (!ipset2)
		{
			input2 = argv[i]; ipset2 = true;
		}
		else
			output = argv[i];
	}
	
	TDataFile file1 = getFileServer().loadDataSet( input );
	
	TImagePtr image1 = static_pointer_cast<TImage>(file1.first);
	cerr << "Input image 1 loaded: " << image1->getExtent(0) << " x " << image1->getExtent(1) << " x " 
		<< image1->getExtent(2) << endl;
	
	TDataFile file2 = getFileServer().loadDataSet( input2 );
	TImagePtr image2 = static_pointer_cast<TImage>(file2.first);
	cerr << "Input image 2 loaded: " << image2->getExtent(0) << " x " << image2->getExtent(1) << " x " 
		<< image2->getExtent(2) << endl;

	for( TImage::iterator it = image1->begin(); it != image1->end(); ++it )
	{
		if ((*it)!=1) (*it)=0;
	}
	for( TImage::iterator it = image2->begin(); it != image2->end(); ++it )
	{
		if ((*it)!=1) (*it)=0;
	}
	CDiscrepancyMeasures eval(0);
	eval.setInput( image1 );
	eval.setInput( image2, 1 );
	eval.apply();
	CTypedMap* parameters = eval.getParameters();
	cerr << "DiceCoefficient     " << parameters->getDouble( "DiceCoefficient" ) << endl;
	cerr << "TanimotoCoefficient " << parameters->getDouble( "TanimotoCoefficient" ) << endl;
	cerr << "HausdorffDistance   " << parameters->getDouble( "HausdorffDistance" ) << endl;
	cerr << "MeanDistance        " << parameters->getDouble( "MeanDistance" ) << endl;
	cerr << "InputRegionSize     " << parameters->getUnsignedLong( "InputRegionSize" ) << endl;
	cerr << "ReferenceRegionSize " << parameters->getUnsignedLong( "ReferenceRegionSize" ) << endl;
	cerr << "SharedRegionSize    " << parameters->getUnsignedLong( "SharedRegionSize" ) << endl;
	cerr << "InputSurface        " << parameters->getUnsignedLong( "InputSurface" ) << endl;
	cerr << "ReferenceSurface    " << parameters->getUnsignedLong( "ReferenceSurface" ) << endl;
	cerr << "CombinedArea        " << parameters->getUnsignedLong( "CombinedArea"  ) << endl;
	cerr << "FalsePositives      " << parameters->getUnsignedLong( "FalsePositives" ) << endl;
	cerr << "FalseNegatives      " << parameters->getUnsignedLong( "FalseNegatives" ) << endl;
	//getFileServer().saveDataSet( output, file );
  return EXIT_SUCCESS;
}
