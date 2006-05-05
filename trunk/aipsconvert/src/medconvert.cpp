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

#include <boost/shared_ptr.hpp>

#include <cdatafileserver.h>
#include <citkhandler.h>
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

using namespace std;
using namespace aips;
using namespace boost;

int main(int argc, char *argv[])
{
	if ( argc < 3 || argc > 7 )
	{
		cout << "aipsconvert - conversion between different image formats" << endl;
		cout << "command syntax:" << endl;
		cout << "aipsconvert [-s -c%] inputfile.ext outputfile.ext" << endl;
		cout << " -s swap data endianess" << endl;		
		cout << " -c%1 %2 %3 combine slices %1 to %2 into a single volume file containing %3 slices" << endl;
		cout << " -f%1 %2 fill mask starting at position %1;%2 in a single volume file" << endl;
		cout << " -g output should be an slice-by-slice contour" << endl;
		cout << " -a%1 add a blank border of given size" << endl;
		return EXIT_SUCCESS;
	}
  
//	uint force = 0;
	bool bSwapEndianess = false;
	bool bCombine = false;
	bool bBorder = false;
	bool ipset = false;
	bool bFill = false;
	bool bContour = false;
	uint slices = 0;
	uint sliceend = 0;
	uint sliceMax = 0;
	uint fillx = 0;
	uint filly = 0;
	uint bordersize = 0;
	string input, output;
	for( int i = 1; i < argc; ++i )
	{
		if ( argv[i][0] == '-' )
		{
			if ( argv[i][1] == 's' )
			{
				bSwapEndianess = true;
			}			
			else
			if ( argv[i][1] == 'a' )
			{
				bBorder=true;
				char tmps[10];
				strcpy( tmps, &(argv[i][2]) );
				bordersize = atoi( tmps );
			}
			else
			if ( argv[i][1] == 'c' )
			{
				bSwapEndianess = false;
				bCombine = true;
				char tmps[10];
				strcpy( tmps, &(argv[i][2]) );
				slices = atoi( tmps );
				cerr << "Slices " << slices << endl;
				sliceend = atoi( argv[i+1] );
				i+=1;
				cerr << "to " << sliceend << endl;
				sliceMax = atoi( argv[i+1] );
				cerr << "Slices in result" << sliceMax << endl;
				i+=1;
			}			
			else if ( argv[i][1] == 'f' )
			{
				bFill = true;
				char tmps[10];
				strcpy( tmps, &(argv[i][2]) );
				fillx = atoi( tmps );
				cerr << "Fill " << slices << endl;
				filly = atoi( argv[i+1] );
				i+=1;
				cerr << "and " << sliceend << endl;
			}	
			else if ( argv[i][1] == 'g' )
			{
				bContour = true;
			}		
			else 
			{
				cout << "Invalid option: " << argv[i] << endl;
				return EXIT_SUCCESS;
			}
		}
		else if (!ipset)
		{
			input = argv[i]; ipset = true;
		}
		else
			output = argv[i];
	}
	
	string extension = input.substr( input.find_last_of('.')+1 );
	string filename = input.substr( 0, input.find_last_of('.') );
	cerr << "Ext: " << extension << " file: " << filename << endl;
	
	shared_ptr<CITKHandler> h1 ( new CITKHandler );	
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

	if (!bCombine )
	{
	bool bBigEndian = false;
	try
	{	
		TDataFile file = getFileServer().loadDataSet( input );
		if ( file.second->isDefined("FileEndianess") )
		{
			bBigEndian = file.second->getBool("FileEndianess");
			if ( bSwapEndianess && bBigEndian )
				file.second->setBool( "FileEndianess", false );
			else if ( bSwapEndianess && !bBigEndian )
				file.second->setBool( "FileEndianess", true );
			else if ( !bSwapEndianess && bBigEndian )
				file.second->setBool( "FileEndianess", true );
			else
				file.second->setBool( "FileEndianess", false );
		}
		else
		{
			cerr << "File handler didnt define 'Endianess'" << endl;
			vector<string> vec = file.second->getKeyList();
			for( vector<string>::iterator it = vec.begin(); it != vec.end(); ++it )
				cerr << "<" << *it << "> = <" << file.second->getString( *it ) << ">" << endl;
		}
		if ( bSwapEndianess )
			file.second->setBool( "FileEndianess", !bBigEndian );
		//file.second->setString("ForceDataType", "Int16");
		if ( bContour )
		{
			TImagePtr img = static_pointer_cast<TImage>(file.first);
			TImagePtr out ( new TImage(*img) );
			*out = 0;
			for( uint z = 0; z < img->getExtent(2); ++z )
				for( uint y = 0; y < img->getExtent(1); ++y )
					for( uint x = 0; x < img->getExtent(0); ++x )
					{
						if ( (*img)(x,y,z) == 0 ) 
							continue;
						uint cnt = 0;
						if ( (*img)(x-1,y,z) == 1 ) cnt++;
						if ( (*img)(x+1,y,z) == 1 ) cnt++;
						if ( (*img)(x,y-1,z) == 1 ) cnt++;
						if ( (*img)(x,y+1,z) == 1 ) cnt++;
						if ( cnt < 4 ) (*out)(img->getExtent(0)-1-x,img->getExtent(1)-1-y,z)=1;
					}
			file.first = out;
		}
		else if ( bBorder )
		{
			vector<size_t> extents(3);
			extents[0] = file.first->getExtent(0)+2*bordersize;
			extents[1] = file.first->getExtent(1)+2*bordersize;
			extents[2] = file.first->getExtent(2)+2*bordersize;
			TImagePtr img = static_pointer_cast<TImage>(file.first);
			TImagePtr out ( new TImage( 3, extents ) );
			for( uint z = 0; z < img->getExtent(2); ++z )
				for( uint y = 0; y < img->getExtent(1); ++y )
					for( uint x = 0; x < img->getExtent(0); ++x )
					{
						(*out)(x+bordersize,y+bordersize,z+bordersize)=(*img)(x,y,z);
					}
			file.first = out;
		}
		getFileServer().saveDataSet( output, file );
	}
	catch ( std::exception& e )
	{
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}
	}
	else // Combine incoming images
	{
		// We need a volume that can take all images
		
		uint uiNoOfSlices = sliceend - slices + 1;
		cerr << sliceend << " " << slices << " " << uiNoOfSlices << endl;
		// Load the first slice
		string sActualFilename = filename + lexical_cast<string>(slices) + "." + extension;
		TImagePtr slice = dynamic_pointer_cast<TImage>( getFileServer().loadDataSet( sActualFilename ).first );
		vector<size_t> dims = slice->getExtents();
		dims[2] = sliceMax;
		cerr << "Volume dimensions " << dims[0] << " " << dims[1] << " " << dims[2] << endl;
		TImagePtr volume ( new TImage( 3, dims ) );
		uint z = slices;
		while( z <= sliceend )
		{
			cerr << "Reading slice " << z << " from " << sActualFilename << endl;
			cerr << "Image is " << slice->getExtent(0) << " x " << slice->getExtent(1) << endl;
			// We need to flip each slice before copying
			for( uint y = 0; y < dims[1]; ++y )
				for( uint x = 0; x < dims[0]; ++x )
					(*volume)( x, y, z-1/*-slices*/ ) = (*slice)(x,y);//dims[1]-(y+1));
			++z;
			if ( z <= sliceend )
			{
				sActualFilename = filename + lexical_cast<string>(z) + "." + extension;
				slice.reset();
				slice = dynamic_pointer_cast<TImage>( getFileServer().loadDataSet( sActualFilename ).first );
			}
		}
/*		if ( bFill )
		{
			TImagePtr outputPtr ( new TImage( *volume ) );
			*outputPtr = 0;
			(*outputPtr)(fillx,filly,1)=1;
			TImagePtr inputPtr = volume;
			for( uint z = 0; z < volume->getExtent(2); ++z )
			{
				cerr << z << endl;
				std::queue<TPoint2D> work;
				TPoint2D p( fillx, filly );
				work.push( p );
				ulong ulRegionThreshold = 0;
				while( !work.empty() )
			  {
			  	TPoint2D p = work.front(); work.pop();
					if ( ( p[0] > 0 )
					  && ( (*outputPtr)( p[0]-1, p[1], z ) == 0 )
						&& ( static_cast<ulong>(abs(static_cast<int>((*inputPtr)( p[0]-1, p[1], z)-(*inputPtr)( p[0], p[1], z))))<=ulRegionThreshold ) )
						{
							work.push( TPoint2D( p[0]-1, p[1] ) ); (*outputPtr)( p[0]-1, p[1], z ) = 1;
						}
					if ( ( p[0] < static_cast<long>(inputPtr->getExtent(0)-1) )
					  && ( (*outputPtr)( p[0]+1, p[1], z ) == 0 )
						&& ( static_cast<ulong>(abs(static_cast<int>((*inputPtr)( p[0]+1, p[1], z)-(*inputPtr)( p[0], p[1], z))))<=ulRegionThreshold ) )
						{
							work.push( TPoint2D( p[0]+1, p[1] ) ); (*outputPtr)( p[0]+1, p[1], z ) = 1;
						}
					if ( ( p[1] > 0 )
				  	&& ( (*outputPtr)( p[0], p[1]-1, z ) == 0 )
						&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]-1, z)-(*inputPtr)( p[0], p[1], z)))<=ulRegionThreshold ) )
						{
							work.push( TPoint2D( p[0], p[1]-1 ) ); (*outputPtr)( p[0], p[1]-1, z ) = 1;
						}
					if ( ( p[1] < static_cast<long>(inputPtr->getExtent(1)-1) )
					  && ( (*outputPtr)( p[0], p[1]+1, z ) == 0 )
						&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]+1, z)-(*inputPtr)( p[0], p[1], z)))<=ulRegionThreshold ) )
						{
							work.push( TPoint2D( p[0], p[1]+1 ) ); (*outputPtr)( p[0], p[1]+1, z ) = 1;
						}
				}
			}
	 		TImage::iterator vit = volume->begin();
 			TImage::iterator vot = outputPtr->begin();
 			while( vit != volume->end() )
	 		{
 				if ( (*vot) > 0 ) (*vit) = 1;
 				++vit; ++vot;
	 		}
 		}*/
		TDataFile f; f.first = volume;
		cerr << "Saving " << output << endl;
		getFileServer().saveDataSet( output, f );
	}
	
  return EXIT_SUCCESS;
}
