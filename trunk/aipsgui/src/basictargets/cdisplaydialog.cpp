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

#include <vtkImageMapToColors.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>

using namespace std;

CDisplayWindow::CDisplayWindow() : lutmin(0),lutmax(255)
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

  vtkLookupTable *theLut = vtkLookupTable::New();
  theLut->SetTableRange (0, 255 );
  theLut->SetSaturationRange (0, 0);
  theLut->SetHueRange (0, 0);
  theLut->SetValueRange (0, 1);
  theLut->Build(); //effective built

  doc1 = new QLabel( "Data range minimum", aColumnPtr );
  dataMin = new QScrollBar ( 0, 255, 1, 10, 255, Qt::Horizontal, aColumnPtr );
  doc2 = new QLabel( "Data range maximum", aColumnPtr );
  dataMax = new QScrollBar ( 0, 0, 1, 10, 0, Qt::Horizontal, aColumnPtr );
  QHBox* aRow1Ptr = new QHBox ( aColumnPtr );
  transparency = new QCheckBox( aRow1Ptr );
  doc3 = new QLabel( "Background transparency", aRow1Ptr );
  QHBox* aRow2Ptr = new QHBox ( aColumnPtr );
  interpolate = new QCheckBox( aRow2Ptr );
  doc4 = new QLabel( "Interpolate", aRow2Ptr );
  connect ( dataMin, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateMin( int ) ) );
  connect ( dataMax, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateMax( int ) ) );
}

void CDisplayWindow::updateMax( int i )
{
  setNewLutValues( lutmin, i );
}

void CDisplayWindow::updateMin( int i ) 
{
  setNewLutValues( i, lutmax );  
}

void CDisplayWindow::update()
{
  interactor->Render();
}

vtkRenderer* CDisplayWindow::getRenderer()
{
  return renderer;
}

void CDisplayWindow::setNewLutValues( double min, double max )
{
  cerr << "Setting lut from " << lutmin << " / " << lutmax << " to " << min << " / " << max << endl;
//   lutmin = min;
//   lutmax = max;
//   theLut->SetTableRange( min, max );
//   theLut->SetSaturationRange (0, 0);
//   theLut->SetHueRange (0, 0);
//   theLut->SetValueRange (0, 1);
//   theLut->Build();
  interactor->Render();
}

vtkLookupTable* CDisplayWindow::getLut()
{
  return theLut;
}

CDisplayWindow::~CDisplayWindow()
{
  theLut->Delete();
  delete interactor;
  delete display;
  renderer->Delete();
}

void CDisplayWindow::resizeEvent( QResizeEvent* e ) throw()
{
  aColumnPtr->setFixedSize( e->size() );
  //displayPtr->
}

CDisplayDialog::CDisplayDialog() throw()
 : aips::CModuleDialog(), width(0), height(0)
{
  displayPtr = new CDisplayWindow();
  displayPtr->hide();
  displayPtr->setCaption( "2D image viewer" );
  myActor = vtkImageActor::New();
//   myCast = vtkImageCast::New();
//   myActor->SetInput( myCast->GetOutput() );
  displayPtr->getRenderer()->AddProp( myActor );
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
  if ( !inputPtr )
    return;
  cerr << "BLA" << endl;
  CVTKAdapter myAdapter( inputPtr );
  vtkImageData* myImage = myAdapter.convertToExternal();
  int* dims = myImage->GetDimensions();
  cerr << dims[0] << " x " << dims[1] << " x " << dims[2] << " - " << myImage->GetNumberOfCells() << endl;
  myImage->SetScalarTypeToUnsignedShort();
  // Start by creatin a black/white lookup table.
  displayPtr->setNewLutValues( inputPtr->getMinimum(), inputPtr->getMaximum() );  
  vtkImageMapToColors *saggitalColors = vtkImageMapToColors::New();
  saggitalColors->SetInput(myImage);
  saggitalColors->SetLookupTable( displayPtr->getLut() );
  myActor->SetInput(saggitalColors->GetOutput());
  myActor->SetDisplayExtent(0,inputPtr->getExtent(0)-1,0,inputPtr->getExtent(1)-1,0,0);
  if ( width != inputPtr->getExtent(0) || height != inputPtr->getExtent(1) )
  {
    width = inputPtr->getExtent(0);
    height = inputPtr->getExtent(1);
    displayPtr->getRenderer()->GetActiveCamera()->SetPosition(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0,
      2.0 * static_cast<double>( std::max( inputPtr->getExtent(0), inputPtr->getExtent(1) ) ) );
    displayPtr->getRenderer()->GetActiveCamera()->SetFocalPoint(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0 , 0.0 );
    displayPtr->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
    displayPtr->getRenderer()->GetActiveCamera()->SetViewUp(0.0,-1.0,0.0);
    displayPtr->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
    displayPtr->getRenderer()->ResetCamera();
  }
  //myCast->Update();  
  displayPtr->getRenderer()->Render();
  displayPtr->update();
  saggitalColors->Delete();
BENCHSTOP;
}

void CDisplayDialog::updateView( TFieldPtr inputPtr ) throw()
{
BENCHSTART;
  CVTKAdapter myAdapter( inputPtr );
  vtkImageData* myImage = myAdapter.convertToExternal();
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
