/************************************************************************
 * File: cmainwindow                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz ($EMAIL)                                            *
 *                                                                      *
 * Created: Mo Okt 18 2004                                                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cmainwindow.h"
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkMarchingCubes.h>
//#include <vtkGenericContourFilter.h>
#include <fstream>
#include <string>
#include <cisosurface.h>
#include <clowpassfilter.h>
//#include <imagefilters/cbinarization.h>

using namespace std;
using namespace aips;
using namespace boost;


struct Pair
{
	int ulID1; // id of Point 1
	int ulID2; // id of Point 2
	SEdge* anEdgePtr;
};

bool pairLess( Pair& a, Pair& b ) 
{ 
	return ( ( a.ulID1 < b.ulID1 ) || ( a.ulID1 <= b.ulID1 && a.ulID2 < b.ulID2 ) ); 
}

struct TVIn
{
	TVector3D thePosition;
	ulong ulID;
	bool del;
};

struct TFIn
{
	int vts[3];
};

bool vecLess(  const TVIn& a, const TVIn& b, const double PROX=0.0001  ) 
{ 
	TVector3D av = a.thePosition;
	TVector3D bv = b.thePosition;
	return ( ( (av[2] + PROX ) < bv[2] ) || ( fabs( av[2] - bv[2] ) < PROX && ( av[1] + PROX ) <  bv[1] ) 
		|| ( fabs( av[2] - bv[2] ) < PROX && fabs( av[1] - bv[1] ) < PROX && (av[0] +PROX ) < bv[0] ) ); 
}

void CMainWindow::updateFile()
{
	reader->SetFileName( fileName->text() );
	reader->Update();
	normals3->Update();
	inputMesh = normals3->GetOutput();
	generateMeshFromVTK();
	subdivideMesh();
	transformBackMesh();
	normals4->SetInput( outputMesh );
// 	interactor->Update();
//	renderer->ResetCamera();
}

void CMainWindow::updateDecimation()
{
	QString a = decEdit->text();
	float p = a.toFloat();
	deci->SetTargetReduction(p);
// 	interactor->update();
}

void CMainWindow::updateSmoothing()
{
	QString a = smoothEdit->text();
	int smoothi = a.toInt();
	smooth->SetNumberOfIterations( smoothi );
// 	interactor->Update();
}

void CMainWindow::updateViewRep( )
{
	if ( !wireframeCheck->isChecked() )
	{
		actorMesh->GetProperty()->SetRepresentationToSurface();
// 		interactor->Update();
	}
	else
	{
		actorMesh->GetProperty()->SetRepresentationToWireframe();
// 		interactor->Update();
	}
}

CMainWindow::CMainWindow( const string& filename ) throw()
 : QMainWindow(), inputMesh( 0 ), renderer ( 0 ), outputMesh( 0 )
{
	 QVBox* vPtr = new QVBox( this );	 
   QHBox* rowPtr = new QHBox ( vPtr );
	 QHBox* row2Ptr = new QHBox ( vPtr );
	 row2Ptr->setFixedHeight(30);
	 QHBox* row3Ptr = new QHBox ( vPtr );
	 QHBox* row4Ptr = new QHBox ( vPtr );
	 row3Ptr->setFixedHeight(30);
	 row4Ptr->setFixedHeight(30);
	 this->setCentralWidget(vPtr);
   
   window = new vtkQtRenderWindow(rowPtr);
	 interactor = vtkQtRenderWindowInteractor::New();
	 interactor->SetRenderWindow( window );
	 QVBox* v2Ptr = new QVBox( rowPtr );
	 v2Ptr->setMargin(8);
	 v2Ptr->setFixedSize( 216, 460 );
	 
	 dataFileButton = new QPushButton( "Data file", v2Ptr );
	 dataFileButton->setFixedSize( 200, 25 );
	 lutFileButton = new QPushButton( "Color table", v2Ptr );
	 lutFileButton->setFixedSize( 200, 25 );
	selMax = new QScrollBar( 0, 180, 1, 10, 60, Qt::Horizontal, v2Ptr );
	selMax->setFixedSize( 200, 20 );
	vMax = new QLCDNumber( 3, v2Ptr );
	vMax->setFixedSize( 120, 30 );
	selMin = new QScrollBar( 0, 180, 1, 10, 60, Qt::Horizontal, v2Ptr );
	selMin->setFixedSize( 200, 20 );
	vMin = new QLCDNumber( 3, v2Ptr );
	vMin->setFixedSize( 120, 30 );

/*	 QLabel *decStep = new QLabel("Decimation",row2Ptr);
	 decEdit = new QLineEdit( "0.99", row2Ptr );
	 QLabel *smoothStep = new QLabel("Smoothing iterations",row3Ptr);
	 smoothEdit = new QLineEdit( "20", row3Ptr );*/
/*	 QLabel *fileStep = new QLabel("Source file",row4Ptr);
	 fileName = new QLineEdit( "/data/data/externe/loni/th.vtk", row4Ptr );*/
	 iterateButton = new QPushButton( "Iterate", v2Ptr );
