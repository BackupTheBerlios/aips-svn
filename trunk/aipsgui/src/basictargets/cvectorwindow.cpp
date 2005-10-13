//
// C++ Implementation: cvectorwindow
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cvectorwindow.h"

#ifdef USE_RENDERING
#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>
#endif

using namespace std;

#ifdef USE_RENDERING
CVectorWindow::CVectorWindow() : QWidget( NULL ), theActualImage(NULL), theActualOverlay(NULL)
{
  dImageLut[0] = 0.0;
  dImageLut[1] = 127.0;
  dOverlayLut[0] = 0.0;
  dOverlayLut[1] = 1.0;
  dImageClampValues[0] = 0.0;
  dImageClampValues[1] = 127.0;
  dOverlayClampValues[0] = 0.0;
  dOverlayClampValues[1] = 1.0;
  //setFixedSize(260,260);
  setMinimumSize(256,256);
  aGlobalColumnPtr = new QVBox( this );
  aGlobalColumnPtr->setGeometry(0,0,250,250);
  aGlobalColumnPtr->setMargin(2);
  aGlobalColumnPtr->setSpacing(8);
  
  display = new vtkQtRenderWindow( aGlobalColumnPtr );
  //display->setMinimumSize(128,128);
  interactor = vtkQtRenderWindowInteractor::New();
  interactor->SetRenderWindow( display );
  interactor->SetInteractorStyle( vtkInteractorStyleImage::New() );
  renderer = vtkRenderer::New();
  display->AddRenderer( renderer );

  theImageLookupTable = vtkLookupTable::New();
  theImageLookupTable->SetTableRange( 0.0, 127.0 );
  theImageLookupTable->SetRampToLinear();
  theImageLookupTable->SetSaturationRange( 0.0, 0.0 );
  theImageLookupTable->SetValueRange( 0.0, 1.0 );
  theImageLookupTable->SetHueRange( 0.0, 0.0);
  theImageLookupTable->Build();
  theImageLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 );
  theImageColorBar = vtkScalarBarActor::New();
  theImageColorBar->SetLookupTable( theImageLookupTable );
  theImageColorBar->SetOrientationToVertical();
  theImageColorBar->SetWidth( 0.05 );
  theImageColorBar->SetHeight( 0.9 );
  dImageClampValues[0] = dImageLut[0];
  dImageClampValues[1] = dImageLut[1];
  theImage = vtkImageActor::New();
  theImageColorsMapper = vtkImageMapToColors::New();
  theImageColorsMapper->SetLookupTable( theImageLookupTable );
  theImageColorsMapper->SetOutputFormatToRGBA();
  theImageColorsMapper->PassAlphaToOutputOff();  

  renderer->AddActor( theImageColorBar );

  theOverlayLookupTable = vtkLookupTable::New();
  theOverlayLookupTable->SetTableRange( 0.0, 127.0 );
  theOverlayLookupTable->SetRampToLinear();
  theOverlayLookupTable->SetSaturationRange( 0.0, 0.0 );
  theOverlayLookupTable->SetValueRange( 0.0, 1.0 );
  theOverlayLookupTable->SetHueRange( 0.0, 1.0);
  theOverlayLookupTable->Build();
  theOverlayLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 );
  theOverlayColorBar = vtkScalarBarActor::New();
  theOverlayColorBar->SetLookupTable( theOverlayLookupTable );
  theOverlayColorBar->SetLayerNumber( 1 );
  theOverlayColorBar->SetOrientationToVertical();
  theOverlayColorBar->SetWidth( 0.05 );
  theOverlayColorBar->SetHeight( 0.9 );
  dOverlayClampValues[0] = dOverlayLut[0];
  dOverlayClampValues[1] = dOverlayLut[1];
  theGlyphSource = vtkArrowSource::New();
  theFieldGlyphs = vtkGlyph2D::New();
  theFieldGlyphs->SetScaleModeToScaleByVector();
  theFieldGlyphs->SetColorModeToColorByVector();
  theFieldGlyphs->SetVectorModeToUseVector();
  theFieldGlyphs->SetSource( theGlyphSource->GetOutput() );  
  theFieldMapper = vtkPolyDataMapper2D::New();
  theFieldMapper->SetInput( theFieldGlyphs->GetOutput() );
  theFieldMapper->SetLookupTable( theOverlayLookupTable );
  theOverlay = vtkActor2D::New();
  theOverlay->SetMapper( theFieldMapper );
  theImage->SetInput( theImageColorsMapper->GetOutput() );
  //renderer->AddActor( theImage );
  renderer->AddActor( theOverlay );
  renderer->AddActor( theOverlayColorBar );
  double* pos = theImageColorBar->GetPosition();
  theOverlayColorBar->SetPosition( pos[0]+0.05, pos[1] );
  
  QTabWidget* dialog = new QTabWidget( aGlobalColumnPtr );
  // Setup tab 1
  QVBox* aColumn1Ptr = new QVBox( this );
  dialog->addTab( aColumn1Ptr, "Image" );
  aColumn1Ptr->setGeometry(0,0,250,250);
  aColumn1Ptr->setMargin(2);
  aColumn1Ptr->setSpacing(8);
  
  theImageLutFileButton = new QPushButton( "Color table", aColumn1Ptr );
  theImageLutFileButton->setMinimumSize( 200, 25 );
  QLabel* doc1 = new QLabel( "Data range minimum", aColumn1Ptr );
  QHBox* aRow3Ptr = new QHBox ( aColumn1Ptr );
  theImageDataMin = new QScrollBar ( 0, 1000, 1, 50, 0, Qt::Horizontal, aRow3Ptr );
  theImageDataMin->setMinimumSize(256,16);
  aRow3Ptr->setStretchFactor( theImageDataMin, 3 );
  aRow3Ptr->setSpacing(10);
  theImageMinDisplay = new QLineEdit( QString::number( dImageLut[0] ), aRow3Ptr );
  theImageMinDisplay->setFixedSize(96,16);
  theImageMinDisplay->setValidator( new QDoubleValidator(0) );
  theImageMinDisplay->setText( QString::number( dImageClampValues[0] ) );
  QLabel* doc2 = new QLabel( "Data range maximum", aColumn1Ptr );
  QHBox* aRow4Ptr = new QHBox ( aColumn1Ptr );
  theImageDataMax = new QScrollBar (0, 1000, 1, 50, 1000, Qt::Horizontal, aRow4Ptr );
  theImageDataMax->setMinimumSize(256,16);
  aRow4Ptr->setStretchFactor( theImageDataMax, 3 );
  aRow4Ptr->setSpacing(10);
  theImageMaxDisplay = new QLineEdit( QString::number( dImageLut[1] ), aRow4Ptr );
  theImageMaxDisplay->setFixedSize(96,16);
  theImageMaxDisplay->setValidator( new QDoubleValidator(0) );
  theImageMaxDisplay->setText( QString::number( dImageClampValues[1] ) );
  QHBox* aRow1Ptr = new QHBox ( aColumn1Ptr );
  aRow1Ptr->setSpacing(5);
  theImageTransparency = new QCheckBox( aRow1Ptr );
  theImageTransparency->setFixedSize(16,16);
  theImageTransparency->setChecked( true );
  QLabel* doc3 = new QLabel( "Background transparency", aRow1Ptr );
  QHBox* aRow2Ptr = new QHBox ( aColumn1Ptr );
  aRow2Ptr->setSpacing(5);
  theImageInterpolate = new QCheckBox( aRow2Ptr );
  theImageInterpolate->setFixedSize(16,16);
  theImageInterpolate->setChecked( true );
  QLabel* doc4 = new QLabel( "Interpolate", aRow2Ptr );

  connect( theImageLutFileButton, SIGNAL( clicked( ) ),
    this, SLOT( loadImageLutFile() ) );
  connect ( theImageDataMin, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateImageMin( int ) ) );
  connect ( theImageDataMax, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateImageMax( int ) ) );
  connect( theImageTransparency, SIGNAL( stateChanged( int ) ),
    this, SLOT( toggleImageTransparency( int ) ) );
  connect( theImageInterpolate, SIGNAL( stateChanged( int ) ),
    this, SLOT( toggleImageInterpolation( int ) ) );
  connect( theImageMinDisplay, SIGNAL( returnPressed() ),
    this, SLOT( imageMinDataChanged() ) );
  connect( theImageMinDisplay, SIGNAL( lostFocus() ),
    this, SLOT( imageMinDataChanged() ) );
  connect( theImageMaxDisplay, SIGNAL( returnPressed() ),
    this, SLOT( imageMaxDataChanged() ) );
  connect( theImageMaxDisplay, SIGNAL( lostFocus() ),
    this, SLOT( imageMaxDataChanged() ) );
    
  // Setup tab 2
  aColumn1Ptr = new QVBox( this );
  dialog->addTab( aColumn1Ptr, "Overlay" );
  aColumn1Ptr->setGeometry(0,0,250,250);
  aColumn1Ptr->setMargin(2);
  aColumn1Ptr->setSpacing(8);
  
  theOverlayLutFileButton = new QPushButton( "Color table", aColumn1Ptr );
  theOverlayLutFileButton->setMinimumSize( 200, 25 );
  doc1 = new QLabel( "Data range minimum", aColumn1Ptr );
  aRow3Ptr = new QHBox ( aColumn1Ptr );
  theOverlayDataMin = new QScrollBar ( 0, 1000, 1, 50, 0, Qt::Horizontal, aRow3Ptr );
  theOverlayDataMin->setMinimumSize(256,16);
  aRow3Ptr->setStretchFactor( theOverlayDataMin, 3 );
  aRow3Ptr->setSpacing(10);
  theOverlayMinDisplay = new QLineEdit( QString::number( dImageLut[0] ), aRow3Ptr );
  theOverlayMinDisplay->setFixedSize(96,16);
  theOverlayMinDisplay->setValidator( new QDoubleValidator(0) );
  theOverlayMinDisplay->setText( QString::number( dOverlayClampValues[0] ) );
  doc2 = new QLabel( "Data range maximum", aColumn1Ptr );
  aRow4Ptr = new QHBox ( aColumn1Ptr );
  theOverlayDataMax = new QScrollBar (0, 1000, 1, 50, 1000, Qt::Horizontal, aRow4Ptr );
  theOverlayDataMax->setMinimumSize(256,16);
  aRow4Ptr->setStretchFactor( theOverlayDataMax, 3 );
  aRow4Ptr->setSpacing(10);
  theOverlayMaxDisplay = new QLineEdit( QString::number( dImageLut[1] ), aRow4Ptr );
  theOverlayMaxDisplay->setFixedSize(96,16);
  theOverlayMaxDisplay->setValidator( new QDoubleValidator(0) );
  theOverlayMaxDisplay->setText( QString::number( dOverlayClampValues[1] ) );
  aRow1Ptr = new QHBox ( aColumn1Ptr );
  aRow1Ptr->setSpacing(5);
  theOverlayTransparency = new QCheckBox( aRow1Ptr );
  theOverlayTransparency->setFixedSize(16,16);
  theOverlayTransparency->setChecked( true );
  doc3 = new QLabel( "Background transparency", aRow1Ptr );
  aRow2Ptr = new QHBox ( aColumn1Ptr );
  aRow2Ptr->setSpacing(5);
  theOverlayInterpolate = new QCheckBox( aRow2Ptr );
  theOverlayInterpolate->setFixedSize(16,16);
  theOverlayInterpolate->setChecked( true );
  doc4 = new QLabel( "Interpolate", aRow2Ptr );

  connect( theOverlayLutFileButton, SIGNAL( clicked( ) ),
    this, SLOT( loadOverlayLutFile() ) );
  connect ( theOverlayDataMin, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateOverlayMin( int ) ) );
  connect ( theOverlayDataMax, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateOverlayMax( int ) ) );
  connect( theOverlayTransparency, SIGNAL( stateChanged( int ) ),
    this, SLOT( toggleOverlayTransparency( int ) ) );
  connect( theOverlayInterpolate, SIGNAL( stateChanged( int ) ),
    this, SLOT( toggleOverlayInterpolation( int ) ) );
  connect( theOverlayMinDisplay, SIGNAL( returnPressed() ),
    this, SLOT( overlayMinDataChanged() ) );
  connect( theOverlayMinDisplay, SIGNAL( lostFocus() ),
    this, SLOT( overlayMinDataChanged() ) );
  connect( theOverlayMaxDisplay, SIGNAL( returnPressed() ),
    this, SLOT( overlayMaxDataChanged() ) );
  connect( theOverlayMaxDisplay, SIGNAL( lostFocus() ),
    this, SLOT( overlayMaxDataChanged() ) );
  
}

