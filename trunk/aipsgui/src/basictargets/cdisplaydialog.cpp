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
#include "cdisplaydialog.h"

#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>

using namespace std;

CDisplayWindow::CDisplayWindow() : lutmin(0.0),lutmax(127.0),actualImage(NULL)
{
  //setFixedSize(260,260);
  setMinimumSize(256,256);
  aColumnPtr = new QVBox( this );
  aColumnPtr->setGeometry(0,0,260,260);
  aColumnPtr->setMargin(2);
  
  display = new vtkQtRenderWindow(aColumnPtr);
  display->setMinimumSize(128,128);
  interactor = vtkQtRenderWindowInteractor::New();
  interactor->SetRenderWindow( display );
  interactor->SetInteractorStyle( vtkInteractorStyleImage::New() );
  renderer = vtkRenderer::New();
  display->AddRenderer( renderer );

  theLookupTable = vtkLookupTable::New();
  theLookupTable->SetTableRange( 0.0, 127.0 );
  theLookupTable->SetRampToLinear();
  theLookupTable->SetSaturationRange( 0.0, 0.0 );
  theLookupTable->SetValueRange( 0.0, 1.0 );
  theLookupTable->SetHueRange( 0.0, 0.0);
  theLookupTable->Build();
  theLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 );
  theColorBar = vtkScalarBarActor::New();
  theColorBar->SetLookupTable( theLookupTable );
  theColorBar->SetOrientationToVertical();
  theColorBar->SetWidth( 0.05 );
  theColorBar->SetHeight( 0.9 );
  dClampValues[0] = 0.0;
  dClampValues[1] = 127.0;

  theImage = vtkImageActor::New();
  colorsMapper = vtkImageMapToColors::New();
  theImage->SetInput( colorsMapper->GetOutput() );
  renderer->AddProp( theImage );
  renderer->AddActor( theColorBar );
  
  doc1 = new QLabel( "Data range minimum", aColumnPtr );
  dataMin = new QScrollBar ( static_cast<int>(lutmin), static_cast<int>(lutmax),
    1, 10, static_cast<int>(lutmin), Qt::Horizontal, aColumnPtr );
  doc2 = new QLabel( "Data range maximum", aColumnPtr );
  dataMax = new QScrollBar ( static_cast<int>(lutmin), static_cast<int>(lutmax),
    1, 10, static_cast<int>(lutmax), Qt::Horizontal, aColumnPtr );
  dClampValues[0] = 0.0;
  dClampValues[1] = 127.0;
  QHBox* aRow1Ptr = new QHBox ( aColumnPtr );
  transparency = new QCheckBox( aRow1Ptr );
  transparency->setChecked( true );
  doc3 = new QLabel( "Background transparency", aRow1Ptr );
  QHBox* aRow2Ptr = new QHBox ( aColumnPtr );
  interpolate = new QCheckBox( aRow2Ptr );
  interpolate->setChecked( true );
  doc4 = new QLabel( "Interpolate", aRow2Ptr );
  connect ( dataMin, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateMin( int ) ) );
  connect ( dataMax, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateMax( int ) ) );
  connect( transparency, SIGNAL( stateChanged( int ) ),
    this, SLOT( toggleTransparency( int ) ) );
  connect( interpolate, SIGNAL( stateChanged( int ) ),
    this, SLOT( toggleInterpolation( int ) ) );
}

void CDisplayWindow::toggleTransparency( int i )
{
  if ( i == QButton::On )
  {
    theLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 );
  }
  else
  {
    theLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 1.0 );
  }
  if ( !doNotUpdate ) interactor->Render();
}

void CDisplayWindow::toggleInterpolation( int i )
{
  if ( i == QButton::On )
  {
    theImage->InterpolateOn();
  }
  else
  {
    theImage->InterpolateOff();
  }
  if ( !doNotUpdate ) interactor->Render();
}

void CDisplayWindow::loadLookupTable( std::string sFilename )
{
  theLookupTable->SetNumberOfTableValues( 256 );
  ifstream file ( sFilename.c_str() );
  unsigned char r[256],g[256],b[256];
  double color[4];
  color[3] = 1.0;
  file.read( (char*)r, 256 );
  file.read( (char*)g, 256 );
  file.read( (char*)b, 256 );
  file.close();
  for( int i = 0; i < 256; ++i )    
  {
    color[0] = static_cast<double>( r[i] ) / 255.0;
    color[1] = static_cast<double>( g[i] ) / 255.0;
    color[2] = static_cast<double>( b[i] ) / 255.0;   
    theLookupTable->SetTableValue( i, color );
  } 
  theLookupTable->GetTableValue( 0, color );
  color[3] = 0.0;
  theLookupTable->SetTableValue( 0, color );
  if ( !doNotUpdate ) interactor->Render();
}

void CDisplayWindow::setUpperClamp( double dValue )
{
  dClampValues[1] = dValue;
  if ( !doNotUpdate ) 
  {theLookupTable->SetTableRange( dClampValues );
  colorsMapper->Update();
  //renderer->Render();
  interactor->Render();}
}