/*   wireframeCheck = new QCheckBox("Wireframe representation",vPtr);*/

 	 connect( dataFileButton, SIGNAL( clicked( ) ),
	 	this, SLOT( loadDataFile() ) );
 	 connect( lutFileButton, SIGNAL( clicked( ) ),
	 	this, SLOT( loadLutFile() ) );
	 
/*	 connect( decEdit, SIGNAL( returnPressed() ),
    this, SLOT( updateDecimation() ) );
	 connect( smoothEdit, SIGNAL( returnPressed() ),
    this, SLOT( updateSmoothing() ) );
	 connect( fileName, SIGNAL( returnPressed() ),
    this, SLOT( updateFile() ) );*/
	 connect( iterateButton, SIGNAL( clicked( ) ),
	 	this, SLOT( iterateModel() ) );
	 /*connect( wireframeCheck, SIGNAL( clicked( ) ),
    this, SLOT( updateViewRep( ) ) );*/
  selX = new QScrollBar( 0, 180, 1, 10, 60, Qt::Horizontal, v2Ptr );
	selX->setFixedSize( 200, 20 );
  vX = new QLCDNumber( 3, v2Ptr );
	vX->setFixedSize( 120, 30 );
  selY = new QScrollBar( 0, 180, 1, 10, 60, Qt::Horizontal, v2Ptr );
	selY->setFixedSize( 200, 20 );
  vY = new QLCDNumber( 3, v2Ptr );
	vY->setFixedSize( 120, 30 );
  selZ = new QScrollBar( 0, 180, 1, 10, 60, Qt::Horizontal, v2Ptr );
	selZ->setFixedSize( 200, 20 );
  vZ = new QLCDNumber( 3, v2Ptr );
	vZ->setFixedSize( 120, 30 );
	
  connect ( selX, SIGNAL( valueChanged( int ) ),
    vX, SLOT( display( int ) ) );
  connect ( selY, SIGNAL( valueChanged( int ) ),
    vY, SLOT( display( int ) ) );
  connect ( selZ, SIGNAL( valueChanged( int ) ),
    vZ, SLOT( display( int ) ) );
  connect ( selMax, SIGNAL( valueChanged( int ) ),
    vMax, SLOT( display( int ) ) );
  connect ( selMin, SIGNAL( valueChanged( int ) ),
    vMin, SLOT( display( int ) ) );
  connect ( selX, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateSliceX( int ) ) );
  connect ( selY, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateSliceY( int ) ) );
  connect ( selZ, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateSliceZ( int ) ) );
  connect ( selMin, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateMin( int ) ) );
  connect ( selMax, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateMax( int ) ) );

	
	// Read dataset and convert into simplified mesh
/*	reader = vtkStructuredPointsReader::New();
	reader->SetFileName("/data/data/externe/loni/nc.vtk");//"/home/belitz/data/nc.vtk");
	reader->Update();
	vtkMarchingCubes *iso = vtkMarchingCubes::New();
	iso->SetInput( (vtkImageData*)reader->GetOutput() );
	iso->SetValue( 0, 1 );
	deci = vtkQuadricDecimation::New();
	deci->SetTargetReduction(0.95);
	deci->SetInput( iso->GetOutput() );
	smooth = vtkWindowedSincPolyDataFilter::New();
	smooth->SetInput( deci->GetOutput() );
	smooth->SetNumberOfIterations( 50 );
	vtkPolyDataNormals *normals1 = vtkPolyDataNormals::New();
	normals1->SetInput( iso->GetOutput() );
	normals1->FlipNormalsOn();
	vtkPolyDataNormals *normals2 = vtkPolyDataNormals::New();
	normals2->SetInput( smooth->GetOutput() );
	normals2->ConsistencyOn();
	//normals2->FlipNormalsOn();
	normals2->SplittingOff();*/
// 	vtkSphereSource *sphere = vtkSphereSource::New();
// 	sphere->SetRadius( 30 );
// 	sphere->SetCenter( 64.0, 64.0, 64.0 );
// 	sphere->SetThetaResolution(25);
// 	sphere->SetPhiResolution(25);

// Read dataset and convert into simplified mesh
	
	//cerr << "Loading mask... ";
	//TDataFile theData = getFileServer().loadDataSet( "EMIL/wtrsl/mrmauswt1_rsl_mask.hdr" );
/*	cerr << "done" << endl;
	cerr << "Starting conversion... ";
	cerr << "Read " << theData.first->getExtent(0) << " x " <<
		theData.first->getExtent(1) << " x " << theData.first->getExtent(2) << endl << theData.first->getType().name() << endl;
	// Create vtk structured points structure
  vtkImageData* sp = vtkImageData::New();
  sp->SetDimensions( theData.first->getExtent(0), theData.first->getExtent(1), theData.first->getExtent(2) );
  sp->AllocateScalars();	
  // Assign dataset to structured points
  vtkPointData *p=sp->GetPointData();
	vtkShortArray* sArray = NULL;
	vtkDoubleArray* fArray = NULL;*/
	
