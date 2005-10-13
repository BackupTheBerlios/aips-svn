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
#include "cdisplayvectordialog.h"
#include <cvtkadapter.h>
#include <vtkCamera.h>

using namespace std;

CDisplayVectorDialog::CDisplayVectorDialog() throw()
 : aips::CModuleDialog(), width(0), height(0)
{
#ifdef USE_RENDERING
  displayPtr = new CVectorWindow();
#else
  displayPtr = new CImageDisplay();
#endif
  displayPtr->hide();
  displayPtr->setCaption( "2D vector viewer" );
}


CDisplayVectorDialog::~CDisplayVectorDialog() throw()
{
  delete displayPtr;
}

/** \returns true */
bool CDisplayVectorDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CDisplayVectorDialog::getDialogHandle() const throw( NotPresentException )
{
  return displayPtr;
}

/** \param sCaption the new window caption */
void CDisplayVectorDialog::setCaption ( const std::string sCaption ) throw()
{
  displayPtr->setCaption( sCaption.c_str() );
}

void CDisplayVectorDialog::showDialog() throw( NotPresentException )
{
  displayPtr->show();
}

void CDisplayVectorDialog::hideDialog() throw( NotPresentException )
{
  displayPtr->hide();
}

bool CDisplayVectorDialog::isHidden() throw( NotPresentException )
{
  return displayPtr->isHidden();
}

