/***************************************************************************
 *   Copyright (C) 2004 by Patrick Musmann                                 *
 *   p.musmann@fz-juelich.de                                               *
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
#include "cvinterfilehandler.h"

using namespace aips;
using namespace std;


/***************************************************************************
 * Structors                                                               *
 ***************************************************************************/
/** Constructor */
CVInterfileHandler::CVInterfileHandler() throw()
  : CBinaryFileHandler( "CVInterfileHandler", "0.1", "CBinaryFileHandler" )
{
  supportedFileTypesVec.push_back("HV");
  supportedFileTypesVec.push_back("hv");
  supportedFileTypesVec.push_back("V");
  supportedFileTypesVec.push_back("v");
}

/** Destructor */
CVInterfileHandler::~CVInterfileHandler() throw()
{
}

/***************************************************************************
 * Structors                                                               *
 ***************************************************************************/
/// Loads a data set
TDataFile CVInterfileHandler::load( const string& sFilename )
  const throw( FileException )
{
#ifdef VERBOSE
    cerr << __PRETTY_FUNCTION__ << "\n given sFilename = " << sFilename << endl;
#endif
    // Inputfilenames for Header and Data files
    const string sHeaderFilename = sFilename.substr( 0, sFilename.rfind( ".", sFilename.length()-1 ) ) + ".hv";
    // Open Header File
    ifstream theFile( sHeaderFilename.c_str() );
    if( !theFile.good() ){
        throw( FileException( SERROR( ("Cant open file = " + sHeaderFilename).c_str() ),
        	CException::RECOVER, ERR_FILENOTFOUND ) );
    }
    //read header
    CInterfileHeader *aHeader = new CInterfileHeader();
    aHeader->loadHeader( theFile );
    // Close Header, reset ifstream
    theFile.close();
    theFile.clear();

    // Get Dimensions
    std::vector<size_t> dimensionSize = aHeader->getExtents();

    // Get name of data file
    string sDataFilename = aHeader->getString( "name of data file" );
    if( sDataFilename.at(0) != '/' ) {
        //if sDataFilename is not a absolute path, use the path from sFilename
        if( sFilename.at(0) == '/' )
            sDataFilename = sFilename.substr( 0, sFilename.rfind( "/", sFilename.length()-1 ) ) + '/' + sDataFilename;
    }

    // Open data file
    theFile.open( sDataFilename.c_str() );
    if( !theFile.good() )
        throw( FileException( SERROR( ("Cant open file = "+sDataFilename).c_str() ),
        	CException::RECOVER, ERR_FILENOTFOUND ) );
    // Initialization of Data (3D), allocation of memory
    CTypedData<float> *aDataSet = new CTypedData<float>( 3, dimensionSize );
    // read raw data file
    theFile.read( reinterpret_cast<char*>(aDataSet->getVoidArray()), aDataSet->getArraySize() * sizeof(float) );

    return make_pair( aDataSet, aHeader );
}