/*	if ( theData.first->getType() == typeid( double ) )
	{
		TFieldPtr floatSet = static_pointer_cast<TField>( theData.first );
		fArray = vtkDoubleArray::New();
		fArray->SetArray(floatSet->getArray(),
			theData.first->getExtent(0)*theData.first->getExtent(1)*theData.first->getExtent(2), 1 );
		p->SetScalars( fArray );
		sp->SetScalarTypeToDouble();
	}
	else 
	{
		
		if ( theData.first->getType() == typeid( short ) )
		{
			TImagePtr shortSet = static_pointer_cast<TImage>( theData.first );
			cerr << "Here" << endl;
			short* sA = new short[theData.first->getExtent(0)*theData.first->getExtent(1)*theData.first->getExtent(2)];
			short* sAs = sA;
			for( TImage::iterator it = shortSet->begin(); it != shortSet->end(); ++it, ++sAs )
				*sAs = *it;
			sArray = vtkShortArray::New();
			sArray->SetArray( sA,
				theData.first->getExtent(0)*theData.first->getExtent(1)*theData.first->getExtent(2), 1 );
			p->SetScalars( sArray );
			sp->SetScalarTypeToShort();
			cerr << "There" << endl;
		}		
		else
		{
	    sp->Delete();		
			cerr << "CVTkHandler - Unknown image format in dataset. Image was not saved" << endl;
			exit(-1);
		}
	}
	sp->SetSpacing( 1.0, 1.0, 1.0 );
	sp->SetOrigin( static_cast<double>( theData.first->getExtent(0) )/ 2.0, 
		static_cast<double>( theData.first->getExtent(1) )/ 2.0,
		static_cast<double>( theData.first->getExtent(2) )/ 2.0	);
	sp->UpdateData();*/
	
/*	vtkMarchingCubes *iso = vtkMarchingCubes::New(); */
/*	vtkGenericContourFilter *iso = vtkContourFilter::New();
	iso->SetInput( sp );
	iso->SetValue( 0, 1 );*/
/*	iso->ComputeNormalsOff();
	iso->ComputeGradientsOff();*/
/* 	deci = vtkQuadricDecimation::New();
 	deci->SetTargetReduction(0.95 );
 	deci->SetInput( iso->GetOutput() );
 	smooth = vtkWindowedSincPolyDataFilter::New();
 	smooth->SetInput( deci->GetOutput() );
 	smooth->SetNumberOfIterations( 50 );*/
// 	vtkPolyDataNormals *normals1 = vtkPolyDataNormals::New();
// 	normals1->SetInput( iso->GetOutput() );
// 	normals1->FlipNormalsOn();
// 	vtkPolyDataNormals *normals2 = vtkPolyDataNormals::New();
// 	normals2->SetInput( smooth->GetOutput() );
// 	normals2->ConsistencyOn();

	// Get VTK mesh, convert into internal data and remesh
 	//inputMesh = normals3->GetOutput();
	//outputMesh = normals3->GetOutput();
 	//generateMeshFromVTK();
 	generateMeshFromFile( "/data/data/EMILSegmentierungen/neu/wtrsl/mrmauswt1_rsl_mask.hdr" );
 	cerr << "A" << endl;
//	subdivideMesh();
// 	
// 	// Do the backconversion and generate normals
 	deci = vtkQuadricDecimation::New();
 	deci->SetTargetReduction(0.98 );
 	deci->SetInput( outputMesh );
 	smooth = vtkWindowedSincPolyDataFilter::New();
 	smooth->SetInput( deci->GetOutput() );
 	smooth->SetNumberOfIterations( 10 );
 	smooth->Update();
 	cerr << "A" << endl;
  inputMesh = smooth->GetOutput();
	generateMeshFromVTK();
 	cerr << "B" << endl;
 	transformBackMesh();
	cerr << "C" << endl;
	normals3 = vtkPolyDataNormals::New();
	normals3->SetInput( outputMesh );
	//normals3->SetInput( sphere->GetOutput() );
	normals3->ConsistencyOn();
	normals3->FlipNormalsOn();
	normals3->SplittingOff();
	normals3->Update();
	inputMesh = normals3->GetOutput();
	generateMeshFromVTK(true);
	
	normals4 = vtkPolyDataNormals::New();
	normals4->SetInput( normals3->GetOutput() );
	normals4->ConsistencyOn();
	normals4->SplittingOff();
	normals4->FlipNormalsOn();
	
	dataColors.loadLookupTable();
	dataColors.setLowerClamp( 50.0 );
	dataColors.setUpperClamp( 200.0 );
	meshColors.loadLookupTable();
	meshColors.setLowerClamp( 0.0 );
	meshColors.setUpperClamp( 255.0 );