void CVectorWindow::loadImageLutFile()
{
  string path;
  if ( getGlobalConfiguration().isDefined( "AIPS_LUT" ) )
    path = getGlobalConfiguration().getString( "AIPS_LUT" );
  else
    path = "/home/hendrik/bin/mricro";
  QString fn = QFileDialog::getOpenFileName( path.c_str(), "*.lut", this, "File loader", "Select a color table to load" );
  loadLookupTable( fn.ascii(), true );
  theImageLutFileButton->setText( fn );
}

void CVectorWindow::loadOverlayLutFile()
{
  string path;
  if ( getGlobalConfiguration().isDefined( "AIPS_LUT" ) )
    path = getGlobalConfiguration().getString( "AIPS_LUT" );
  else
    path = "/home/hendrik/bin/mricro";
  QString fn = QFileDialog::getOpenFileName( path.c_str(), "*.lut", this, "File loader", "Select a color table to load" );
  loadLookupTable( fn.ascii(), false );
  theOverlayLutFileButton->setText( fn );
}

void CVectorWindow::imageMinDataChanged()
{
  theImageDataMin->setValue( static_cast<int>( theImageMinDisplay->text().toDouble()
    / ( dImageClampValues[1] - dImageClampValues[0] ) * 1000.0 ) );
}

