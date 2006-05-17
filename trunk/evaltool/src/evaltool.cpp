/***************************************************************************
 *   Copyright (C) 2006 by Hendrik Belitz                                  *
 *   hbelitz@users.berlios.de                                              *
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
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>

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
    cout << "evaltool - computes evaluation metrics for two given voxel images" << endl;
    cout << theOptionsDescription << endl;
    return false;
  }

  if ( vm.count( "append" ) )
    bAppendToOutput = true;

  if ( vm.count( "outputfile" ) )
    sOutputFile = vm["outputfile"].as<string>();
  else
  {
    cout << "No outputfile given, output will be written to stdout" << endl;
    sOutputFile = "";    
  }

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
  
  if ( sOutputFile == "" )
  {
  	cout << "DiceCoefficient     " << parameters->getDouble( "DiceCoefficient" ) << endl;
	  cout << "TanimotoCoefficient " << parameters->getDouble( "TanimotoCoefficient" ) << endl;
  	cout << "HausdorffDistance   " << parameters->getDouble( "HausdorffDistance" ) << endl;
	  cout << "MeanDistance        " << parameters->getDouble( "MeanDistance" ) << endl;
    cout << "InputRegionSize     " << parameters->getUnsignedLong( "InputRegionSize" ) << endl;
    cout << "ReferenceRegionSize " << parameters->getUnsignedLong( "ReferenceRegionSize" ) << endl;
    cout << "SharedRegionSize    " << parameters->getUnsignedLong( "SharedRegionSize" ) << endl;
    cout << "InputSurface        " << parameters->getUnsignedLong( "InputSurface" ) << endl;
    cout << "ReferenceSurface    " << parameters->getUnsignedLong( "ReferenceSurface" ) << endl;
    cout << "CombinedArea        " << parameters->getUnsignedLong( "CombinedArea"  ) << endl;
    cout << "FalsePositives      " << parameters->getUnsignedLong( "FalsePositives" ) << endl;
    cout << "FalseNegatives      " << parameters->getUnsignedLong( "FalseNegatives" ) << endl;
    return EXIT_SUCCESS;
  }
  ofstream* theOutputFile;
  if ( bAppendToOutput )
    theOutputFile = new ofstream( sOutputFile.c_str(), ios_base::out );
  else
  {
    theOutputFile = new ofstream( sOutputFile.c_str() );
    (*theOutputFile) << "Input image,Reference image,Dice coefficient,Tanimoto coefficient,Hausdorff distance,Mean distance,";
    (*theOutputFile) << "Input region size,Reference region size,Shared region size,Input surface,Reference surface,Combined area,";
    (*theOutputFile) << "False positives,False negatives" << endl;
  }
    
  (*theOutputFile) << theInputFileNamesVec[0] << "," << theInputFileNamesVec[1] << ",";
  (*theOutputFile) << parameters->getDouble( "DiceCoefficient" ) << ",";
  (*theOutputFile) << parameters->getDouble( "TanimotoCoefficient" ) << ",";
  (*theOutputFile) << parameters->getDouble( "HausdorffDistance" ) << ",";
  (*theOutputFile) << parameters->getDouble( "MeanDistance" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "InputRegionSize" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "ReferenceRegionSize" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "SharedRegionSize" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "InputSurface" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "ReferenceSurface" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "CombinedArea" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "FalsePositives" ) << ",";
  (*theOutputFile) << parameters->getUnsignedLong( "FalseNegatives" ) << endl;
  theOutputFile->close();
  delete theOutputFile;
  
  return EXIT_SUCCESS;
}