void CDisplayWindow::setLowerClamp( double dValue )
{
  dClampValues[0] = dValue;
  if ( !doNotUpdate ) 
  {
  theLookupTable->SetTableRange( dClampValues );
  colorsMapper->Update();
  //renderer->Render();
  interactor->Render();
  }
}

void CDisplayWindow::updateMax( int i )
{
  setUpperClamp( static_cast<double>( i ) );
  if ( i < dataMin->value() )
  {
    dataMin->setValue( i );
    setLowerClamp( static_cast<double>( i ) );
  }
}

void CDisplayWindow::updateMin( int i ) 
{
  setLowerClamp( static_cast<double>( i ) );
  if ( i > dataMax->value() )
  {
    dataMax->setValue( i );
    setUpperClamp( static_cast<double>( i ) );
  }
}

void CDisplayWindow::update()
{
  if ( !doNotUpdate ) interactor->Render();
}

vtkRenderer* CDisplayWindow::getRenderer()
{
  return renderer;
}

void CDisplayWindow::testDataRamge( double minRange, double maxRange )
{
FBEGIN;
	doNotUpdate = true;
	if( dataMin->minValue() != static_cast<int>( minRange )
    || dataMin->maxValue() != static_cast<int>( maxRange ) )
  {
  	cerr << "a1" << endl;    
    double newOffset = static_cast<int>( minRange ) - dataMin->minValue();
    double factor = (maxRange+newOffset) / static_cast<double>( dataMin->maxValue() );    
    double value = ( static_cast<double>( dataMin->value() ) * factor ) - newOffset;
    cerr << "off " << newOffset << " factor " << factor << " value " << value << endl;
    dClampValues[0] = minRange;
    dClampValues[1] = maxRange;
    dataMin->setMinValue( static_cast<int>( dClampValues[0] ) );
    dataMin->setMaxValue( static_cast<int>( dClampValues[1] ) );
    dataMin->setValue( static_cast<int>( value ) );    
  }
  if( dataMax->minValue() != static_cast<int>( minRange )
    || dataMax->maxValue() != static_cast<int>( maxRange ) )
  {
  	cerr << "a2" << endl;
    double newOffset = static_cast<int>( minRange ) - dataMax->minValue();
    double factor = (maxRange+newOffset) / static_cast<double>( dataMax->maxValue() );
    double value = ( static_cast<double>( dataMax->value() ) * factor ) - newOffset;
    cerr << "off " << newOffset << " factor " << factor << " value " << value << endl;
    
    dClampValues[0] = minRange;
    dClampValues[1] = maxRange;
    dataMax->setMinValue( static_cast<int>( dClampValues[0] ) );
    dataMax->setMaxValue( static_cast<int>( dClampValues[1] ) );
    dataMax->setValue( static_cast<int>( value ) );        
  }  
  doNotUpdate = false;
FEND;
}

CDisplayWindow::~CDisplayWindow()
{
  theLookupTable->Delete();
  theImage->Delete();
  theColorBar->Delete();
  colorsMapper->Delete();
  renderer->Delete();  
  interactor->Delete();
  display->Delete();
  
  delete aColumnPtr;
}

void CDisplayWindow::resizeEvent( QResizeEvent* e ) throw()
{
  aColumnPtr->setFixedSize( e->size() );
  //displayPtr->
}

void CDisplayWindow::setImage( vtkImageData* anImage )
{
 FBEGIN;
	doNotUpdate = true;
	theLookupTable->SetTableRange( dClampValues );
	colorsMapper->SetLookupTable( theLookupTable );
  if ( actualImage ) 
    actualImage->Delete();
  actualImage = anImage;
	colorsMapper->SetInput( anImage );
  doNotUpdate = false;
  interactor->Render();
 FEND;
}

CDisplayDialog::CDisplayDialog() throw()
 : aips::CModuleDialog(), width(0), height(0)
{
  displayPtr = new CDisplayWindow();
  displayPtr->hide();
  displayPtr->setCaption( "2D image viewer" );

  displayPtr->getRenderer()->TwoSidedLightingOn();
  displayPtr->getRenderer()->SetAmbient( 1.0, 1.0, 1.0 );  
  displayPtr->getRenderer()->GetActiveCamera()->SetPosition(128.0,128.0,-256.0);
  displayPtr->getRenderer()->GetActiveCamera()->SetFocalPoint(128.0,128.0,0.0);
  displayPtr->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
  displayPtr->getRenderer()->GetActiveCamera()->SetViewUp(0.0,-1.0,0.0);
  displayPtr->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
  displayPtr->getRenderer()->ResetCamera();
}


CDisplayDialog::~CDisplayDialog() throw()
{
  delete displayPtr;
}

/** \returns true */
bool CDisplayDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CDisplayDialog::getDialogHandle() const throw( NotPresentException )
{
  return displayPtr;
}