/*	vtkLookupTable* table = meshColors.getLookupTable();
	for( uint i = 0; i < 256; ++i )
	{
		double rgba[4];
		table->GetTableValue( i, rgba );
		rgba[3] = 1.0 - ( 0.75 * ( static_cast<double>(i) / 255.0 ) );
		table->SetTableValue( i, rgba );
	}*/
	//imageData.loadDataSet( "/data/data/phantoms/3dphantom.dat" );
	imageData.loadDataSet( "/data/data/externe/joaquin/original/emil/data1/uint_mrmauswt1_rsl.hdr" );
	forceField = imageData.computeExternalForces();
	selX->setRange( 0, imageData.getExtent( 2 ) );
	selX->setValue( 64 );
	selY->setRange( 0, imageData.getExtent( 1 ) );
	selY->setValue( 64 );
	selZ->setRange( 0, imageData.getExtent( 0 ) );
	selZ->setValue( 64 );
	int imageMax = static_cast<int>(imageData.getMaximum());
	int imageMin = static_cast<int>(imageData.getMinimum());
	selMax->setRange( imageMin, imageMax );
	selMax->setValue( imageMax );
	selMin->setRange( imageMin, imageMax );
	selMin->setValue( imageMin );
	
	// Map mesh data
	vtkPolyDataMapper *mapperMeshData = vtkPolyDataMapper::New();
	mapperMeshData->SetInput( outputMesh );
	mapperMeshData->SetLookupTable( meshColors.getLookupTable() );
//	mapperMeshData->SetScalarMaterialModeToAmbient();
	mapperMeshData->UseLookupTableScalarRangeOn();
	actorMesh = vtkActor::New();
	actorMesh->SetMapper( mapperMeshData );
	// Map mesh normal data
	vtkPolyDataMapper *mapperMeshNormalData = vtkPolyDataMapper::New();
	vtkMaskPoints *ptMask = vtkMaskPoints::New();
	ptMask->SetInput( normals4->GetOutput() );
	ptMask->SetOnRatio( 1 );
	vtkConeSource *cone = vtkConeSource::New();
	cone->SetResolution(4);
	vtkTransform *trans = vtkTransform::New();
	trans->Translate( 0.5, 0.0, 0.0 );
	trans->Scale( 1.0, 0.25, 0.25 );
	vtkTransformPolyDataFilter* tpdf = vtkTransformPolyDataFilter::New();
	tpdf->SetInput( cone->GetOutput() );
	tpdf->SetTransform( trans );
	vtkGlyph3D *glyph = vtkGlyph3D::New();
	glyph->SetInput( ptMask->GetOutput() );
	glyph->SetSource( tpdf->GetOutput() );
	glyph->SetVectorModeToUseNormal();
	glyph->SetScaleModeToScaleByVector();
	glyph->SetScaleFactor( 2.0 );
	mapperMeshNormalData->SetInput( glyph->GetOutput() );
	actorMeshNormal = vtkActor::New();
 	actorMeshNormal->SetMapper( mapperMeshNormalData );
 	actorMeshNormal->GetProperty()->SetColor( 0.9, 0.2, 0.2 );

	// Map orthoslices
		
	cerr << "Init renderer" << endl;
	renderer = vtkRenderer::New();
	window->AddRenderer( renderer );
	renderer->TwoSidedLightingOn();
	renderer->SetAmbient( 1.0, 1.0, 1.0 );
	//axesActor->GetProperty()->SetColor( 1.0, 1.0, 1.0 );
/*	axesActor->GetProperty()->SetAmbient( 0.5 );
	axesActor->GetProperty()->SetDiffuse( 1.0 );
	axesActor->GetProperty()->BackfaceCullingOff();
	axesActor->GetProperty()->FrontfaceCullingOff();*/
	cerr << "1" << endl;
	renderer->AddActor( meshColors.getColorBar() );
	cerr << "2" << endl;
	imageData.setColorTable( dataColors.getLookupTable() );
	cerr << "3" << endl;
	imageData.setSlicePosition( 0, 90 );
	imageData.setSlicePosition( 1, 90 );
	imageData.setSlicePosition( 2, 90 );
/*	actorMesh->GetProperty()->SetColor( 1.0, 1.0, 1.0 );
	actorMesh->GetProperty()->SetAmbient( 1.0 );*/
	renderer->AddActor( actorMesh );
/*	actorMeshNormal->GetProperty()->SetColor( 1.0, 1.0, 1.0 );
	actorMeshNormal->GetProperty()->SetAmbient( 1.0 );*/
	renderer->AddActor( actorMeshNormal );
// 	actorSliceX->GetProperty()->SetColor( 1.0, 1.0, 1.0 );	
	renderer->AddActor( imageData.getSlice( 0 ) );
// 	actorSliceY->GetProperty()->SetColor( 1.0, 1.0, 1.0 );	
	renderer->AddActor( imageData.getSlice( 1 ) );
// 	actorSliceZ->GetProperty()->SetColor( 1.0, 1.0, 1.0 );
	renderer->AddActor( imageData.getSlice( 2 ) );
	renderer->ResetCamera();
cerr << "Bye" << endl;
}


CMainWindow::~CMainWindow() throw()
{
}

