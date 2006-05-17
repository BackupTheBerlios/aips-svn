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
#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

#include <cdatafileserver.h>
#include <canalyzehandler.h>
#include <csimpledathandler.h>
#include <cdatahandler.h>
#include <ctypedmap.h>
#include "cdiscrepancymeasures.h"

using namespace std;
using namespace aips;
using namespace boost;
using namespace boost::program_options;

bool bAppendToOutput;
uint uiLabel;
string sOutputFile;
vector<string> theInputFileNamesVec;

bool parseCommandLine( int argc, char *argv[] )
{
  boost::program_options::options_description theOptionsDescription( "Valid options" );
  theOptionsDescription.add_options()
    ( "help,h", "produces this help message" )
    ( "inputfile,i", 
      boost::program_options::value<vector<string> >(), "files to do computation on. These need to be exactly two files" )
    ( "outputfile,o",
      boost::program_options::value<string>(), "text file to write results to" )
    ( "append,a", "append results to output file" )
    ( "label,l", boost::program_options::value<uint>( &uiLabel )->default_value(1), "label to evaluate (defaults to 1)" )
  ;

  boost::program_options::positional_options_description thePositionalOptions;
  thePositionalOptions.add("inputfile", -1);

  boost::program_options::variables_map vm;
  boost::program_options::store( command_line_parser( argc, argv ).options( theOptionsDescription ).positional(
    thePositionalOptions ).run(), vm );
  boost::program_options::notify( vm );

  bAppendToOutput = false;

  if ( vm.count( "help" ) )
  {
    cout << "evaltool - slice-by-slice evaluation of two volume images" << endl;
    cout << theOptionsDescription << endl;
    return false;
  }

  if ( vm.count( "append" ) )
    bAppendToOutput = true;

  if ( vm.count( "outputfile" ) )
    sOutputFile = vm["outputfile"].as<string>();
  else
    cout << "No outputfile given, output will be written to stdout" << endl;

  if ( vm.count( "inputfile" ) )
  {
    theInputFileNamesVec = vm["inputfile"].as<vector<string> >();
  }

  if ( theInputFileNamesVec.size() != 2 )
  {
    cout << "Need exactly two input files to run, aborting..." << endl;
    return false;
  }

  return true;
}

int main(int argc, char *argv[])
{
  if ( !parseCommandLine( argc, argv ) )
    return EXIT_FAILURE;

  cerr << "Inputs: <" << theInputFileNamesVec[0] << "><" << theInputFileNamesVec[1] << ">" << endl;
  cerr << "Output: <" << sOutputFile << ">" << endl;
  cerr << "Append: " << bAppendToOutput << endl;
  cerr << "Label: " << uiLabel << endl;
  
/*	if ( argc < 3 || argc > 7 )
	{
		cout << "evaltool - slice-by-slice evaluation of two volume images" << endl;
		cout << "command syntax:" << endl;
		cout << "aipsconvert inputfile1.ext inputfile2.ext outputfile.txt" << endl;
		return EXIT_SUCCESS;
	}*/

  shared_ptr<CAnalyzeHandler> h1 ( new CAnalyzeHandler );
	shared_ptr<CSimpleDatHandler> h2 ( new CSimpleDatHandler );	
	shared_ptr<CDataHandler> h3 ( new CDataHandler );
	
	getFileServer().addHandler( h1 );
	getFileServer().addHandler( h2 );
	getFileServer().addHandler( h3 );
		
	CDiscrepancyMeasures eval(0);
  CTypedMap* parameters = eval.getParameters();
  parameters->setUnsignedLong( "Label", uiLabel );
	eval.setInput( getFileServer().loadDataSet( theInputFileNamesVec[0] ).first );
	eval.setInput( getFileServer().loadDataSet( theInputFileNamesVec[1] ).first, 1 );
	eval.apply();
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