/** \param sCaption the new window caption */
void CDisplayDialog::setCaption ( const std::string sCaption ) throw()
{
  displayPtr->setCaption( sCaption.c_str() );
}

void CDisplayDialog::showDialog() throw( NotPresentException )
{
  displayPtr->show();
}

void CDisplayDialog::hideDialog() throw( NotPresentException )
{
  displayPtr->hide();
}

bool CDisplayDialog::isHidden() throw( NotPresentException )
{
  return displayPtr->isHidden();
}

void CDisplayDialog::updateView( TImagePtr inputPtr ) throw()
{
BENCHSTART;
FBEGIN;
  if ( !inputPtr )
    return;  
  CVTKAdapter myAdapter( inputPtr );
  vtkImageData* myImage = myAdapter.convertToExternal();
  //int* dims = myImage->GetDimensions();
/*  cerr << dims[0] << " x " << dims[1] << " x " << dims[2] << " - " << myImage->GetNumberOfCells() << endl;
  cerr << "Data range: " << inputPtr->getMinimum() << " - " << inputPtr->getMaximum() << endl;*/
  myImage->SetScalarTypeToUnsignedShort();
  displayPtr->testDataRamge( inputPtr->getMinimum(), inputPtr->getMaximum() );
  displayPtr->setImage( myImage );
  if ( width != inputPtr->getExtent(0) || height != inputPtr->getExtent(1) )
  {
    width = inputPtr->getExtent(0);
    height = inputPtr->getExtent(1);
    displayPtr->getImage()->SetDisplayExtent(0,inputPtr->getExtent(0)-1,0,inputPtr->getExtent(1)-1,0,0);
    displayPtr->getRenderer()->GetActiveCamera()->SetPosition(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0,
      1.0 * static_cast<double>( std::max( inputPtr->getExtent(0), inputPtr->getExtent(1) ) ) );
    displayPtr->getRenderer()->GetActiveCamera()->SetFocalPoint(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0 , 0.0 );
    displayPtr->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
    displayPtr->getRenderer()->GetActiveCamera()->SetViewUp(0.0,-1.0,0.0);
    displayPtr->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
    displayPtr->getRenderer()->ResetCamera();
  }
  displayPtr->update();
FEND;  
BENCHSTOP;
}

void CDisplayDialog::updateView( TFieldPtr inputPtr ) throw()
{
BENCHSTART;
  exit( 0 );
  CVTKAdapter myAdapter( inputPtr );
//  vtkImageData* myImage = myAdapter.convertToExternal();
//   myCast->SetInput( myImage );
  displayPtr->update();
/*  QImage processed;
  switch( inputPtr->getDataDimension() )
  {
    case 1:
      processed.create( inputPtr->getExtent(0), inputPtr->getExtent(1), 8, 256 );
      break;
		case 2:
    case 3:
      processed.create( inputPtr->getExtent(0), inputPtr->getExtent(1), 32, 16777216 );
      break;
    case 4:
      processed.create( inputPtr->getExtent(0), inputPtr->getExtent(1), 32, 16777216*256 );
      break;
  }

  float fIntensityRange = 1.0 /
    static_cast<float>( inputPtr->getMaximum() - inputPtr->getMinimum() + 1 ) * 256.0;
	double dMinimum = inputPtr->getMinimum();
  if ( inputPtr->getMaximum() < 2 ) 
	{
		fIntensityRange = 255.0;
		dMinimum = 0;
	}
  for ( ushort x = 0; x < inputPtr->getExtent(0); ++x )
    for ( ushort y = 0; y < inputPtr->getExtent(1); ++y )
    {
      if ( inputPtr->getDataDimension() == 1 )
      {
        ushort value = static_cast<ushort>( ( (*inputPtr)( x, y ) 
					- dMinimum ) * fIntensityRange );
        if ( value < 256 ) processed.setPixel( x, y, value );
#ifdef DEBUG				
        else
        {
          alog << LWARN << "\n **** Pixel value " << (*inputPtr)( x, y ) << "/" << value 
					<< " too high (" << inputPtr->getMaximum() << "/" << dMinimum << ") ****" 
					<< " fInten " << fIntensityRange << endl;
        }
#endif				
      }
      else if ( inputPtr->getDataDimension() == 2 )
        processed.setPixel( x, y, qRgb(	static_cast<ushort>( static_cast<float>( 
					(*inputPtr)( x, y, 0 ) - inputPtr->getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ), 0 ) );
      else if ( inputPtr->getDataDimension() == 3 )
        processed.setPixel( x, y, qRgb( 
				static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 0 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 2 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ) ) );
      else if ( inputPtr->getDataDimension() == 4 )
        processed.setPixel( x, y, qRgba( 
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 0 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 2 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ), 
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 3 ) 
					- inputPtr->getMinimum() ) * fIntensityRange ) ) );
    }
  displayPtr->setImage( processed );*/
BENCHSTOP;
}

void CDisplayDialog::activateDialog()	throw( NotPresentException )
{
	displayPtr->setActiveWindow();
}