void CMainWindow::transformBackMesh()
{
// 	cerr << "Backtransformation (HalfEdge -> VTK)... ";
	if ( outputMesh != NULL ) 
		outputMesh->Reset();
	else
		outputMesh = vtkPolyData::New();
 	vtkCellArray *meshpolys = vtkCellArray::New();
	vtkPoints *meshpts = vtkPoints::New();
	
	ulong i = 0;
	for( list<SVertex*>::iterator it = work.vList.begin(); it != work.vList.end(); ++it, ++i )
	{
		(*it)->ulID = i;
	}
	vtkDoubleArray* pointForces = vtkDoubleArray::New();
	
	meshpts->SetNumberOfPoints( i );
	for( list<SVertex*>::iterator it = work.vList.begin(); it != work.vList.end(); ++it )
	{
		double v[3];
		v[0] = (*it)->thePosition[0];
		v[1] = (*it)->thePosition[1];
		v[2] = (*it)->thePosition[2];
		meshpts->SetPoint( (*it)->ulID, v );
		pointForces->InsertTuple1( (*it)->ulID, norm((*it)->theForce)*255.0 );
	}
	
	list<SFace*>::iterator ot = work.fList.begin();
	vtkIdType pa[100];
	vtkIdType* p = pa;
	

	for( uint i = 0; i < work.fList.size(); ++i )
	{
		p[0] = (*ot)->anEdgePtr->endPointPtr->ulID;
		p[2] = (*ot)->anEdgePtr->nextEdgePtr->endPointPtr->ulID;
		p[1] = (*ot)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->ulID;
		++ot;
		meshpolys->InsertNextCell( 3, p );
	}	
	outputMesh->SetPolys( meshpolys );
	outputMesh->SetPoints( meshpts );
	vtkPointData* pd = outputMesh->GetPointData();
	pd->SetScalars( pointForces );
	outputMesh->BuildCells();
	outputMesh->BuildLinks();
	meshpolys->Delete();
	meshpts->Delete();
	pointForces->Delete();
// cerr << "done" << endl;	
}

void CMainWindow::subdivideMesh()
{
	work.checkTopology();	
	work.subdivide(12.0);
	work.checkTopology();
	work.edgeMelt(4.0);
	work.checkTopology();
}

void CMainWindow::generateMeshFromFile( std::string filename )
{	
	//work.reset();

	cerr << "Loading mask... ";
	TDataFile theData = getFileServer().loadDataSet( filename );
	cerr << "ok" << endl;
	TImagePtr img = static_pointer_cast<TImage>( theData.first );
	for( TImage::iterator it = img->begin();
		it != img->end(); ++it )
	{			
			if ( *it == 1 ) *it = 10;
	};	
	cerr << "Filtering... ";
 	shared_ptr<CPipelineItem> lp( new CLowPassFilter( 0 ) );
	CTypedMap* params = lp->getParameters();
	params->setUnsignedLong( "Radius", 3 );
	lp->setInput( img );
	lp->apply();	
	cerr << "ok" << endl;
	CTypedData<int> intimg( theData.first->getDimension(), theData.first->getExtents() );
	CTypedData<int>::iterator ot = intimg.begin();
 	img = static_pointer_cast<TImage>( lp->getOutput() );
	for( TImage::iterator it = img->begin();
		it != img->end(); ++it, ++ot )
	{
			
			if ( *it > 5 ) *ot = 1;
			else *ot = 0;
	};
	CIsoSurface iso( img->getExtent(0), img->getExtent(1), img->getExtent(2), intimg.getArray() );
	std::vector<TVector3D> vertices;
	std::vector<TriangleKey> tris;
	iso.ExtractContour (0.5, vertices, tris);
	cerr << "After extraction" << vertices.size() << " / " << tris.size() << endl;
	iso.MakeUnique( vertices, tris );
	cerr << "After unification" << vertices.size() << " / " << tris.size() << endl;
	iso.OrientTriangles( vertices, tris, false );
	cerr << "After orientation" << vertices.size() << " / " << tris.size() << endl;
	vector<SVertex*> vVector;
	for( std::vector<TVector3D>::iterator it = vertices.begin(); it != vertices.end(); ++it )
	{
		SVertex* v = new SVertex( *it );
		vVector.push_back( v );
	}

	list<TFIn> FL;
	cerr << "Starting mesh consists of " << vVector.size() << " vertices";
	// Now we need to sort the vectors according to their position. Lets hope to find some double vertices ....
	int res = 1;
	for( uint i = 0; i < tris.size(); ++i )
	{
		TFIn f;
		f.vts[0] = tris[i].V[0];
		f.vts[1] = tris[i].V[1];
		f.vts[2] = tris[i].V[2];
		FL.push_back(f);
	}	
	cerr << " " << endl << "Generated vertex lists " << vVector.size() << endl;
	cerr << "Resulting mesh consists of " << work.fList.size() << " faces and " << work.vList.size() << " vertices" << endl;
	if ( outputMesh != NULL ) 
		outputMesh->Reset();
	else
		outputMesh = vtkPolyData::New();
	cerr << "Converting Mesh to VTK" << endl;
 	vtkCellArray *meshpolys = vtkCellArray::New();
	vtkPoints *meshpts = vtkPoints::New();
	
	ulong i = 0;
	vtkDoubleArray* pointForces = vtkDoubleArray::New();
	
	meshpts->SetNumberOfPoints( vertices.size() );	
	for( vector<TVector3D>::iterator it = vertices.begin(); it != vertices.end(); ++it,++i )
	{
		double v[3];
		v[0] = (*it)[0];
		v[1] = (*it)[2];
		v[2] = (*it)[1];
		meshpts->SetPoint( i, v );
		pointForces->InsertTuple1( i, 128.0 );		
	}
	
	vtkIdType pa[100];
	vtkIdType* p = pa;
	
	for( uint i = 0; i < tris.size(); ++i )
	{
		p[0] = tris[i].V[0];
		p[2] = tris[i].V[2];
		p[1] = tris[i].V[1];
		meshpolys->InsertNextCell( 3, p );
	}	
	
	cerr << "Clearing up the mess" << endl;
	
	outputMesh->SetPolys( meshpolys );
	cerr << "1" << endl;
	outputMesh->SetPoints( meshpts );
	cerr << "2" << endl;
	vtkPointData* pd = outputMesh->GetPointData();
	cerr << "3" << endl;
	pd->SetScalars( pointForces );
	cerr << "4" << endl;
	outputMesh->BuildCells();
	cerr << "5" << endl;
	outputMesh->BuildLinks();
 	cerr << "6" << endl;
 	meshpolys->Delete();
 	cerr << "7" << endl;
 	meshpts->Delete();
 	cerr << "8" << endl;
 	pointForces->Delete();
 	cerr << "9" << endl;
}



