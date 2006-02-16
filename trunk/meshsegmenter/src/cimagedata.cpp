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
#include "cimagedata.h"
#include <cdatafileserver.h>

using namespace boost;

CImageData::CImageData() : colorTable( 0 )
{
	for ( int i = 0; i < 3; ++i )
	{
		extent[i] = 0;
		slicers[i] = vtkImageReslice::New();
		colorMappers[i] = vtkImageMapToColors::New();
		planeSources[i] = vtkPlaneSource::New();
		planeSources[i]->SetResolution( 1, 1 );
		planeSources[i]->SetOrigin( 0.0, 0.0, 0.0 );
		planeMappers[i] = vtkPolyDataMapper::New();
		planeMappers[i]->SetInput( planeSources[i]->GetOutput() );
		planeTextures[i] = vtkTexture::New();
		sliceActors[i] = vtkActor::New();
		sliceActors[i]->SetMapper( planeMappers[i] );
		sliceActors[i]->SetTexture( planeTextures[i] );
		sliceActors[i]->GetProperty()->SetAmbient( 0.8 );
		slicers[i]->SetOutputDimensionality( 2 );	
		slicers[i]->AutoCropOutputOn();
	}
	planeSources[0]->SetPoint1( 1.0, 0.0, 0.0 );
	planeSources[0]->SetPoint2( 0.0, 1.0, 0.0 );
	planeSources[1]->SetPoint1( 1.0, 0.0, 0.0 );
	planeSources[1]->SetPoint2( 0.0, 0.0, 1.0 );
	planeSources[2]->SetPoint1( 0.0, 0.0, 1.0 );
	planeSources[2]->SetPoint2( 0.0, 1.0, 0.0 );
	slicers[0]->SetResliceAxesDirectionCosines( 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 );
	slicers[1]->SetResliceAxesDirectionCosines( 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 );
	slicers[2]->SetResliceAxesDirectionCosines( 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0 );
	dataOutlinesActor = vtkActor::New();
	dataOutlines = vtkStructuredGridOutlineFilter::New();
}


CImageData::~CImageData()
{
	if ( colorTable )
		colorTable->Delete();	
	for ( int i = 0; i < 3; ++i )
	{
		sliceActors[i]->Delete();
		slicers[i]->Delete();
		colorMappers[i]->Delete();
		planeSources[i]->Delete();
		planeMappers[i]->Delete();
		planeTextures[i]->Delete();
	}
	dataOutlinesActor->Delete();
	dataOutlines->Delete();
}


vtkActor* CImageData::getOutlines()
{
	return NULL;
}

vtkActor* CImageData::getSlice( uint orientation )
{
	return sliceActors[orientation];
}

void CImageData::setSlicePosition( uint orientation, uint position )
{
	if ( orientation == 0 )
	{
		if ( position > extent[2] )
			position = extent[2];
		slicers[0]->SetResliceAxesOrigin( 0.0, 0.0, extent[2]/2.0 + position );
		planeSources[0]->SetOrigin( 0.0, 0.0, position );
		planeSources[0]->SetPoint1( extent[0], 0.0, position );
		planeSources[0]->SetPoint2( 0.0, extent[1], position );
 		planeSources[0]->UpdateWholeExtent();
//		slicers[0]->SetOutputExtent( 0, extent[0], 0, extent[1], 0, 0 );
 		slicers[0]->UpdateWholeExtent();
		planeMappers[0]->Update();
	}
	else if ( orientation == 1 )
	{
		if ( position > extent[1] )
			position = extent[1];
		slicers[1]->SetResliceAxesOrigin( 0.0, extent[1]/2.0 + position, 0.0 );
		planeSources[1]->SetOrigin( 0.0, position, 0.0 );
		planeSources[1]->SetPoint1( extent[0], position, 0.0 );
		planeSources[1]->SetPoint2( 0.0, position, extent[2] );
		planeSources[1]->Update();
		slicers[1]->Update();
	}
	else
	{
		if ( position > extent[0] )
			position = extent[0];
		slicers[2]->SetResliceAxesOrigin( extent[0]/2.0 + position, 0.0, 0.0 );
		planeSources[2]->SetOrigin( position, 0.0, 0.0 );
		planeSources[2]->SetPoint1( position, 0.0, extent[2] );
		planeSources[2]->SetPoint2( position, extent[1], 0.0 );		
		planeSources[2]->Update();
		slicers[2]->Update();
	}
}

void CImageData::setColorTable( vtkLookupTable* lut )
{
	if ( colorTable )
		colorTable->Delete();
	else if ( extent[0] > 0 )
	for ( int i = 0; i < 3; ++i )
	{
		colorMappers[i]->SetInput( slicers[i]->GetOutput() );			
		planeTextures[i]->SetInput( colorMappers[i]->GetOutput() );
	}
	colorTable = lut;
	colorTable->Register( this );
	for ( int i = 0; i < 3; ++i )
	{
		colorMappers[i]->SetLookupTable( lut );
	}
}