void CVectorWindow::imageMaxDataChanged()
{
  theImageDataMax->setValue( static_cast<int>( theImageMaxDisplay->text().toDouble()
    / ( dImageClampValues[1] - dImageClampValues[0] ) * 1000.0 ) );
}

void CVectorWindow::overlayMinDataChanged()
{
  theOverlayDataMin->setValue( static_cast<int>( theOverlayMinDisplay->text().toDouble()
    / ( dOverlayClampValues[1] - dOverlayClampValues[0] ) * 1000.0 ) );
}

void CVectorWindow::overlayMaxDataChanged()
{
  theOverlayDataMax->setValue( static_cast<int>( theOverlayMaxDisplay->text().toDouble()
    / ( dOverlayClampValues[1] - dOverlayClampValues[0] ) * 1000.0 ) );
}

void CVectorWindow::toggleImageTransparency( int i )
{
  if ( i == QButton::On )
  {
    theImageLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 );
  }
  else
  {
    theImageLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 1.0 );
  }
  if ( !doNotUpdate ) interactor->Render();
}

void CVectorWindow::toggleOverlayTransparency( int i )
{
  if ( i == QButton::On )
  {
    theOverlayLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 );
  }
  else
  {
    theOverlayLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 1.0 );
  }
  if ( !doNotUpdate ) interactor->Render();
}