void CMainWindow::generateMeshFromVTK( bool flip )
{	
	work.reset();
	
	vtkCellArray *polys = inputMesh->GetPolys();
	vtkPoints *pts = inputMesh->GetPoints();
		
	vector<TVIn> VV;
	list<TFIn> FL;
	for( int i = 0; i < pts->GetNumberOfPoints(); ++i )
	{
		TVIn v;		
		double pointPosition[3];
		pts->GetPoint( i, pointPosition );
		v.thePosition[0] = pointPosition[0];		
		v.thePosition[1] = pointPosition[1];
		v.thePosition[2] = pointPosition[2];
		if ( !flip )
			swap( v.thePosition[1], v.thePosition[2] );
		v.ulID = i;
		VV.push_back(v);		
	}
	cerr << "Starting mesh consists of " << VV.size() << " vertices";
	// Now we need to sort the vectors according to their position. Lets hope to find some double vertices ....
	std::sort( VV.begin(), VV.end(), vecLess );
	std::map<ulong,ulong> vertexMap; // Maps the indices read to vertex array indices. Double vertices will be removed
	for( uint i = 0; i < VV.size(); ++i )
	{
		// Find the end of the current block
		uint begin = i;
		uint end = i;
		while( end < (VV.size()-1) && !vecLess(VV[end],VV[end+1]) ) 
			end++;
		for( uint j = begin; j < end+1; j++ )
		{
			vertexMap[VV[j].ulID] = begin;
			if ( j != begin ) VV[j].del = true; else
			VV[j].del = false;
		}
		i = end;
	}
	vtkIdType pa[100];
	vtkIdType* p = pa;
	vtkIdType np;
	polys->InitTraversal();

	int res = 1;
	while( res )
	{
		res = polys->GetNextCell( np, p );
		if ( res )
		{
			TFIn f;	
			f.vts[0] = p[0];
			f.vts[1] = p[1];
			f.vts[2] = p[2];
			FL.push_back(f);
		}
	}
	// All data was read. Now build the vertex lists
	vector<SVertex*> vVector;	
	int ctn = 0; cerr << endl;
	for( vector<TVIn>::iterator it = VV.begin(); it != VV.end(); ++it )
	{
			SVertex* v = new SVertex( it->thePosition );
			if ( it->del == false ) 
			{
				v->ulID = vid; ++vid;
				work.vList.push_back( v );
			}
			vVector.push_back( v );
			switch( ctn )
			{
				case 0: fprintf(stderr, "-\b"); break;
				case 1: fprintf(stderr, "/\b"); break;
				case 2: fprintf(stderr, "|\b"); break;
				case 3: fprintf(stderr, "\\\b"); break;
			}
			ctn++; if (ctn == 4) ctn = 0;
	}
	cerr << " " << endl << "Generated vertex lists" << endl;
	// Generate triangles
	list<Pair> pairList;
	for( list<TFIn>::iterator it = FL.begin(); it != FL.end(); ++it )
	{
		SFace* f = new SFace;
		SVertex* p1 = vVector[ vertexMap[it->vts[0]] ];
		SVertex* p2 = vVector[ vertexMap[it->vts[1]] ];
		SVertex* p3 = vVector[ vertexMap[it->vts[2]] ];
		SEdge* e1 = new SEdge( p2, f );
		SEdge* e2 = new SEdge( p3, f );
		SEdge* e3 = new SEdge( p1, f );
		e1->nextEdgePtr = e2;
		e2->nextEdgePtr = e3;
		e3->nextEdgePtr = e1;

		work.eList.push_back( e1 );
		work.eList.push_back( e2 );
		work.eList.push_back( e3 );
		p1->anEdgePtr = e1;
		p2->anEdgePtr = e2;
		p3->anEdgePtr = e3;
		f->anEdgePtr = e1;
		work.fList.push_back(f);
		Pair p;
		p.ulID1 = vertexMap[it->vts[0]];
		p.ulID2 = vertexMap[it->vts[1]];
		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
		p.anEdgePtr = e1;
		pairList.push_back( p );
		p.ulID1 = vertexMap[it->vts[1]];
		p.ulID2 = vertexMap[it->vts[2]];
		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
		p.anEdgePtr = e2;
		pairList.push_back( p );
		p.ulID1 = vertexMap[it->vts[2]];
		p.ulID2 = vertexMap[it->vts[0]];
		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
		p.anEdgePtr = e3;
		pairList.push_back( p );		
	}
	pairList.sort( pairLess );
	// Rebuild topology
	list<Pair>::iterator it = pairList.begin();
	while( it != pairList.end() )
	{
		Pair p = *it; ++it;
		if ( it->ulID1 == p.ulID1 && it->ulID2 == p.ulID2 )
		{
			p.anEdgePtr->opposingEdgePtr = it->anEdgePtr;
			it->anEdgePtr->opposingEdgePtr = p.anEdgePtr;
			if ( p.anEdgePtr->opposingEdgePtr->theFacePtr == it->anEdgePtr->opposingEdgePtr->theFacePtr )
			{
				cerr << "Faces are identical!" << endl;
				exit(-1);
			}
			++it;
		}
		else 
		{
			cout << "WARN: Pair " << p.ulID1 << "/" << p.ulID2 << " has no partner" << endl;
			cout << "Next is " << it->ulID1 << "/" << it->ulID2 << endl;
			p.anEdgePtr->opposingEdgePtr = p.anEdgePtr;
		}
	}	
	cerr << "Resulting mesh consists of " << work.fList.size() << " faces and " << work.vList.size() << " vertices" << endl;
}

