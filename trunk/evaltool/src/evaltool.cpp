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
#include <boost/signal.hpp>

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
#include "cmeandistance.h"
#include "chausdorffdistance.h"
#include "cdicecoefficient.h"
#include "cregionsize.h"

using namespace std;
using namespace aips;
using namespace boost;

void rg( std::queue<TPoint2D> work, TImagePtr inputPtr, TImagePtr outputPtr )
{
	ulong ulRegionThreshold = 0;
	while( !work.empty() )
	{
	 	TPoint2D p = work.front(); work.pop();
		if ( ( p[0] > 0 )
		  && ( (*outputPtr)( p[0]-1, p[1] ) == 0 )
			&& ( static_cast<ulong>(abs((*inputPtr)( p[0]-1, p[1])-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
		{
			work.push( TPoint2D( p[0]-1, p[1] ) ); (*outputPtr)( p[0]-1, p[1] ) = 1;
		}
		if ( ( p[0] < static_cast<long>(inputPtr->getExtent(0)-1) )
		  && ( (*outputPtr)( p[0]+1, p[1] ) == 0 )
			&& ( static_cast<ulong>(abs((*inputPtr)( p[0]+1, p[1])-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
		{
			work.push( TPoint2D( p[0]+1, p[1] ) ); (*outputPtr)( p[0]+1, p[1] ) = 1;
		}
		if ( ( p[1] > 0 )
	  	&& ( (*outputPtr)( p[0], p[1]-1 ) == 0 )
			&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]-1)-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
		{
			work.push( TPoint2D( p[0], p[1]-1 ) ); (*outputPtr)( p[0], p[1]-1 ) = 1;
		}
		if ( ( p[1] < static_cast<long>(inputPtr->getExtent(1)-1) )
		  && ( (*outputPtr)( p[0], p[1]+1 ) == 0 )
			&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]+1)-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
		{
			work.push( TPoint2D( p[0], p[1]+1 ) ); (*outputPtr)( p[0], p[1]+1 ) = 1;
		}
	}
}

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
	shared_ptr<CDF3Handler> h4 ( new CDF3Handler );
//	CADFHandler* h5 = new CADFHandler;
	shared_ptr<CCommonImageHandler> h6 ( new CCommonImageHandler);
	shared_ptr<CTRasterHandler> h7 ( new CTRasterHandler);
	shared_ptr<CVtkHandler> h8 ( new CVtkHandler);
	shared_ptr<CVInterfileHandler> h9 ( new CVInterfileHandler);
	
	getFileServer().addHandler( h1 );
	getFileServer().addHandler( h2 );
	getFileServer().addHandler( h3 );
	getFileServer().addHandler( h4 );
// 	getFileServer().addHandler( h5 );
	getFileServer().addHandler( h6 );
	getFileServer().addHandler( h7 );
 	getFileServer().addHandler( h8 );
	getFileServer().addHandler( h9 );
  
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
	
	if ( image1->getExtent(0) != image2->getExtent(0) || image1->getExtent(1) != image2->getExtent(1)
		|| image1->getExtent(2) != image2->getExtent(2) )
	{
		cerr << "ERROR: Image dimensions do not match" << endl;
		exit(-1);
	}
	ofstream op( output.c_str() );
	op << "Slice;MeanDistance;HausdorffDistance;DiceCoefficient;Region1Size;Region2Size" << endl;
	CMeanDistance m(0);
	CHausdorffDistance h(0);
	CRegionSize r(0);
	CDiceCoefficient d(0);
	TImagePtr slice1 ( new TImage( 2, image1->getExtents() ) );
	TImagePtr slice2 ( new TImage( 2, image1->getExtents() ) );
	std::queue<TPoint2D> work;		
	for( uint z = 0; z < image1->getExtent(2); ++z )
	{
		TPoint2D p;
		// Get local slice and define contour center
		for( uint y = 0; y < image1->getExtent(1); ++y )
			for( uint x = 0; x < image1->getExtent(0); ++x )
			{
				(*slice1)(x,y) = (*image1)(x,y,z);				
				if( (*slice1)(x,y) ) 
				{
					p[0]=x; p[1]=y;	work.push(p);
				}
				(*slice2)(x,y) = (*image2)(x,y,z);
				if( (*slice2)(x,y) ) 
				{
					p[0]=x; p[1]=y;	work.push(p);
				}
			}
		// Calculate contour center;
		double xx = 0.0; 
		double yy = 0.0;
		double den = work.size();
		while( !work.empty() )
		{
			p = work.front();
			work.pop();
			xx += p[0];
			yy += p[1];
		}
		xx /= den;
		yy /= den;
		cerr << "Contour center is " << xx << ";" << yy << endl;
		// Evaluate actual slice
		m.setInput( slice1, 0 );
		m.setInput( slice2, 1 );
		m.apply();
		double mean = m.getParameters()->getDouble("Result");		
		h.setInput( slice1, 0 );
		h.setInput( slice2, 1 );
		double hd = h.getParameters()->getDouble("Result");
		h.apply();
		r.setInput( slice1, 0 );
		r.apply();
		double reg1 = r.getParameters()->getDouble("Result");
		r.setInput( slice2, 0 );
		r.apply();
		double reg2 = r.getParameters()->getDouble("Result");
		TImagePtr outputPtr ( new TImage( *slice1 ) );
		*outputPtr = 0;
		p[0] = static_cast<short>( round(xx) );
		p[1] = static_cast<short>( round(yy) );
		(*outputPtr)(p[0],p[1])=1;
		work.push( p );
		rg( work, slice1, outputPtr );
	 	TImage::iterator vit = slice1->begin();
 		TImage::iterator vot = outputPtr->begin();
 		while( vit != slice1->end() )
	 	{
 			if ( (*vot) > 0 ) (*vit) = 1;
 			++vit; ++vot;
	 	}
		*outputPtr = 0;
		// Define contour center 		
		p[0] = static_cast<short>( round(xx) );
		p[1] = static_cast<short>( round(yy) );
		(*outputPtr)(p[0],p[1])=1;
		work.push( p );
		rg( work, slice2, outputPtr );
	 	vit = slice2->begin();
 		vot = outputPtr->begin();
 		while( vit != slice2->end() )
	 	{
 			if ( (*vot) > 0 ) (*vit) = 1;
 			++vit; ++vot;
	 	}	
		d.setInput( slice1, 0 );
		d.setInput( slice2, 1 );
		d.apply();
		double dice = d.getParameters()->getDouble("Result");
		// Output results
		op << z << ";" << mean << ";" << hd << ";" << dice << ";" << reg1 << ";" << reg2 << endl;
		cerr << z << ";" << mean << ";" << hd << ";" << dice << ";" << reg1 << ";" << reg2 << endl;
	}
	op.close();
	//getFileServer().saveDataSet( output, file );
  return EXIT_SUCCESS;
}