void CVectorWindow::toggleImageInterpolation( int i )
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

void CVectorWindow::toggleOverlayInterpolation( int i )
{

}

void CVectorWindow::loadLookupTable( std::string sFilename, bool bForImage )
{
  if ( bForImage )
    theImageLookupTable->SetNumberOfTableValues( 256 );
  else
    theOverlayLookupTable->SetNumberOfTableValues( 256 );
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
    if ( bForImage )
      theImageLookupTable->SetTableValue( i, color );
    else
      theOverlayLookupTable->SetTableValue( i, color );
  }
  if ( bForImage )
  {
    theImageLookupTable->GetTableValue( 0, color );
    color[3] = 0.0;
    theImageLookupTable->SetTableValue( 0, color );
  }
  else
  {
    theOverlayLookupTable->GetTableValue( 0, color );
    color[3] = 0.0;
    theOverlayLookupTable->SetTableValue( 0, color );
  }  
  if ( !doNotUpdate ) interactor->Render();
}

void CVectorWindow::setImageUpperClamp( double dValue )
{
  dImageClampValues[1] = dValue;
  theImageMaxDisplay->setText( QString::number( dValue ) );
  if ( !doNotUpdate ) 
  {
    theImageLookupTable->SetTableRange( dImageClampValues );
    theImageColorsMapper->Update();
    interactor->Render();
  }
}