/// Saves a data set
void CVInterfileHandler::save( const string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
#ifdef VERBOSE
    cerr << __PRETTY_FUNCTION__ << "\n given sFilename = " << sFilename << endl;
#endif
    // Outputfilenames for Header and Data files
    const string sFilenameBase=sFilename.substr( 0, sFilename.rfind( ".", sFilename.length() - 1 ) );
    const string sHeaderFilename=sFilenameBase+".hv";
    const string sDataFilename=sFilenameBase+".v";

    // Check DataSet
    if( theData.first == NULL )
        throw( NullException( SERROR( "No DataSet" ), CException::RECOVER, ERR_REQUESTNULL ) );
    // Check data type
    if( theData.first->getType() != typeid(float) )
        throw( FileException( SERROR( "No float DataSet" ), CException::RECOVER, ERR_FILEFORMATUNSUPPORTED ) );
    // ... so we have a float data set
/* FIXME */		
    boost::shared_ptr<CTypedData<float> > fDataSet = 
			boost::dynamic_pointer_cast<CTypedData<float> >(theData.first);

    // Determe dimension size 2d/3d
    size_t dimensionSize[3];
    dimensionSize[0] = theData.first->getExtent(0);
    dimensionSize[1] = theData.first->getExtent(1);
    if( theData.first->getDimension() > 2 )
        dimensionSize[2] = theData.first->getExtent(2);
    else
        dimensionSize[2] = 1;

    // Saving Header
    ofstream theFile(sHeaderFilename.c_str());
    if ( !theFile.good() )
      throw( FileException( SERROR( ("Cant open file = "+sHeaderFilename).c_str() ),
       	CException::RECOVER, ERR_FILENOTFOUND ) );

    // a new Interfile header; setting required arguments
    CInterfileHeader aHeader( initHV() );
    aHeader.setString( "name of data file", sDataFilename );

    aHeader.setUnsignedLong( "!matrix size [3]", dimensionSize[2] );
    aHeader.setUnsignedLong( "!matrix size [2]", dimensionSize[1] );
    aHeader.setUnsignedLong( "!matrix size [1]", dimensionSize[0] );

    // setting optional arguments
    // set voxelsize in mm from cvolume
    if( theData.second->isDefined( "d" ) )
    {
        float d = theData.second->getDouble( "d" );
        aHeader.setDouble( "scaling factor (mm/pixel) [3]", d );
        aHeader.setDouble( "scaling factor (mm/pixel) [2]", d );
        aHeader.setDouble( "scaling factor (mm/pixel) [1]", d );
    }

    // TODO: append/save given header (theData.second) as comment

    // save and close header
    aHeader.saveHeader( theFile );
    theFile.close();
    theFile.clear();

    // Saving Data
    theFile.open( sDataFilename.c_str() );
    if ( !theFile.good() )
        throw( FileException( SERROR( ("Cant open file = "+sDataFilename).c_str() ),
        	CException::RECOVER, ERR_FILENOTFOUND ) );
    // storage order on disk is x,y,z; x is the fasted variing dimension
    theFile.write( reinterpret_cast<const char*>(fDataSet->getVoidArray()), fDataSet->getArraySize() * sizeof(float) );
}


const vector< pair<string, string> > CVInterfileHandler::initHV() const
{
    vector< pair<string, string> > defaultVec;
    defaultVec.push_back( make_pair( "!INTERFILE",                                           "" ) );
    defaultVec.push_back( make_pair( "name of data file",                     "[missing argument]" ) );
    defaultVec.push_back( make_pair( "!GENERAL DATA",                                        "" ) );
    defaultVec.push_back( make_pair( "!GENERAL IMAGE DATA",                                  "" ) );
    defaultVec.push_back( make_pair( "!type of data",                                     "PET" ) );
#if BYTE_ORDER==LITTLE_ENDIAN
    defaultVec.push_back( make_pair( "imagedata byte order",                     "LITTLEENDIAN" ) );
#else
    defaultVec.push_back( make_pair( "imagedata byte order",                        "BIGENDIAN" ) );
#endif
    defaultVec.push_back( make_pair( "!PET STUDY (General)",                                 "" ) );
    defaultVec.push_back( make_pair( "!PET data type",                                  "Image" ) );
    defaultVec.push_back( make_pair( "process status",                          "Reconstructed" ) );
    //TODO: flexible number format, see above
    defaultVec.push_back( make_pair( "!number format",                                  "float" ) );
    defaultVec.push_back( make_pair( "!number of bytes per pixel",                          "4" ) );
    //dimension
    defaultVec.push_back( make_pair( "number of dimensions",                                "3" ) );
    // dim x, fasted variing dimension
    defaultVec.push_back( make_pair( "matrix axis label [1]",                               "x" ) );
    defaultVec.push_back( make_pair( "!matrix size [1]",                   "[missing argument]" ) );
    defaultVec.push_back( make_pair( "scaling factor (mm/pixel) [1]",                       "1" ) );
    // dim y
    defaultVec.push_back( make_pair( "matrix axis label [2]",                               "y" ) );
    defaultVec.push_back( make_pair( "!matrix size [2]",                   "[missing argument]" ) );
    defaultVec.push_back( make_pair( "scaling factor (mm/pixel) [2]",                       "1" ) );
    // dim z
    defaultVec.push_back( make_pair( "matrix axis label [3]",                               "z" ) );
    defaultVec.push_back( make_pair( "!matrix size [3]",                   "[missing argument]" ) );
    defaultVec.push_back( make_pair( "scaling factor (mm/pixel) [3]",                       "1" ) );
    //TODO: what is this?
    defaultVec.push_back( make_pair( "number of time frames",                               "1" ) );
    defaultVec.push_back( make_pair( "image scaling factor[1]",                             "1" ) );
    defaultVec.push_back( make_pair( "data offset in bytes[1]",                             "0" ) );
    defaultVec.push_back( make_pair( "quantification units",                                "1" ) );
    defaultVec.push_back( make_pair( "!END OF INTERFILE",                                    "" ) );
    return defaultVec;
}
