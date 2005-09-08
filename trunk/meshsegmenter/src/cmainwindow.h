/************************************************************************
 * File: cmainwindow                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz <h.belitz@fz-juelich.de>                                           *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Pre-Alpha                                                   *
 * Created: Mo Okt 18 2004                                                      *
 * Based on:                                                            *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H
#include <mesh.h>
#include <vector>
#include <cimageloader.h>
#include <cfilehandler.h>
#include <cdatafileserver.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkContourFilter.h>
#include <vtkQuadricClustering.h>
#include <vtkQuadricDecimation.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkStructuredPointsReader.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyData.h>
#include <vtkImageChangeInformation.h>
#include <vtkAxes.h>
#include <vtkMaskPoints.h>
#include <vtkConeSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkGlyph3D.h>

#include <iostream>
#include <qmainwindow.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qscrollbar.h>
#include <qlcdnumber.h>
#include <qfiledialog.h>
#include <vtkQtRenderWindow.h>
#include <vtkQtRenderWindowInteractor.h>

#include <cdatasetcolors.h>
#include <cimagedata.h>
#include <cdiscretemodel.h>
#include <cobserver.h>

using namespace std;

/**
@author Hendrik Belitz
*/
class CMainWindow : public QMainWindow, public CObserver
{
Q_OBJECT
public:
    CMainWindow( const std::string& filename ) throw();
    ~CMainWindow() throw();
public slots:
	void iterateModel();
	void loadDataFile();
	void loadLutFile();
	void updateSliceX( int i ) { imageData.setSlicePosition( 0, i ); interactor->Render();}
	void updateSliceZ( int i ) { imageData.setSlicePosition( 2, i ); interactor->Render();}
	void updateSliceY( int i ) { imageData.setSlicePosition( 1, i ); interactor->Render();}
	void updateMax( int i ) { dataColors.setUpperClamp( static_cast<double>( i ) ); interactor->Render();}
	void updateMin( int i ) { dataColors.setLowerClamp( static_cast<double>( i ) ); interactor->Render();}
	virtual void execute( boost::shared_ptr<CEvent> anEvent );
public:	
/*private:*/
	vtkQuadricDecimation *deci;
	vtkWindowedSincPolyDataFilter *smooth;
	QPushButton *iterateButton;
	QLineEdit *fileName;
	QLineEdit *decEdit;
	QLineEdit *smoothEdit;
	QCheckBox *wireframeCheck;
	vtkRenderer *renderer;
	vtkActor *actorMesh;
	vtkActor *actorMeshNormal;
	vtkActor *axesActor;
	vtkAxes* coordSystem;
	vtkPolyData *inputMesh;
	vtkPolyData *outputMesh;
	CMesh original;
	CMesh work;
	vtkQtRenderWindow *window;
	vtkQtRenderWindowInteractor *interactor;
	vtkStructuredPointsReader *reader;	
	void generateMeshFromVTK( bool flip = false );
	void generateMeshFromFile( std::string filename );
	void subdivideMesh();
	void transformBackMesh();
	void generateSimpleMesh();
	ulong eid;
	ulong vid;
	vtkPolyDataNormals *normals3;
	vtkPolyDataNormals *normals4;
	vtkGlyph3D *glyph;
	vtkTransform *trans;
	vtkConeSource *cone;
	vtkPolyDataMapper *mapperMeshNormalData;
	vtkMaskPoints *ptMask;
	vtkTransformPolyDataFilter* tpdf;
	vtkPolyDataMapper *mapperMeshData; 
	CImageData imageData;
	CDataSetColors dataColors;
	CDataSetColors meshColors;
	QPushButton* dataFileButton;
	QPushButton* lutFileButton;
	QScrollBar* selX;
	QScrollBar* selY;
	QScrollBar* selZ;
	QLCDNumber* vX;
	QLCDNumber* vY;
	QLCDNumber* vZ;
	QScrollBar* selMax;
	QScrollBar* selMin;
	QLCDNumber* vMax;
	QLCDNumber* vMin;
	TField3DPtr forceField;
	CDiscreteModel theModel;
private:
	void initUI();
	void initMesh();
	void initVis();	
};

#endif