void CMainWindow::generateSimpleMesh()
{	
	work.reset();
	
	cout << "Opening file" << endl;
	ifstream file( "/home/belitz/tetrahedron.pd" );
	vector<TVIn> VV;
	list<TFIn> FL;
	char buff[200]; int read=200;
	file.getline(buff,read);
	cout << buff << endl;
//	int i;
	string ctxt; int vts; int fcs;
	file >> ctxt >> vts;
	cout << ctxt << ": " << vts << endl;
	file >> ctxt >> fcs;
	cout << ctxt << ": " << fcs << endl;
	for( int i = 0; i < vts; ++i )
	{
		TVIn v;
		file >> v.ulID >> v.thePosition[0] >> v.thePosition[1] >> v.thePosition[2];
		v.ulID = i;
		VV.push_back(v);
		//cout << v.ulID << ": " << v.thePosition[0] << " " << v.pos[1] << " " << v.pos[2] << endl;
	}
	// Now we need to sort the vectors according to their position. Lets hope to find some double vertices ....
	std::sort( VV.begin(), VV.end(), vecLess );
	std::map<ulong,ulong> vertexMap; // Maps the indices read to vertex array indices. Double vertices will be removed
	ulong rem = 0;
	for( uint i = 0; i < VV.size(); ++i )
	{
		// Find the end of the current block
		uint begin = i;
		uint end = i;
		while( end < (VV.size()-1) && !vecLess(VV[end],VV[end+1]) ) 
			end++;
		for( uint j = begin; j < end+1; j++ )
		{
			vertexMap[VV[j].ulID] = begin;
			if ( j != begin ) {VV[j].del = true; ++rem;}
			else VV[j].del = false;
		}
		i = end;
	}
	cerr << "Marked " << rem << " vertices for removal" << endl;
	for( int i = 0; i < fcs; ++i )
	{
		TFIn f;
		file >> f.vts[0] >> f.vts[1] >> f.vts[2];
		FL.push_back(f);
	}	
	file.close();
	cerr << "File read" << endl;
	// All data was read. Now build the vertex lists
	vector<SVertex*> vVector;
	for( vector<TVIn>::iterator it = VV.begin(); it != VV.end(); ++it )
	{
			SVertex* v = new SVertex( it->thePosition );
			v->ulID = it->ulID; 
			if ( vid < it->ulID ) 
				vid = it->ulID;
			if ( it->del == false ) 
				work.vList.push_back( v );
			vVector.push_back( v );
			cout << ".";
	}
	cerr << endl << "Generated vertex lists" << endl;
	// Generate triangles
	list<Pair> pairList;
	for( list<TFIn>::iterator it = FL.begin(); it != FL.end(); ++it )
	{
		SFace* f = new SFace;
		SVertex* p1 = vVector[ vertexMap[it->vts[0]] ];
		SVertex* p2 = vVector[ vertexMap[it->vts[1]] ];
		SVertex* p3 = vVector[ vertexMap[it->vts[2]] ];
		SEdge* e1 = new SEdge( p2, f );
		SEdge* e2 = new SEdge( p3, f );
		SEdge* e3 = new SEdge( p1, f );
		e1->nextEdgePtr = e2;
		e2->nextEdgePtr = e3;
		e3->nextEdgePtr = e1;
		work.eList.push_back( e1 );
		work.eList.push_back( e2 );
		work.eList.push_back( e3 );
		p1->anEdgePtr = e1;
		p2->anEdgePtr = e2;
		p3->anEdgePtr = e3;
		f->anEdgePtr = e1;
		work.fList.push_back(f);
		Pair p;
		p.ulID1 = vertexMap[it->vts[0]];
		p.ulID2 = vertexMap[it->vts[1]];
		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
		p.anEdgePtr = e1;
		pairList.push_back( p );
		p.ulID1 = vertexMap[it->vts[1]];
		p.ulID2 = vertexMap[it->vts[2]];
		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
		p.anEdgePtr = e2;
		pairList.push_back( p );
		p.ulID1 = vertexMap[it->vts[2]];
		p.ulID2 = vertexMap[it->vts[0]];
		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
		p.anEdgePtr = e3;
		pairList.push_back( p );		
	}
	pairList.sort( pairLess );
	// Rebuild topology
	list<Pair>::iterator it = pairList.begin();
	while( it != pairList.end() )
	{
		Pair p = *it; ++it;
		if ( it->ulID1 == p.ulID1 && it->ulID2 == p.ulID2 )
		{
			p.anEdgePtr->opposingEdgePtr = it->anEdgePtr;
			it->anEdgePtr->opposingEdgePtr = p.anEdgePtr;
			++it;
		}
		else 
		{
			cout << "WARN: Pair " << p.ulID1 << "/" << p.ulID2 << " has no partner" << endl;
			cout << "Next is " << it->ulID1 << "/" << it->ulID2 << endl;
			p.anEdgePtr->opposingEdgePtr = p.anEdgePtr;
		}
	}	
 	cerr << "Rebuild topology" << endl;
	vid++;
 	cerr << "Resulting mesh consists of " << work.fList.size() << " faces and " << work.vList.size() << " vertices" << endl;
}

