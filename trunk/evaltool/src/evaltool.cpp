/***************************************************************************
 *   Copyright (C) 2006 by Hendrik Belitz                                  *
 *   hbelitz@users.berlios.de                                              *
 *                                                                         *
 ***************************************************************************/

/*! \mainpage evaltool - computing discrepancy metrics for images
 *
 * evaltool - computes evaluation metrics for two given voxel images
 *
 * Evaltool is directly called from the command prompt:
 *
 * <code>evaltool <i>file1 file2</i> [-o outfile] [-a] [-l labelvalue] [-h]</code>
 *
 * You must call evaltool with two image file names. The output file is optional and
 * defaults to stdout. With [-a], you tell evaltool to append to an existing file instead
 * of overwriting it. [-l] defines the label of the objects to compare and defaults to 1.
 * [-h] displays a help message.
 */
 
// Standard library includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>

// Boost standard library includes
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

// AIPS includes
#include <cdatafileserver.h>
#include <canalyzehandler.h>
#include <citkhandler.h>
#include <csimpledathandler.h>
#include <cdatahandler.h>
#include <ctypedmap.h>

// Local includes
#include "cdiscrepancymeasures.h"
#include "checklic.h"

using namespace std;
using namespace aips;
using namespace boost;
using namespace boost::program_options;

bool bAppendToOutput; ///< Append to old output or create a new one
uint uiLabel; ///< Intensity label to analyze
string sOutputFile; ///< File name of output
vector<string> theInputFileNamesVec; ///< Input file names

void license()
{
	licInfo* lic = readLicenseFile( getenv("MBS_LICENSE") );
  if( lic == NULL )
  {
  	cerr << "Cannot find license file!" << endl;
  	exit(-1);
  }
  /* Check for valid license key */
  int ret = validateLicenseInfo( lic );
  switch( ret )
  {
                case LIC_OK:
                        cerr << "License is valid\n" << endl; break;
                case LIC_EXPIRED:
                        cerr << "License has expired\n" << endl; break;
                case LIC_WRONGUSER:
                        cerr << "Invalid user for license\n" << endl; break;
                case LIC_WRONGHOST:
                        cerr << "Invalid host for license\n" << endl; break;
                case LIC_WRONGHOSTID:
                        cerr << "Invalid hostid for license\n" << endl; break;
                case LIC_WRONGDOMAIN:
                        cerr << "Invalid domain for license\n" << endl; break;
                case LIC_KEYINVALID:
                        cerr << "Invalid key for license\n" << endl; break;
                default:
                        printf("Unknown error\n"); break;
  }
  if ( ret != 0 )
  	exit( EXIT_FAILURE );
}

/**
 * Parses the command line to extract and set all parameters
 * \param argc Number of arguments (from main() )
 * \param argv Argument list (from main() )
 */
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
	// Check license
	license();
	
	// Check command line. If something went wrong, exit
  if ( !parseCommandLine( argc, argv ) )
    return EXIT_FAILURE;

	// Initialize file handling routines
  shared_ptr<CITKHandler> h1 ( new CITKHandler );
	shared_ptr<CSimpleDatHandler> h2 ( new CSimpleDatHandler );	
	shared_ptr<CDataHandler> h3 ( new CDataHandler );
	shared_ptr<CAnalyzeHandler> h4 ( new CAnalyzeHandler );
	
	getFileServer().addHandler( h1 );
	getFileServer().addHandler( h2 );
	getFileServer().addHandler( h3 );
	getFileServer().addHandler( h4 );

	// Initialize computation
	CDiscrepancyMeasures eval(0);
  CTypedMap* parameters = eval.getParameters();

  // Set parameters and input data, start computation  
  parameters->setUnsignedLong( "Label", uiLabel );
  TDataSetPtr image = getFileServer().loadDataSet( theInputFileNamesVec[0] ).first;
  TDataSetPtr reference = getFileServer().loadDataSet( theInputFileNamesVec[1] ).first;
	eval.setInput( image );
	eval.setInput( reference, 1 );
	eval.apply();

  // No output file given, output to stdout
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

  // Output to file
  auto_ptr<ofstream> theOutputFile;
  if ( bAppendToOutput )
    theOutputFile.reset( new ofstream( sOutputFile.c_str(), ios_base::out ) );
  else
  {
    theOutputFile.reset( new ofstream( sOutputFile.c_str() ) );
    (*theOutputFile) << "Input image,Reference image,Dice coefficient,Tanimoto coefficient,"
    								 << "Hausdorff distance,Mean distance,"
    								 << "Input region size,Reference region size,Shared region size,"
    								 << "Input surface,Reference surface,Combined area,"
    								 << "False positives,False negatives" << endl;
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
  
  return EXIT_SUCCESS;
}