void CDisplayVectorDialog::updateView( TImage* imagePtr, TField2D* fieldPtr ) throw()
{
BENCHSTART;
FBEGIN;
#ifdef USE_RENDERING
  if ( !fieldPtr || !imagePtr )
    return;
  boost::shared_ptr<TImage> myImg ( imagePtr );
  CVTKAdapter myAdapter( myImg );
  vtkImageData* myImage = myAdapter.convertToExternal();
  vtkStructuredGrid* myField = vtkStructuredGrid::New();
  int dims[3];
  dims[0] = imagePtr->getExtent(0);
  dims[1] = imagePtr->getExtent(1);
  dims[2] = 1;
  myField->SetDimensions(dims);
  vtkDoubleArray *vectors = vtkDoubleArray::New();
  vtkPoints *points = vtkPoints::New();
  points->Allocate(dims[0]*dims[1]);
  vectors->SetNumberOfComponents(3);
  vectors->SetNumberOfTuples(dims[0]*dims[1]);
  size_t count = 0;
  double x[3] = {1.0,1.0,0.0};
  uint dx = 0; uint dy = 0;
  for ( TField2D::iterator it = fieldPtr->begin(); it != fieldPtr->end(); ++it, ++count )
  {
    x[0] = static_cast<double>(dx); ++dx;
    if ( dx > fieldPtr->getExtent(0) )
      {dy++;dx=0;}
    x[1] = dy;
/*    if ( (*it)[0] > 0.0 )
      cerr << count << ": Setting " << x[0] << ";" << x[1] << " to " << (*it) << endl;*/
    double v[3];
    v[0] = (*it)[0];
    v[1] = (*it)[1];
    v[2] = 0.0;
    vectors->InsertTuple(count,v);
    points->InsertPoint(count,x);
  }
  myField->SetPoints(points);
  points->Delete();
  myField->GetPointData()->SetVectors(vectors);
  vectors->Delete();
  myImage->SetScalarTypeToShort();
//   displayPtr->testImageDataRange( imagePtr->getMinimum(), imagePtr->getMaximum() );
//   displayPtr->setImage( myImage );
  displayPtr->testOverlayDataRange( 0.0, 1.0 );
  displayPtr->setOverlay( myField );
  if ( width != imagePtr->getExtent(0) || height != imagePtr->getExtent(1) )
  {
    width = imagePtr->getExtent(0);
    height = imagePtr->getExtent(1);
    displayPtr->getImage()->SetDisplayExtent(0,imagePtr->getExtent(0)-1,0,imagePtr->getExtent(1)-1,0,0);
    displayPtr->getRenderer()->GetActiveCamera()->SetPosition(
      static_cast<double>( imagePtr->getExtent(0) ) / 2.0, static_cast<double>( imagePtr->getExtent(1) ) / 2.0,
      0.5 * static_cast<double>( std::max( imagePtr->getExtent(0), imagePtr->getExtent(1) ) ) );
    displayPtr->getRenderer()->GetActiveCamera()->SetFocalPoint(
      static_cast<double>( imagePtr->getExtent(0) ) / 2.0, static_cast<double>( imagePtr->getExtent(1) ) / 2.0 , 0.0 );
    displayPtr->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
    displayPtr->getRenderer()->GetActiveCamera()->SetViewUp(0.0,-1.0,0.0);
    displayPtr->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
    displayPtr->getRenderer()->ResetCamera();
  }
  displayPtr->update();
  myField->Delete();
#else  
  // Draw the zoomed image
  ushort w = fieldPtr->getExtent(0);
  ushort h = fieldPtr->getExtent(1);
  bufferPixmap.resize( w * 7, h * 7 );
  QPainter p( &bufferPixmap ); // Initialize painter to display the vector field
  if ( imagePtr && imagePtr->getExtent(0) > 0 && imagePtr->getExtent(1) > 0 )
  {
    p.scale( 7, 7 );
    QImage tmpImage;  
    tmpImage.create( imagePtr->getExtent(0), imagePtr->getExtent(1), 32, 256*256*256 );
    float intRange = 1.0 / static_cast<float>
      ( imagePtr->getMaximum() - imagePtr->getMinimum() + 1 ) * 256.0;    
    for ( uint x = 0; x < imagePtr->getExtent(0); x++ )
      for ( uint y = 0; y < imagePtr->getExtent(1); y++ )
      {
        if ( imagePtr->getDataDimension() == 1 )
        {
          ushort value = static_cast<int>( static_cast<float>
            ( (*imagePtr)(x,y) - imagePtr->getMinimum() ) * intRange );
          if ( value < 256 )
            tmpImage.setPixel( x, imagePtr->getExtent(1) - 1 - y, qRgb( value, value, value ) );
#ifdef DEBUG
          else
          {
            alog << LWARN << "\n **** Pixel value " << value << " too high ("
              << imagePtr->getMaximum() << ") ****" << endl;
          }
#endif					
        }
        else if ( imagePtr->getDataDimension() == 3 )
          tmpImage.setPixel( x, y, qRgb(
            (*imagePtr)(x,imagePtr->getExtent(1) - 1 - y,0), (*imagePtr)(x,imagePtr->getExtent(1) - 1 - y,1), (*imagePtr)(x,imagePtr->getExtent(1) - 1 - y,2) ) );
        else if ( imagePtr->getDataDimension() == 4 )
          tmpImage.setPixel( x, y, qRgba(
            (*imagePtr)(x,imagePtr->getExtent(1) - 1 - y,0), (*imagePtr)(x,imagePtr->getExtent(1) - 1 - y,1), (*imagePtr)(x,imagePtr->getExtent(1) - 1 - y,2), (*imagePtr)(x,imagePtr->getExtent(1) - 1 - y,3) ) );    
 
      }  
    p.drawImage( 0, 0, tmpImage );
    p.scale( 1.0/7.0, 1.0/7.0);
  }
  else // Just clear the window
  {
    p.eraseRect( 0, 0, fieldPtr->getExtent(0) * 7, fieldPtr->getExtent(1) * 7 );
  }
  // Draw the vector field
  for ( uint x = 0; x < fieldPtr->getExtent(0); x++ )
    for ( uint y = 0; y < fieldPtr->getExtent(1); y++ )
    {      
      double val = norm( (*fieldPtr)(x,imagePtr->getExtent(1) - 1 - y) );
      if ( val > 0.0 )
      {
        int v = static_cast<int>( val * 2550.0 );
        int v2 = 0; int v3 = 0;
        if ( v > 255 )
        {
          v2 = v - 255;
          if ( v2 > 255 )
            v3 = v2 - 255;
          v = std::min( v, 255 );
          v2 = std::min( v2, 255 );
          v3 = std::min( v3, 255 );
        }
        if ( v > 240 && v2 > 240 && v3 > 240 ) // Very large vectors are shown in green
          p.setPen( Qt::green );
        else if ( v < 10 && v2 < 10 && v3 < 10 ) // Very small vectors are shown in blue
          p.setPen( Qt::blue );        
        else
          p.setPen( QColor( v, v2, v3 ) );
				ushort ycoord = imagePtr->getExtent(1) - 1 - y;
        TVector2D draw ( (*fieldPtr)(x,ycoord) );
        draw *= ( 3.0 / val );
        p.drawLine( ( x * 7 ) + 3, ( y * 7 ) + 3,
          ( x * 7 ) + 3 + static_cast<int>( draw[0] ),
          ( y * 7 ) + 3 - static_cast<int>( draw[1] ) );
        p.setPen( Qt::blue );
        p.drawPoint( ( x * 7 ) + 3, ( y * 7 ) + 3 );
      }
    }
  displayPtr->setImage( bufferPixmap.convertToImage() );
  p.flush();
  p.end();  
#endif
BENCHSTOP;
}

void CDisplayVectorDialog::activateDialog()	throw( NotPresentException )
{
	displayPtr->setActiveWindow();
}
