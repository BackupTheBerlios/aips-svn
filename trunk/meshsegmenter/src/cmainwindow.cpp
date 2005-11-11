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
//#include <vtkPowerCrustSurfaceReconstruction.h>
//#include "MeshCheckerT.hh"
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
// 
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

void CMainWindow::initUI()
{
	/* BEGIN UI stuff */
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
	iterateButton = new QPushButton( "Iterate", v2Ptr );

	connect( dataFileButton, SIGNAL( clicked( ) ),
		this, SLOT( loadDataFile() ) );
 	connect( lutFileButton, SIGNAL( clicked( ) ),
	 	this, SLOT( loadLutFile() ) );
	connect( iterateButton, SIGNAL( clicked( ) ),
	 	this, SLOT( iterateModel() ) );
  
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

	/* END UI stuff */
}

void CMainWindow::initMesh()
{
	// Load a binary mask and generate an initial mesh from it

 	generateMeshFromFile( "/data/data/EMILSegmentierungen/neu/wtrsl/mrmauswt1_rsl_mask.hdr" );
 	//generateSimpleMesh();

cerr << "Mask loaded" << endl;

	// Refine the initial mesh
 	deci = vtkQuadricDecimation::New();
 	deci->SetTargetReduction(0.98 );
 	deci->SetInput( outputMesh );
 	
 	smooth = vtkWindowedSincPolyDataFilter::New();
 	smooth->SetInput( deci->GetOutput() );
 	smooth->SetNumberOfIterations( 10 );
 	smooth->Update();
 	
  inputMesh = smooth->GetOutput();
  
  // Produce an internal representation of the smoothed mesh
	generateMeshFromVTK();
	// Return the smoothed mesh for display
 	transformBackMesh();
	
	// Generate normals
	normals3 = vtkPolyDataNormals::New();
	normals3->SetInput( outputMesh );	
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
}

void CMainWindow::initVis()
{
// Setup color tables for volume and mesh
	dataColors.loadLookupTable();
	dataColors.setLowerClamp( 50.0 );
	dataColors.setUpperClamp( 200.0 );
	meshColors.loadLookupTable();
	meshColors.setLowerClamp( 0.0 );
	meshColors.setUpperClamp( 255.0 );
	
	// Load the image
	imageData.loadDataSet( "/data/data/externe/joaquin/original/emil/data1/uint_mrmauswt1_rsl.hdr" );
	//imageData.loadDataSet( "/data/data/externe/mni/t1.hdr" );
	
	// Load the GVF field
	forceField = imageData.computeExternalForces();
	
	// Set sliders to image extent and value ranges
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
	mapperMeshData = vtkPolyDataMapper::New();
	mapperMeshData->SetInput( outputMesh );
	mapperMeshData->SetLookupTable( meshColors.getLookupTable() );
	mapperMeshData->UseLookupTableScalarRangeOn();
	actorMesh = vtkActor::New();
	actorMesh->SetMapper( mapperMeshData );
	
	// Map mesh normal data
	mapperMeshNormalData = vtkPolyDataMapper::New();
	ptMask = vtkMaskPoints::New();
	ptMask->SetInput( normals4->GetOutput() );
	ptMask->SetOnRatio( 1 );
	cone = vtkConeSource::New();
	cone->SetResolution(4);
	trans = vtkTransform::New();
	trans->Translate( 0.5, 0.0, 0.0 );
	trans->Scale( 1.0, 0.25, 0.25 );
	tpdf = vtkTransformPolyDataFilter::New();
	tpdf->SetInput( cone->GetOutput() );
	tpdf->SetTransform( trans );
	glyph = vtkGlyph3D::New();
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
	renderer->AddActor( meshColors.getColorBar() );
	imageData.setColorTable( dataColors.getLookupTable() );
	imageData.setSlicePosition( 0, 90 );
	imageData.setSlicePosition( 1, 90 );
	imageData.setSlicePosition( 2, 90 );
	
	// Add all actors to the renderer
	renderer->AddActor( actorMesh );
	renderer->AddActor( actorMeshNormal );
	renderer->AddActor( imageData.getSlice( 0 ) );
	renderer->AddActor( imageData.getSlice( 1 ) );
	renderer->AddActor( imageData.getSlice( 2 ) );
	renderer->ResetCamera();
}	

CMainWindow::CMainWindow( const string& filename ) throw()
 : QMainWindow(), renderer ( 0 ), inputMesh( 0 ), outputMesh( 0 )
{
cerr << "Init UI components... ";
	initUI();
cerr << "done" << endl <<	"Loading/generating initial mesh... ";
	initMesh();
cerr << "done" << endl <<	"Initialisiation of visualization componenets... ";
	initVis();
cerr << "done" << endl << "Initialisiation completed... time to work" << endl;
}


