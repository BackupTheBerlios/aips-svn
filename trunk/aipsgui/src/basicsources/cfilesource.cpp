/************************************************************************
 * File: cfilesource.cpp                                                *
 * Project: AIPS basic sources plugin library                           *
 * Description: A file loading dialog module                            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 29.10.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

// Library includes
/*#include "libid.h"*/

extern const char* sLibID;

#include "cfilesource.h"

using namespace std;

/* Structors */

/**
 * \param ulID unique module ID
 */
CFileSource::CFileSource( ulong ulID ) throw()
  : CSource ( ulID, 1, "CFileSource", "0.4", "CSource" ), CObserver ( "CFileSource", "0.4", "CSource" )
{
  setModuleID( sLibID );
  setModuleName( "Image loader" );

	outputsVec[0].portType = IOOther;
	
  sDocumentation = 	"Loads image or field data.\n"
										"** Input ports:\n"
										" none\n"
										"** Output ports:\n"
										" 0: A scalar multi-channel 2D or 3D data set\n"
										"** Parameters:\n"
										" Filename: file to load\n"
										" Path: path to data files";

	parameters.initString( "Filename", "" );
	parameters.initString( "Path", "" );									
/* HB 28-06-05 */	
 	myFileSourceDialog.reset( new CFileSourceDialog( this ) );
  setModuleDialog( myFileSourceDialog );
	myFileSourceDialog->setFileTypes( getFileServer().supportedFileTypes() );
	myFileSourceDialog->setCaption( getModuleName().c_str() );	
	myFileSourceDialog->attachObserver( this, EFileNameChangedEvent );
	myFileSourceDialog->attachObserver( this, ELoadActivatedEvent );
}

CFileSource::~CFileSource() throw()
{
}

/* Other methods */

void CFileSource::selectNewFile( string sFilename )
{
	string sPath = sFilename.substr( 0, sFilename.find_last_of("/")+1 );
  parameters.setString( "Filename", sFilename );
	parameters.setString( "Path", sPath );
	myFileSourceDialog->setPath( sPath );
}

void CFileSource::updateData()
{
FBEGIN;
BENCHSTART;
  deleteOldOutput();
cerr << "A" << endl;
  shared_ptr<CDataSet> inputDataPtr;
  try
  {
    inputDataPtr = getFileServer().loadDataSet( parameters.getString( "Filename" ) ).first;		
cerr << "B" << endl;
  }
  catch ( FileException &e )
  {	 
		alog << LWARN << "No data loaded" << endl; return;
	}
  catch ( NullException &e )
  { 
cerr << "C" << endl;
		alog << LFATAL << e.what() << endl; std::terminate(); 
	}
cerr << "D" << inputDataPtr.get() << endl;
  setOutput( inputDataPtr );
BENCHSTOP;	
FEND;
cerr << "E" << inputDataPtr.get() << endl;
	forceRecomputation();
	notify( shared_ptr<CDataChangedEvent>( new CDataChangedEvent( this ) ) );
}

CPipelineItem* CFileSource::newInstance( ulong ulID ) const throw()
{ 
	return ( new CFileSource( ulID ) ); 
}

void CFileSource::apply() throw()
{
	//myFileSourceDialog->setPath( parameters.getString( "Path" ) );	
	bModuleReady = true;
}

void CFileSource::execute( shared_ptr<CEvent> anEvent )
{
	if ( anEvent->getType() == EFileNameChangedEvent )
	{
		CFileNameChangedEvent* ptr = static_cast<CFileNameChangedEvent*>( anEvent.get() );
		if( ptr )
			selectNewFile( ptr->getFilename() );
	}
	else if ( anEvent->getType() == ELoadActivatedEvent )
	{
cerr << "1" << endl;	
		updateData();
	}
}
