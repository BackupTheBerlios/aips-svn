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

using namespace std;
using namespace aips;
using namespace boost;

int main(int argc, char *argv[])
{
	if ( argc < 3 || argc > 4 )
	{
		cout << "aipsconvert - conversion between different image formats" << endl;
		cout << "command syntax:" << endl;
		cout << "aipsconvert [-s -c%] inputfile.ext outputfile.ext" << endl;
		cout << " -s swap data endianess" << endl;		
		cout << " -c% combine % slices into a single volume file" << endl;
		return EXIT_SUCCESS;
	}
  
//	uint force = 0;
	bool bSwapEndianess = false;
	bool bCombine = true;
	bool ipset = false;
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
			if ( argv[i][1] == 'c' )
			{
				bSwapEndianess = false;
				bCombine = true;
				int count = 0;
				int number[4];
				int slices = 0;
				char tmps[10];
				strcpy( tmps, &(argv[i][2]) );
				slices = atoi( tmps );
				cerr << "Slices " << slices << endl;
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
		file.second->setString("ForceDataType", "Int16");
		getFileServer().saveDataSet( output, file );
	}
	catch ( std::exception& e )
	{
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

/*	shared_ptr<CAnalyzeHandler> h1 ( new CAnalyzeHandler );	
	shared_ptr<CImageHandler> h6 ( new CImageHandler);
	getFileServer().addHandler( h1 );
	getFileServer().addHandler( h6 );
	
	size_t dims[] = { 256, 256, 50 };
	TImagePtr output( new TImage(3, dims) );
	(*output) = 0;
	uint z = 12;
	while( z < 26 )
	{
		string filename = "mr289_two_sl" + lexical_cast<string>( z ) + ".bmp";
		TImagePtr slice;
		try
		{
			slice = dynamic_pointer_cast<TImage>( getFileServer().loadDataSet( filename ).first );
		}
		catch( std::exception &e )
		{
			cerr << e.what() << endl;
		}
		cerr << "Slice " << z << endl;
		for( uint y = 0; y < dims[1]; ++y )
			for( uint x = 0; x < dims[0]; ++x )
				if ( (*slice)(x,dims[1]-1-y) > 0)
					(*output)(x,y,z) = 0;
				else
					(*output)(x,y,z) = 1;
		++z;
	}
	TDataFile f; f.first = output;
	getFileServer().saveDataSet( "mr289_brain.hdr", f );*/
  return EXIT_SUCCESS;
}
