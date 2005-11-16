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

#ifdef USE_RENDERING
#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>
#endif

using namespace std;



CDisplayDialog::CDisplayDialog() throw()
 : aips::CModuleDialog(), width(0), height(0)
{
#ifdef USE_RENDERING
  displayPtr = new CDisplayWindow();
#else
  displayPtr = new CImageDisplay();
#endif  
  displayPtr->hide();
  displayPtr->setCaption( "2D image viewer" );
#ifdef USE_RENDERING
  displayPtr->getRenderer()->TwoSidedLightingOn();
  displayPtr->getRenderer()->SetAmbient( 1.0, 1.0, 1.0 );  
  displayPtr->getRenderer()->GetActiveCamera()->SetPosition(128.0,128.0,-256.0);
  displayPtr->getRenderer()->GetActiveCamera()->SetFocalPoint(128.0,128.0,0.0);
  displayPtr->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
  displayPtr->getRenderer()->GetActiveCamera()->SetViewUp(0.0,-1.0,0.0);
  displayPtr->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
  displayPtr->getRenderer()->ResetCamera();
#endif  
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

void CDisplayDialog::updateView( TImagePtr inputPtr, bool bImage ) throw()
{
BENCHSTART;
FBEGIN;
#ifdef USE_RENDERING
  if ( !inputPtr )
    return;  
  CVTKAdapter myAdapter( inputPtr );
  vtkImageData* myImage = myAdapter.convertToExternal();
  myImage->SetScalarTypeToShort();
  if ( bImage )
  {
    displayPtr->testImageDataRange( inputPtr->getMinimum(), inputPtr->getMaximum() );
    displayPtr->setImage( myImage );  
  }
  else
  {
    displayPtr->testOverlayDataRange( inputPtr->getMinimum(), inputPtr->getMaximum() );
    displayPtr->setOverlay( myImage );
  }
  if ( width != inputPtr->getExtent(0) || height != inputPtr->getExtent(1) )
  {
    width = inputPtr->getExtent(0);
    height = inputPtr->getExtent(1);
    if ( bImage )
      displayPtr->getImage()->SetDisplayExtent(0,inputPtr->getExtent(0)-1,0,inputPtr->getExtent(1)-1,0,0);
    else
      displayPtr->getOverlay()->SetDisplayExtent(0,inputPtr->getExtent(0)-1,0,inputPtr->getExtent(1)-1,0,0);
    displayPtr->getRenderer()->GetActiveCamera()->SetPosition(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0,
      0.5 * static_cast<double>( std::max( inputPtr->getExtent(0), inputPtr->getExtent(1) ) ) );
    displayPtr->getRenderer()->GetActiveCamera()->SetFocalPoint(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0 , 0.0 );
    displayPtr->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
    displayPtr->getRenderer()->GetActiveCamera()->SetViewUp(0.0,-1.0,0.0);
    displayPtr->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
    displayPtr->getRenderer()->ResetCamera();
  }
  displayPtr->update();
#else
  QImage processed;
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
    static_cast<float>( inputPtr->getDataRange().getMaximum() - inputPtr->getDataRange().getMinimum() + 1 ) * 256.0;
  double dMinimum = inputPtr->getDataRange().getMinimum();
  if ( inputPtr->getDataRange().getMaximum() < 2 ) 
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
      }
      else if ( inputPtr->getDataDimension() == 2 )
        processed.setPixel( x, y, qRgb( static_cast<ushort>( static_cast<float>( 
          (*inputPtr)( x, y, 0 ) - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
          static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ), 0 ) );
      else if ( inputPtr->getDataDimension() == 3 )
        processed.setPixel( x, y, qRgb( 
        static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 0 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
          static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
          static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 2 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ) ) );
      else if ( inputPtr->getDataDimension() == 4 )
        processed.setPixel( x, y, qRgba( 
          static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 0 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
          static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
          static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 2 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ), 
          static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 3 ) 
          - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ) ) );
    }
  displayPtr->setImage( processed );
#endif  
FEND;  
BENCHSTOP;
}

void CDisplayDialog::updateView( TFieldPtr inputPtr, bool bImage ) throw()
{
BENCHSTART;
FBEGIN;
#ifdef USE_RENDERING
  CVTKAdapter myAdapter( inputPtr );
  vtkImageData* myImage = myAdapter.convertToExternal();
  myImage->SetScalarTypeToDouble();
  if ( bImage )
  {
    displayPtr->testImageDataRange( inputPtr->getMinimum(), inputPtr->getMaximum() );
    displayPtr->setImage( myImage );
  }
  else
  {
    displayPtr->testOverlayDataRange( inputPtr->getMinimum(), inputPtr->getMaximum() );
    displayPtr->setOverlay( myImage );
  }
  if ( width != inputPtr->getExtent(0) || height != inputPtr->getExtent(1) )
  {
    width = inputPtr->getExtent(0);
    height = inputPtr->getExtent(1);
    if ( bImage )
      displayPtr->getImage()->SetDisplayExtent(0,inputPtr->getExtent(0)-1,0,inputPtr->getExtent(1)-1,0,0);
    else
      displayPtr->getOverlay()->SetDisplayExtent(0,inputPtr->getExtent(0)-1,0,inputPtr->getExtent(1)-1,0,0);
    displayPtr->getRenderer()->GetActiveCamera()->SetPosition(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0,
      0.5 * static_cast<double>( std::max( inputPtr->getExtent(0), inputPtr->getExtent(1) ) ) );
    displayPtr->getRenderer()->GetActiveCamera()->SetFocalPoint(
      static_cast<double>( inputPtr->getExtent(0) ) / 2.0, static_cast<double>( inputPtr->getExtent(1) ) / 2.0 , 0.0 );
    displayPtr->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
    displayPtr->getRenderer()->GetActiveCamera()->SetViewUp(0.0,-1.0,0.0);
    displayPtr->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
    displayPtr->getRenderer()->ResetCamera();
  }
  displayPtr->update();
#else
  QImage processed;
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
    static_cast<float>( inputPtr->getDataRange().getMaximum() - inputPtr->getDataRange().getMinimum() + 1 ) * 256.0;
	double dMinimum = inputPtr->getDataRange().getMinimum();
  if ( inputPtr->getDataRange().getMaximum() < 2 ) 
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
      }
      else if ( inputPtr->getDataDimension() == 2 )
        processed.setPixel( x, y, qRgb(	static_cast<ushort>( static_cast<float>( 
					(*inputPtr)( x, y, 0 ) - inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ), 0 ) );
      else if ( inputPtr->getDataDimension() == 3 )
        processed.setPixel( x, y, qRgb( 
				static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 0 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 2 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ) ) );
      else if ( inputPtr->getDataDimension() == 4 )
        processed.setPixel( x, y, qRgba( 
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 0 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 2 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ), 
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 3 ) 
					- inputPtr->getDataRange().getMinimum() ) * fIntensityRange ) ) );
    }
  displayPtr->setImage( processed );
#endif  
BENCHSTOP;
}

void CDisplayDialog::activateDialog()	throw( NotPresentException )
{
	displayPtr->setActiveWindow();
}
