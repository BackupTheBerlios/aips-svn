/************************************************************************
 * File: cimagedisplay.cpp                                              *
 * Project: AIPS                                                        *
 * Description: A display class for 2D images, used by CDisplay         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 28.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cimagedisplay.h"

using namespace aips;
using namespace std;

/*****************
 * CImageDisplay *
 *****************/

/** Constructor */
CImageDisplay::CImageDisplay() throw()
  : QWidget(), CBase( "CImageDisplay", "0.2", "CBase,QWidget" )
{
  setMinimumSize(160,200);
  aColumnPtr = new QVBox( this );
  aColumnPtr->setGeometry(0,0,width(),height());
  aColumnPtr->setMargin(8);

  processedPicPtr = new QScrollView ( aColumnPtr );
  processedPicPtr->setHScrollBarMode( QScrollView::Auto );
  processedPicPtr->setVScrollBarMode( QScrollView::Auto );
  imageLabelPtr = new QLabel(NULL);
  processedPicPtr->addChild( imageLabelPtr );
  aRowPtr = new QHBox( aColumnPtr );
  zoomBoxLabelPtr = new QLabel( "Zoom factor: ", aRowPtr );
  zoomFactorBoxPtr = new QComboBox ( aRowPtr );
  zoomFactorBoxPtr->insertItem( " 10%", 0 );
  zoomFactorBoxPtr->insertItem( " 25%", 1 );
  zoomFactorBoxPtr->insertItem( " 50%", 2 );
  zoomFactorBoxPtr->insertItem( " 75%", 3 );
  zoomFactorBoxPtr->insertItem( "100%", 4 );
  zoomFactorBoxPtr->insertItem( "200%", 5 );
  zoomFactorBoxPtr->insertItem( "300%", 6 );
  zoomFactorBoxPtr->insertItem( "400%", 7 );
  zoomFactorBoxPtr->setCurrentItem( 4 );
  colorMapPtr = new QCheckBox ( "Use color map", aColumnPtr );
  colorMapPtr->setChecked( false );

  paletteVec.resize(256);
  paletteVec[0].setRgb( 0, 0, 0 );
  for ( uint i = 1; i < 256; ++i )
  {
    paletteVec[i].setHsv( ( i * 360 ) / 256, 255, 255 );
  }

  connect ( zoomFactorBoxPtr, SIGNAL( activated(int) ),
    this, SLOT( updateView() ) );
  connect ( colorMapPtr, SIGNAL( clicked() ),
    this, SLOT( updateView() ) );
}

/** Destructor */
CImageDisplay::~CImageDisplay() throw()
{
  paletteVec.clear();
}

/** Reimplemented from QWidget */
void CImageDisplay::resizeEvent( QResizeEvent* e ) throw()
{
  aColumnPtr->setFixedSize( e->size() );
}

/**
 * Set the image palette
 * \param paletteVec_ the new image palette
 */
void CImageDisplay::setImagePalette( vector<QColor> paletteVec_ )
  throw()
{
  paletteVec.clear();
  paletteVec = paletteVec_;
}

/**
 * Set the image that should be displayed
 * \param processedImage image to show
 * \param dataDimension number of image channels
 */
void CImageDisplay::setImage( const QImage& processedImage ) throw()
{
  image = processedImage;
  if ( image.numColors() == 0 || image.numColors() > 256 )  
  {
    colorMapPtr->hide();
    colorMapPtr->setChecked( false );
  }
  else
  {
    colorMapPtr->show();
  }
  updateView();
}

/** Reimplemented from QWidget */
QSize CImageDisplay::sizeHint() const throw()
{
  return QSize( 160, 200 );
}

void CImageDisplay::updateView()
{
  QImage processedImage;
  int iWidth = image.width();
  int iHeight = image.height();

  double factor = 1.0;
  if ( zoomFactorBoxPtr->currentText() == " 10%" )
    factor = 0.1;
  else if ( zoomFactorBoxPtr->currentText() == " 25%" )
    factor = 0.25;
  else if ( zoomFactorBoxPtr->currentText() == " 50%" )
    factor = 0.5;
  else if ( zoomFactorBoxPtr->currentText() == " 75%" )
    factor = 0.75;
  else if ( zoomFactorBoxPtr->currentText() == "200%" )
    factor = 2.0;
  else if ( zoomFactorBoxPtr->currentText() == "300%" )
    factor = 3.0;
  else if ( zoomFactorBoxPtr->currentText() == "400%" )
    factor = 4.0;

  iWidth = static_cast<int>( static_cast<double>(iWidth) * factor );
  iHeight = static_cast<int>( static_cast<double>(iHeight) * factor );
  if ( factor != 1.0 ) processedImage = image.scale( iWidth, iHeight );
  else processedImage = image;

  if ( !( image.numColors() == 0 || image.numColors() > 256 ) )
    if ( colorMapPtr->isChecked() )
      for ( int i = 0; i < 256; i++ )
        processedImage.setColor( i, paletteVec[i].rgb() );
    else
      for ( int i = 0; i < 256; i++ )
      {
        QColor tmpColor( i, i, i );
        processedImage.setColor( i, tmpColor.rgb() );
      }

  QPixmap processedPixmap;
  processedPixmap.convertFromImage( processedImage );
  imageLabelPtr->setEraseColor( Qt::black );
  imageLabelPtr->setFixedSize( iWidth, iHeight );
  imageLabelPtr->setPixmap( processedPixmap );
  if ( iWidth > 256 ) iWidth = 256;
  if ( iHeight > 256 ) iHeight = 256;
  if ( iWidth + 21 > 80 ) setMinimumWidth( iWidth + 21 );
  if ( iHeight + 70 > 80 ) setMinimumHeight( iHeight + 70 );
}