CMainWindow::~CMainWindow() throw()
{
	renderer->Delete();
	actorMeshNormal->Delete();
	actorMesh->Delete();
	axesActor->Delete();
	coordSystem->Delete();	
	glyph->Delete();
	tpdf->Delete();
	cone->Delete();
	trans->Delete();
	ptMask->Delete();
	mapperMeshNormalData->Delete();
	mapperMeshData->Delete();
	normals4->Delete();
	normals3->Delete();
	smooth->Delete();
	deci->Delete();
	inputMesh->Delete();
	outputMesh->Delete();
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
/*	if ( outputMesh != NULL ) 
		outputMesh->Reset();
	else
		outputMesh = vtkPolyData::New();
 	vtkCellArray *meshpolys = vtkCellArray::New();
	vtkPoints *meshpts = vtkPoints::New();
	
	TMesh::VertexIter v_it, v_end( work.vertices_end() );
	ulong i = 0;
	for( v_it = work.vertices_begin(); v_it != v_end; ++v_it, ++i )
	{
		v_it->ulID = i;
	}
	vtkDoubleArray* pointForces = vtkDoubleArray::New();	
	meshpts->SetNumberOfPoints( i );
	for( v_it = work.vertices_begin(); v_it != v_end; ++v_it )
	{
		double v[3];
		TMesh::Point p = work.point( v_it );
		v[0] = p[0];
		v[1] = p[1];
		v[2] = p[2];
		meshpts->SetPoint( v_it->ulID, v );
		pointForces->InsertTuple1( v_it->ulID, norm(v_it->theForce)*255.0 );
	}		
	vtkIdType pa[100];
	vtkIdType* p = pa;
	TMesh::FaceIter f_it, f_end = work.faces_end();	
	for( f_it = work.faces_begin(); f_it != f_end; ++f_it )
	{
		uint index[] = {0,2,1};
		uint i = 0;
		for( TMesh::FaceVertexIter fv_it = work.fv_iter( f_it.handle() ); fv_it; ++fv_it, ++i )
		{
			p[index[i]] = fv_it->ulID;
		}
		meshpolys->InsertNextCell( 3, p );
	}*/
	
/*	outputMesh->SetPolys( meshpolys );
	outputMesh->SetPoints( meshpts );
	vtkPointData* pd = outputMesh->GetPointData();
	pd->SetScalars( pointForces );
	outputMesh->BuildCells();
	outputMesh->BuildLinks();
	meshpolys->Delete();
	meshpts->Delete();
	pointForces->Delete();*/
// cerr << "done" << endl;	
}

void CMainWindow::subdivideMesh()
{
	work.checkTopology( );	
	work.subdivide( 12.0);
	work.checkTopology( );
	work.edgeMelt( 4.0);
	work.checkTopology( );
// 	work.checkTopology( work );	
// 	work.subdivide( work, 12.0);
// 	work.checkTopology( work );
// 	work.edgeMelt( work, 4.0);
// 	work.checkTopology( work );
}