void CMainWindow::iterateModel()
{
	theModel.setExternalForceField( forceField );
	theModel.setMesh( &work );
	theModel.attachObserver( this, 0 );
	theModel.iterate();
	actorMesh->GetProperty()->SetRepresentationToWireframe();
	normals4->FlipNormalsOff();
	renderer->RemoveActor( actorMeshNormal );
	vtkLookupTable* table = meshColors.getLookupTable();
	for( uint i = 0; i < 256; ++i )
	{
		double rgba[4];
		table->GetTableValue( i, rgba );
		rgba[0] = 1.0;
		rgba[1] = 1.0;
		rgba[2] = 1.0;
		rgba[3] = 1.0;
		table->SetTableValue( i, rgba );
	}	
}

void CMainWindow::loadDataFile()
{
	QString fn = QFileDialog::getOpenFileName( "/data/data", "*", this, "File loader", "Select data file to load" );
	imageData.loadDataSet( fn.ascii() );
	selX->setRange( 0, imageData.getExtent( 2 ) );
	selX->setValue( 90 );	
	selY->setRange( 0, imageData.getExtent( 1 ) );
	selY->setValue( 90 );	
	selZ->setRange( 0, imageData.getExtent( 0 ) );
	selZ->setValue( 90 );	
	imageData.setSlicePosition( 0, 90 );
	imageData.setSlicePosition( 1, 90 );
	imageData.setSlicePosition( 2, 90 );

	dataFileButton->setText( fn );
	dataColors.setLowerClamp( imageData.getMinimum() );
	dataColors.setUpperClamp( imageData.getMaximum() );
	selMax->setRange( static_cast<int>(imageData.getMinimum()), static_cast<int>(imageData.getMaximum()) );
	selMax->setValue( static_cast<int>(imageData.getMaximum()) );	
	selMin->setRange( static_cast<int>(imageData.getMinimum()), static_cast<int>(imageData.getMaximum()) );
	selMin->setValue( static_cast<int>(imageData.getMinimum()) );		
}

void CMainWindow::loadLutFile()
{
	QString fn = QFileDialog::getOpenFileName( "/home/belitz/bin/mricro", "*.lut", this, "File loader", "Select a color table to load" );
	dataColors.loadLookupTable( fn.ascii() );
	lutFileButton->setText( fn );
}

void CMainWindow::execute( boost::shared_ptr<CEvent> anEvent )
{
	if ( anEvent->getType() == 0 )
	{
		transformBackMesh();
		normals4->SetInput( outputMesh );
		interactor->Render();
	}
	qApp->processEvents( 2 );
}
