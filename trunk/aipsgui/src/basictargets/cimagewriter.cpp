/************************************************************************
 * File: cimagewriter.cpp                                               *
 * Project: AIPS                                                        *
 * Description: A writer for the simple data format                     *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2003-12-15                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cimagewriter.h"

using namespace std;
using namespace boost;

/** \param ulID unique module id */
CImageWriter::CImageWriter( ulong ulID ) throw()
  : CTarget ( ulID, 1, "CImageWriter", "0.3", "CTarget" ), sFilename( "" ), CObserver( "CImageWriter", "0.3", "CTarget" )
{
  setModuleName( "Image writer" );
  setModuleID( sLibID );

  sDocumentation = "Writes 2D and 3D scalar images\n"
									 "** Input ports:\n"
                   " 0: A scalar multi-channel 2D or 3D data set\n"
                   "** Output ports:\n"
                   " none";

  inputsVec[0].portType = IOOther;
/* HB 28-06-05 */	
myDialog.reset( new CImageWriterDialog( this ) );
	setModuleDialog( myDialog );
	myDialog->attachObserver( this, EFileNameChangedEvent );
	myDialog->attachObserver( this, ELoadActivatedEvent );
}

CImageWriter::~CImageWriter() throw()
{
}

void CImageWriter::selectNewFile( string sFile ) 
{
	string sPath = sFilename.substr( 0, sFile.find_last_of("/")+1 );
  parameters.setString( "Filename", sFile );
	parameters.setString( "Path", sPath );
	myDialog->setPath( sPath );
	sFilename = sFile;
}

void CImageWriter::updateData() 
{
FBEGIN;
BENCHSTART;
  if ( sFilename == "" )
  {
    alog << LWARN << "Could not save file with no name" << endl;
    return;
  }
	if ( typeid( *myInput ) == typeid( TImage ) )
		actualSaver<TImage>();
	else if ( typeid( *myInput ) == typeid( TField ) )
		actualSaver<TField>();		
	else if ( typeid( *myInput ) == typeid( TField2D ) )
		actualSaver<TField2D>();		
	else if ( typeid( *myInput ) == typeid( TField3D ) )
		actualSaver<TField3D>();		
	else
		alog << LWARN << SERROR( "No input or wrong data type" ) << endl;	
  
BENCHSTOP;
FEND;
}

CPipelineItem* CImageWriter::newInstance( ulong ulID ) const throw()
{
  return ( new CImageWriter( ulID ) );
}

const std::string CImageWriter::dump() const throw()
{
/*  ostringstream os;
  os << "sFilename: " << sFilename << " displayPtr: " << displayPtr << "\n";*/
  return CTarget::dump();// + os.str();
}

template<typename T> void CImageWriter::actualSaver() throw()
{
	if ( !myInput || myInput->getType() != typeid( dataTraits<T>::dataType ) )
		return;
	shared_ptr<T> inputPtr = static_pointer_cast<T>( myInput );
  if ( !checkInput( inputPtr ) ) // No data, no operation...
		return;
	shared_ptr<CImageHeader> theHeader;
  TDataFile aPair( inputPtr, theHeader );	
  try
  {		
    getFileServer().saveDataSet( sFilename, aPair );
  }
  catch ( FileException &e )
  { 
		alog << LWARN << "No data saved" << e.what() << endl; 
		return; 
	}
}

void CImageWriter::execute( boost::shared_ptr<CEvent> anEvent )
{
	CEvent* eventPtr = anEvent.get();
	if ( anEvent->getType() == EFileNameChangedEvent )
	{
		CFileNameChangedEvent* ptr = static_cast<CFileNameChangedEvent*>( eventPtr );
		if( ptr )
			selectNewFile( ptr->getFilename() );
	}
	else if ( anEvent->getType() == ELoadActivatedEvent )
	{
		CLoadActivatedEvent* ptr = static_cast<CLoadActivatedEvent*>( eventPtr );
		if( ptr )
			updateData();
	}
}

void CImageWriter::apply() throw()
{
	myInput = getInput();
}