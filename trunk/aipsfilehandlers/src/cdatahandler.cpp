/***********************************************************************
 * File: cdatahandler.h                                                *
 * Project: AIPS                                                       *
 * Description: A file handler for the ".data" volume format           *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 24.10.03                                                   *
 ***********************************************************************/

#include "cdatahandler.h"

using namespace std;
using namespace boost;

/** Constructor */
CDataHandler::CDataHandler() throw()
  : CBinaryFileHandler( "CDataHandler", "0.4", "CBinaryFileHandler" )
{
  supportedFileTypesVec.push_back("DATA");
  supportedFileTypesVec.push_back("data");  
}

/** Destructor */
CDataHandler::~CDataHandler() throw()
{
}

/**
 * Loads an unsigned short scalar volume dataset from the given file
 * \param sFilename Name of the volume file
 * \exception FileException on any file error
 */
TDataFile CDataHandler::load( const std::string& sFilename )
  const throw( FileException )
{
FBEGIN;
  ifstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );

  if ( theFile.is_open() )
  {
    shared_ptr<CDataHeader> aHeader ( new CDataHeader );
		aHeader->loadHeader( theFile );
    vector<size_t> dimensionSize = aHeader->getExtents();
    EDataType voxelSize = getDataType( aHeader->getVoxelType() );
		bool bFileEndianess = aHeader->getEndianess();
    theFile.close();
    // Determine data filename
    std::string sDataFilename( sFilename, 0, sFilename.size() - 4 );
    if ( voxelSize == DUInt8 )
      sDataFilename += "rawb";
    else if ( voxelSize == DUInt16 )
      sDataFilename += "raws";
    else
    {
      ostringstream os;
      os << "Image format with voxel size " << aHeader->getVoxelType() << " not supported!";
      throw ( FileException( SERROR( os.str().c_str() ),
      	CException::RECOVER, ERR_FILEFORMATUNSUPPORTED ) );
    }
		bool bCompressed = false;
		gz::igzstream theGzFile;
		theFile.clear();
	  theFile.open( sDataFilename.c_str() );
  	if ( !theFile.is_open() )
	  {
			theFile.clear();
			alog << LINFO << "Uncompressed data file not found. Looking for compressed data file..." << endl;
			bCompressed = true;
			sDataFilename += ".gz";
			theGzFile.open( sDataFilename.c_str() );
	  	if ( !theGzFile.good() )
    		throw ( FileException( SERROR( "Could not open image data file!" ),
  	    	CException::RECOVER, ERR_FILEACCESS ) );
	  }
    TDataSetPtr aDataSet;
    if ( dimensionSize[2] > 1 )
      aDataSet.reset( new TImage( 3, dimensionSize ) );
    else // For 2D images a 2D data set is sufficient
      aDataSet.reset( new TImage( 2, dimensionSize ) );

    if ( !bCompressed )
		{
			loadData( aDataSet, theFile, getDataType( aHeader->getVoxelType() ), bFileEndianess );
			theGzFile.close();
		}
		else
		{
			loadData( aDataSet, theGzFile, getDataType( aHeader->getVoxelType() ), bFileEndianess );
			theFile.close();
		}

    return make_pair( aDataSet, aHeader );
  } //endif ( theFile.is_open() )
	
  throw ( FileException( SERROR( "Could not open header file" ),
    CException::RECOVER, ERR_FILEACCESS ) );
FEND;		
}

/**
 * Saves an unsigned short scalar image or volume dataset to the given file
 * \param sFilename Name of the volume file
 * \param theData Pair of data set and header to be saved
 * \exception FileException on any file error
 */
void CDataHandler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
FBEGIN;
	shared_ptr<CDataHeader> aHeader ( new CDataHeader );
	
	bool bCompressed = false;
	if ( theData.second->isDefined( "Compress" ) )
	{
		if ( theData.second->getBool( "Compress" ) == true )
			bCompressed = true;
	}

	bool bFileEndianess = false;
	if ( theData.second->isDefined( "FileEndianess" ) )
		bFileEndianess = theData.second->getEndianess();
	aHeader->setEndianess( bFileEndianess );
		
  if ( theData.first->getDataDimension() != 1 )
    throw( FileException( SERROR( "<.data> only supports monochannel images" ), CException::RECOVER ) );

  // Define and initialize variables
 	if ( !( theData.first->getType() == typeid( dataTraits<TImage>::dataType ) ) )
	{
		alog << LWARN << "Illegal input for image writer" << endl;
		return;
	}

  shared_ptr<TImage> aDataSet = static_pointer_cast<TImage>( theData.first );
  if ( aDataSet == NULL )
    throw ( FileException(
      SERROR( "Data set type != ushort cannot be saved as <.data> files yet" ), CException::RECOVER,
      ERR_FILEFORMATUNSUPPORTED ) );

	ushort usVoxelSize = 0;
  if ( aDataSet->getMaximum() <= 255 )
    usVoxelSize = 1;
  else if ( aDataSet->getMaximum() <= numeric_limits<ushort>::max() )
    usVoxelSize = 2;	
	else
		throw( FileException( SERROR( "Cannot only write files with a voxel size up to 16 bit" ),
			CException::RECOVER ) );

	if ( usVoxelSize == 1 )
		aHeader->setVoxelType( "UInt8" );
	else
		aHeader->setVoxelType( "UInt16" );
	aHeader->setExtents( aDataSet->getExtents() );
	
  ofstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "File creation error" ),
    	CException::RECOVER, ERR_FILECREATIONERROR ) );
			
	aHeader->saveHeader( theFile );  
  theFile.close();
  
	std::string sDataFilename( sFilename, 0, sFilename.size() - 4 );
  if ( usVoxelSize == 1 )
    sDataFilename += "rawb";
  else if ( usVoxelSize == 2 )
    sDataFilename += "raws";
	if ( !bCompressed )
	{
	  theFile.open( sDataFilename.c_str() );
  	if ( !theFile.is_open() )
	  {
  	  throw ( FileException( SERROR( "Could not create rbyte/rshort data file" ),
	      CException::RECOVER, ERR_FILEACCESS ) );
  	}
		if ( usVoxelSize == 1 )
  		saveData( aDataSet, theFile, DUInt8, bFileEndianess );
		else
			saveData( aDataSet, theFile, DUInt16, bFileEndianess );
		theFile.close();
	}
	else
	{
		gz::ogzstream theGzFile;
		sDataFilename += ".gz";
  	theGzFile.open( sDataFilename.c_str() );
  	if ( !theGzFile.rdbuf()->is_open() )
	  {
    	throw ( FileException( SERROR( "Could not create rbyte/rshort data file!" ),
      	CException::RECOVER, ERR_FILEACCESS ) );
  	}			
		if ( usVoxelSize == 1 )
  		saveData( aDataSet, theGzFile, DUInt8, bFileEndianess );
		else
			saveData( aDataSet, theGzFile, DUInt16, bFileEndianess );	  
		theGzFile.close();
	}
FEND;	
}