void CMainWindow::generateMeshFromFile( std::string filename )
{	
	//work.reset();

	cerr << "Loading mask... ";
	TDataFile data = getFileServer().loadDataSet( filename );
	cerr << "ok" << endl;
	TImagePtr img ( new TImage( *(static_pointer_cast<TImage>(data.first) ) ) );
// 	data.first.reset();
// 	data.second.reset();
	cerr << "img" << img.use_count() << endl;
// 	img.reset();
// 	cerr << "img" << img.use_count() << endl;
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
	lp->update();	
	TImage* image = (static_cast<TImage*>( lp->getOutput().get() ));
	CTypedData<int> intimg( image->getDimension(), image->getExtents() );
	CTypedData<int>::iterator ot = intimg.begin();
	for( TImage::iterator it = image->begin();
		it != image->end(); ++it, ++ot )
	{			
			if ( *it > 5 ) *ot = 1;
			else *ot = 0;
	};
	CIsoSurface iso( intimg.getExtent(0), intimg.getExtent(1), intimg.getExtent(2), intimg.getArray() );
	std::vector<TVector3D> vertices;
	std::vector<TriangleKey> tris;
	iso.ExtractContour (0.5, vertices, tris);
	cerr << "After extraction" << vertices.size() << " / " << tris.size() << endl;
	iso.MakeUnique( vertices, tris );
	cerr << "After unification" << vertices.size() << " / " << tris.size() << endl;
	iso.OrientTriangles( vertices, tris, false );
	cerr << "After orientation" << vertices.size() << " / " << tris.size() << endl;
/*	vector<SVertex*> vVector;
	for( std::vector<TVector3D>::iterator it = vertices.begin(); it != vertices.end(); ++it )
	{
		SVertex* v = new SVertex( *it );
		vVector.push_back( v );
	}
	list<TFIn> FL;*/
	// Now we need to sort the vectors according to their position. Lets hope to find some double vertices ....

//	cerr << " " << endl << "Generated vertex lists " << vVector.size() << endl;
	if ( outputMesh != NULL ) 
		outputMesh->Reset();
	else
		outputMesh = vtkPolyData::New();
	cerr << "Converting Mesh to VTK... ";
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
	cerr << "done" << endl;
	
	cerr << "lp " << lp.use_count() << endl;
	lp.reset();
	
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
  /*
	work = original;
	
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
	//vector<TVector3D> vVector;
	vector<TMesh::VertexHandle> vHandle;
	vector<TMesh::VertexHandle> vvHandle;
	for( vector<TVIn>::iterator it = VV.begin(); it != VV.end(); ++it )
	{
		if ( it->del == false ) 
		{
			vHandle.push_back( work.add_vertex( TMesh::Point( it->thePosition[0], it->thePosition[1], it->thePosition[2] ) ) ); 
		}			
	}
	
	//cerr << " " << endl << "Generated vertex lists " << vHandle.size() << endl;
	// Generate triangles
	for( list<TFIn>::iterator it = FL.begin(); it != FL.end(); ++it )
	{
		vector<TMesh::VertexHandle> face_vhandles(3);
		//cerr << it->vts[0] << " " << it->vts[1] << " " << it->vts[2] << " (" << vHandle.size() << endl; 
		face_vhandles[0] = vHandle[vertexMap[it->vts[0]]];
		face_vhandles[1] = vHandle[vertexMap[it->vts[1]]];
		face_vhandles[2] = vHandle[vertexMap[it->vts[2]]];
		work.add_face( face_vhandles );
	}

	TMesh::VertexIter v_it, v_end = work.vertices_end();
	for( v_it = work.vertices_begin(); v_it != v_end; ++v_it )
	{
		v_it->theForce = 0.0; v_it->theForce[0] = 1.0;
		v_it->isStable = false;
		v_it->stability = 0;
		v_it->lastPositions.clear();
	}
	OpenMesh::Utils::MeshCheckerT<TMesh> myChecker( work );
	if( !myChecker.check() )
		exit(1);*/
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
	/* Read point data from file */
/*  vtkPoints *meshpts = vtkPoints::New();
	
	ulong i = 0;
	vtkDoubleArray* pointForces = vtkDoubleArray::New();
	
	ifstream file( "/home/belitz/nclinks.pts" );
	uint noOfPoints = 0;
	file >> noOfPoints;
	// read number of points
	meshpts->SetNumberOfPoints( noOfPoints );	
	for( int i = 0; i < noOfPoints; ++i )
	{
		double v[3];
		file >> v[0] >> v[1] >> v[2];		
		meshpts->SetPoint( i, v );
		pointForces->InsertTuple1( i, 128.0 );		
	}
		
	vtkPolyData* mesh = vtkPolyData::New();
	mesh->SetPoints( meshpts );
	mesh->GetPointData()->SetScalars( pointForces );
*/	
  /* Use PowerCrust */
//  vtkPowerCrustSurfaceReconstruction* surf = vtkPowerCrustSurfaceReconstruction::New();
//  surf->SetInput(mesh);
  /* Show mesh */
//  surf->Update();
//  inputMesh = surf->GetOutput();
  
// 	work.reset();
// 	
// 	cout << "Opening file" << endl;
// 	ifstream file( "/home/belitz/tetrahedron.pd" );
// 	vector<TVIn> VV;
// 	list<TFIn> FL;
// 	char buff[200]; int read=200;
// 	file.getline(buff,read);
// 	cout << buff << endl;
// //	int i;
// 	string ctxt; int vts; int fcs;
// 	file >> ctxt >> vts;
// 	cout << ctxt << ": " << vts << endl;
// 	file >> ctxt >> fcs;
// 	cout << ctxt << ": " << fcs << endl;
// 	for( int i = 0; i < vts; ++i )
// 	{
// 		TVIn v;
// 		file >> v.ulID >> v.thePosition[0] >> v.thePosition[1] >> v.thePosition[2];
// 		v.ulID = i;
// 		VV.push_back(v);
// 		//cout << v.ulID << ": " << v.thePosition[0] << " " << v.pos[1] << " " << v.pos[2] << endl;
// 	}
// 	// Now we need to sort the vectors according to their position. Lets hope to find some double vertices ....
// 	std::sort( VV.begin(), VV.end(), vecLess );
// 	std::map<ulong,ulong> vertexMap; // Maps the indices read to vertex array indices. Double vertices will be removed
// 	ulong rem = 0;
// 	for( uint i = 0; i < VV.size(); ++i )
// 	{
// 		// Find the end of the current block
// 		uint begin = i;
// 		uint end = i;
// 		while( end < (VV.size()-1) && !vecLess(VV[end],VV[end+1]) ) 
// 			end++;
// 		for( uint j = begin; j < end+1; j++ )
// 		{
// 			vertexMap[VV[j].ulID] = begin;
// 			if ( j != begin ) {VV[j].del = true; ++rem;}
// 			else VV[j].del = false;
// 		}
// 		i = end;
// 	}
// 	cerr << "Marked " << rem << " vertices for removal" << endl;
// 	for( int i = 0; i < fcs; ++i )
// 	{
// 		TFIn f;
// 		file >> f.vts[0] >> f.vts[1] >> f.vts[2];
// 		FL.push_back(f);
// 	}	
// 	file.close();
// 	cerr << "File read" << endl;
// 	// All data was read. Now build the vertex lists
// 	vector<SVertex*> vVector;
// 	for( vector<TVIn>::iterator it = VV.begin(); it != VV.end(); ++it )
// 	{
// 			SVertex* v = new SVertex( it->thePosition );
// 			v->ulID = it->ulID; 
// 			if ( vid < it->ulID ) 
// 				vid = it->ulID;
// 			if ( it->del == false ) 
// 				work.vList.push_back( v );
// 			vVector.push_back( v );
// 			cout << ".";
// 	}
// 	cerr << endl << "Generated vertex lists" << endl;
// 	// Generate triangles
// 	list<Pair> pairList;
// 	for( list<TFIn>::iterator it = FL.begin(); it != FL.end(); ++it )
// 	{
// 		SFace* f = new SFace;
// 		SVertex* p1 = vVector[ vertexMap[it->vts[0]] ];
// 		SVertex* p2 = vVector[ vertexMap[it->vts[1]] ];
// 		SVertex* p3 = vVector[ vertexMap[it->vts[2]] ];
// 		SEdge* e1 = new SEdge( p2, f );
// 		SEdge* e2 = new SEdge( p3, f );
// 		SEdge* e3 = new SEdge( p1, f );
// 		e1->nextEdgePtr = e2;
// 		e2->nextEdgePtr = e3;
// 		e3->nextEdgePtr = e1;
// 		work.eList.push_back( e1 );
// 		work.eList.push_back( e2 );
// 		work.eList.push_back( e3 );
// 		p1->anEdgePtr = e1;
// 		p2->anEdgePtr = e2;
// 		p3->anEdgePtr = e3;
// 		f->anEdgePtr = e1;
// 		work.fList.push_back(f);
// 		Pair p;
// 		p.ulID1 = vertexMap[it->vts[0]];
// 		p.ulID2 = vertexMap[it->vts[1]];
// 		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
// 		p.anEdgePtr = e1;
// 		pairList.push_back( p );
// 		p.ulID1 = vertexMap[it->vts[1]];
// 		p.ulID2 = vertexMap[it->vts[2]];
// 		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
// 		p.anEdgePtr = e2;
// 		pairList.push_back( p );
// 		p.ulID1 = vertexMap[it->vts[2]];
// 		p.ulID2 = vertexMap[it->vts[0]];
// 		if ( p.ulID2 < p.ulID1 ) swap( p.ulID1, p.ulID2 );
// 		p.anEdgePtr = e3;
// 		pairList.push_back( p );		
// 	}
// 	pairList.sort( pairLess );
// 	// Rebuild topology
// 	list<Pair>::iterator it = pairList.begin();
// 	while( it != pairList.end() )
// 	{
// 		Pair p = *it; ++it;
// 		if ( it->ulID1 == p.ulID1 && it->ulID2 == p.ulID2 )
// 		{
// 			p.anEdgePtr->opposingEdgePtr = it->anEdgePtr;
// 			it->anEdgePtr->opposingEdgePtr = p.anEdgePtr;
// 			++it;
// 		}
// 		else 
// 		{
// 			cout << "WARN: Pair " << p.ulID1 << "/" << p.ulID2 << " has no partner" << endl;
// 			cout << "Next is " << it->ulID1 << "/" << it->ulID2 << endl;
// 			p.anEdgePtr->opposingEdgePtr = p.anEdgePtr;
// 		}
// 	}	
//  	cerr << "Rebuild topology" << endl;
// 	vid++;
//  	cerr << "Resulting mesh consists of " << work.fList.size() << " faces and " << work.vList.size() << " vertices" << endl;
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