void CImageData::loadDataSet( std::string sFilename )
{
 	imageData = imageLoader.load( sFilename, theImage );
	cerr << theImage->getType().name() << endl;
	int ext[6];
	
	imageData->GetExtent(ext);
	extent[0] = static_cast<uint>( ext[1] );
	extent[1] = static_cast<uint>( ext[3] );
	extent[2] = static_cast<uint>( ext[5] );
	cerr << "Extents are " << extent[0] << " x " << extent[1] << " x " << extent[2] << endl;
	for ( int i = 0; i < 3; ++i )
	{		
		slicers[i]->SetInput( imageData );		
		if ( colorTable )
		{
			colorMappers[i]->SetInput( slicers[i]->GetOutput() );			
			planeTextures[i]->SetInput( colorMappers[i]->GetOutput() );
		}
		else
		{
			planeTextures[i]->SetInput( slicers[i]->GetOutput() );
		}
		planeSources[i]->SetOrigin( 0.0, 0.0, 0.0 );
	}
	imageData->GetScalarRange( scalarRange );
	cerr << "Range is " << scalarRange[0] << " - " << scalarRange[1] << endl;
	scalarRange[0] = theImage->getDataRange().getMinimum();
	scalarRange[1] = theImage->getDataRange().getMaximum();
	cerr << "Range is " << scalarRange[0] << " - " << scalarRange[1] << endl;
	slicers[2]->SetResliceAxesOrigin( extent[0]/2.0, 0.0, 0.0 );
	slicers[1]->SetResliceAxesOrigin( 0.0, extent[1]/2.0, 0.0 );
	slicers[0]->SetResliceAxesOrigin( 0.0, 0.0, extent[2]/2.0 );
	planeSources[0]->SetPoint1( extent[0], 0.0, 0.0 );
	planeSources[0]->SetPoint2( 0.0, extent[1], 0.0 );
	planeSources[1]->SetPoint1( extent[0], 0.0, 0.0 );
	planeSources[1]->SetPoint2( 0.0, 0.0, extent[2] );
	planeSources[2]->SetPoint1( 0.0, 0.0, extent[2] );
	planeSources[2]->SetPoint2( 0.0, extent[1], 0.0 );
	cerr << "Load end" << endl;
}

uint CImageData::getExtent( uint orientation )
{
	return extent[orientation];
}

double CImageData::getMinimum()
{
	return scalarRange[0];
}

double CImageData::getMaximum()
{
	return scalarRange[1];
}

TField3DPtr CImageData::computeExternalForces()
{
// 	CGaussDerivative* grad = new CGaussDerivative(0);
// 	CSobelGradient* grad2 = new CSobelGradient(0);
// 	CVectorFlow* gvf = new CVectorFlow(0);
// 	CReduceVector* rv = new CReduceVector(0);
// 	CFieldToImage* fti = new CFieldToImage(0);
// 	CTypedMap* params = grad->getParameters();
// 	params->setDouble("Sigma", 1.0);	
// 	grad->setInput( theImage );
// 	cerr << "Compute 1st derivative" << endl;
// 	grad->apply();
// 	rv->setInput( grad->getOutput() );
// 	rv->apply(); 
// 	fti->setInput( rv->getOutput() );
// 	cerr << "Convert to image" << endl;
// 	fti->apply(); 
// 	cerr << "Compute 2nd derivative" << endl;
// 	grad2->setInput( fti->getOutput() );
// 	grad2->apply();
// 	delete rv;
// 	delete fti;
// 	delete grad;
// 	cerr << "Compute GVF field" << endl;
// 	params = gvf->getParameters();
// 	params->setUnsignedLong("Iterations", 50);
// 	//params->setUnsignedLong("Method", 2);
// 	params->setDouble("Kappa", 0.25);
// 	gvf->setInput( grad2->getOutput() );
// 	gvf->apply(); delete grad2;
// 	cerr << "All okay" << endl;
// 	TField3DPtr field = static_pointer_cast<TField3D>( gvf->getOutput() );
// 	delete gvf;
/*	TPipelineItemPtr grad ( new CGaussDerivative(0) );
	TPipelineItemPtr grad2 ( new CSobelGradient(3) );
	TPipelineItemPtr gvf ( new CVectorFlow(4) );
	TPipelineItemPtr rv ( new CReduceVector(1) );
	TPipelineItemPtr fti ( new CFieldToImage(2) );
	CTypedMap* params = grad->getParameters();
	params->setDouble("Sigma", 1.0);	
	grad->setInput( theImage );
	grad->cacheOutput( false );
	params = gvf->getParameters();
	params->setUnsignedLong("Iterations", 50);
	params->setDouble("Kappa", 0.25);
	rv->addConnection( grad );
	rv->cacheOutput( false );
	fti->addConnection( rv );
	fti->cacheOutput( false );
	grad2->addConnection( fti );
	grad2->cacheOutput( false );
	gvf->addConnection( grad2 );
	gvf->update();
	cerr << "All okay" << endl;*/
	TDataFile f =	getFileServer().loadDataSet( "/data/data/mouseout.avf" );
	TField3DPtr field = static_pointer_cast<TField3D>( f.first );
	return field;
}
