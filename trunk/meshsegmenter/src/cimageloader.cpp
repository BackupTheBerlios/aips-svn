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
#include "cimageloader.h"
#include <aipsnumeric.h>
#include <cdatafileserver.h>
#include <canalyzehandler.h>
#include <csimpledathandler.h>
#include <cvectorfieldhandler.h>
#include <citkhandler.h>

using namespace std;
using namespace boost;
using namespace aips;

CImageLoader::CImageLoader()
{
	shared_ptr<CITKHandler> h0 ( new CITKHandler );	
	shared_ptr<CAnalyzeHandler> h1 ( new CAnalyzeHandler );	
	shared_ptr<CSimpleDatHandler> h2 ( new CSimpleDatHandler );
	shared_ptr<CVectorFieldHandler> h3 ( new CVectorFieldHandler );
	getFileServer().addHandler( h0 );
	getFileServer().addHandler( h1 );
	getFileServer().addHandler( h2 );
	getFileServer().addHandler( h3 );
}

CImageLoader::~CImageLoader()
{
}

vtkImageData* CImageLoader::load( std::string sFilename, TImagePtr& imagePtr )
{
	cerr << "Loading data... ";
	TDataFile theData = getFileServer().loadDataSet( sFilename );
	if ( theData.first->getType() == typeid( short ) )
	{
		imagePtr = static_pointer_cast<TImage>( theData.first );
		cerr << "Correct" << endl;
	}
	else
		cerr << theData.first->getType().name() << endl;
	cerr << "done" << endl;
	cerr << "Starting conversion... ";
	// Create vtk structured points structure
  vtkImageData* sp = vtkImageData::New();
  sp->SetDimensions( theData.first->getExtent(0), theData.first->getExtent(1), theData.first->getExtent(2) );
  sp->AllocateScalars();	
  // Assign dataset to structured points
  vtkPointData *p=sp->GetPointData();
	vtkShortArray* sArray = NULL;
	vtkDoubleArray* fArray = NULL;
	TField* floatSet = dynamic_cast<TField*>( theData.first.get() );	
	if ( floatSet != NULL )
	{
		fArray = vtkDoubleArray::New();
		fArray->SetArray(floatSet->getArray(),
			theData.first->getExtent(0)*theData.first->getExtent(1)*theData.first->getExtent(2), 1 );
		p->SetScalars( fArray );
		sp->SetScalarTypeToDouble();
	}
	else 
	{
		TImage* shortSet = dynamic_cast<TImage*>( theData.first.get() );
		if ( shortSet != NULL )
		{
			short* sA = new short[theData.first->getExtent(0)*theData.first->getExtent(1)*theData.first->getExtent(2)];
			short* sAs = sA;
			for( TImage::iterator it = shortSet->begin(); it != shortSet->end(); ++it, ++sAs )
				*sAs = *it;
			sArray = vtkShortArray::New();
			sArray->SetArray( sA,
				theData.first->getExtent(0)*theData.first->getExtent(1)*theData.first->getExtent(2), 1 );
			p->SetScalars( sArray );
			sp->SetScalarTypeToShort();
		}		
		else
		{
	    sp->Delete();		
			throw( FileException( "CVTkHandler - Unknown image format in dataset. Image was not saved" ) );
		}
	}
	sp->UpdateData();
	sp->SetSpacing( 1.0, 1.0, 1.0 );
	sp->SetOrigin( static_cast<double>( theData.first->getExtent(0) )/ 2.0, 
		static_cast<double>( theData.first->getExtent(1) )/ 2.0,
		static_cast<double>( theData.first->getExtent(2) )/ 2.0	);
	cerr << sp->GetNumberOfScalarComponents() << endl;
	cerr << "done" << endl;	
	return sp;
}