void CVectorWindow::setOverlayUpperClamp( double dValue )
{
  dOverlayClampValues[1] = dValue;
  theOverlayMaxDisplay->setText( QString::number( dValue ) );
  if ( !doNotUpdate ) 
  {
    theOverlayLookupTable->SetTableRange( dOverlayClampValues );
//    theOverlayColorsMapper->Update();
    interactor->Render();
  }
}

void CVectorWindow::setImageLowerClamp( double dValue )
{
  dImageClampValues[0] = dValue;
  theImageMinDisplay->setText( QString::number( dValue ) );
  if ( !doNotUpdate ) 
  {
    theImageLookupTable->SetTableRange( dImageClampValues );
    theImageColorsMapper->Update();
    interactor->Render();
  }
}

void CVectorWindow::setOverlayLowerClamp( double dValue )
{
  dOverlayClampValues[0] = dValue;
  theOverlayMinDisplay->setText( QString::number( dValue ) );
  if ( !doNotUpdate ) 
  {
    theOverlayLookupTable->SetTableRange( dOverlayClampValues );
//    theOverlayColorsMapper->Update();
    interactor->Render();
  }
}

void CVectorWindow::updateImageMax( int i )
{
  setImageUpperClamp( static_cast<double>( i ) / 1000.0 * ( dImageLut[1] - dImageLut[0] ) );
  if ( i < theImageDataMin->value() )
  {
    theImageDataMin->setValue( i );
    setImageLowerClamp( static_cast<double>( i ) / 1000.0 * ( dImageLut[1] - dImageLut[0] ) );
  }
}

void CVectorWindow::updateOverlayMax( int i )
{
  setOverlayUpperClamp( static_cast<double>( i ) / 1000.0 * ( dOverlayLut[1] - dOverlayLut[0] ) );
  if ( i < theOverlayDataMin->value() )
  {
    theOverlayDataMin->setValue( i );
    setOverlayLowerClamp( static_cast<double>( i ) / 1000.0 * ( dOverlayLut[1] - dOverlayLut[0] ) );
  }
}

void CVectorWindow::updateImageMin( int i )
{
  setImageLowerClamp( static_cast<double>( i ) / 1000.0 * ( dImageLut[1] - dImageLut[0] ) );
  if ( i > theImageDataMax->value() )
  {
    theImageDataMax->setValue( i );
    setImageUpperClamp( static_cast<double>( i ) / 1000.0 * ( dImageLut[1] - dImageLut[0] ) );
  }
}

void CVectorWindow::updateOverlayMin( int i )
{
  setOverlayLowerClamp( static_cast<double>( i ) / 1000.0 * ( dOverlayLut[1] - dOverlayLut[0] ) );
  if ( i > theOverlayDataMax->value() )
  {
    theOverlayDataMax->setValue( i );
    setOverlayUpperClamp( static_cast<double>( i ) / 1000.0 * ( dOverlayLut[1] - dOverlayLut[0] ) );
  }
}

void CVectorWindow::update()
{
  if ( !doNotUpdate ) interactor->Render();
}

vtkRenderer* CVectorWindow::getRenderer()
{
  return renderer;
}

void CVectorWindow::testImageDataRange( double minRange, double maxRange )
{
FBEGIN;
  doNotUpdate = true;
  if( dImageLut[0] != static_cast<int>( minRange )
    || dImageLut[1] != static_cast<int>( maxRange ) )
  {
    dImageLut[0] = minRange;
    dImageLut[1] = maxRange;
    double value = static_cast<double>( theImageMinDisplay->text().toDouble() );
    dImageClampValues[0] = value;
    theImageMinDisplay->setText( QString::number( value ) );
    theImageDataMin->setValue( static_cast<int>( value / ( dImageLut[1] - dImageLut[0] ) * 1000.0 ) );
    value = static_cast<double>( theImageMaxDisplay->text().toDouble()  );
    dImageClampValues[1] = value;
    theImageMaxDisplay->setText( QString::number( value ) );
    theImageDataMax->setValue( static_cast<int>( value / ( dImageLut[1] - dImageLut[0] ) * 1000.0 ) );
  }  
  doNotUpdate = false;
FEND;
}

void CVectorWindow::testOverlayDataRange( double minRange, double maxRange )
{
FBEGIN;
  doNotUpdate = true;
  if( dOverlayLut[0] != static_cast<int>( minRange )
    || dOverlayLut[1] != static_cast<int>( maxRange ) )
  {
    dOverlayLut[0] = minRange;
    dOverlayLut[1] = maxRange;
    double value = static_cast<double>( theOverlayDataMin->value() );
    dOverlayClampValues[0] = value;
    theOverlayMinDisplay->setText( QString::number( value ) );
    theOverlayDataMin->setValue( static_cast<int>( value / ( dOverlayLut[1] - dOverlayLut[0] ) * 1000.0 ) );
    value = static_cast<double>( theOverlayDataMax->value() );
    dOverlayClampValues[1] = value;
    theOverlayMaxDisplay->setText( QString::number( value ) );
    theOverlayDataMax->setValue( static_cast<int>( value / ( dOverlayLut[1] - dOverlayLut[0] ) * 1000.0 ) );
  }  
  doNotUpdate = false;
FEND;
}

CVectorWindow::~CVectorWindow()
{
  theImageLookupTable->Delete();
  theImage->Delete();
  theImageColorBar->Delete();
  theImageColorsMapper->Delete();
  theOverlayLookupTable->Delete();
  theOverlay->Delete();
  theOverlayColorBar->Delete();
//  theOverlayColorsMapper->Delete();
  renderer->Delete();  
  interactor->Delete();
  display->Delete();
}

void CVectorWindow::resizeEvent( QResizeEvent* e ) throw()
{
  aGlobalColumnPtr->setFixedSize( e->size() );
  //displayPtr->
}

void CVectorWindow::setImage( vtkImageData* anImage )
{
 FBEGIN;
  doNotUpdate = true;
  theImageLookupTable->SetTableRange( dImageClampValues );
  theImageColorsMapper->SetInput( anImage );
  if ( theActualImage )
    theActualImage->Delete();
  theActualImage = anImage;
  doNotUpdate = false;
  interactor->Render();
 FEND;
}

void CVectorWindow::setOverlay( vtkStructuredGrid* anOverlay )
{
 FBEGIN;
  doNotUpdate = true;
  theOverlayLookupTable->SetTableRange( dOverlayClampValues );
//  theOverlayColorsMapper->SetInput( anOverlay );
  if ( theActualOverlay )
    theActualOverlay->Delete();
  theActualOverlay = anOverlay;
  doNotUpdate = false;
  interactor->Render();
 FEND;
}

#endif /* USE_RENDERING */
