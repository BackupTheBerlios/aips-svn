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
#include "cdrawerdialog.h"

using namespace std;

CDrawerDialog::CDrawerDialog( CManualSegmenter* parent_ ) throw()
 : CModuleDialog(), parent( parent_ )
{
  theDisplay = new PolyDrawer( this );
  theDisplay->setCaption( "Polyselector" );
  theDisplay->setGeometry(0,0,80,80);
  theDisplay->hide();
}


CDrawerDialog::~CDrawerDialog() throw()
{
	delete theDisplay;
}

// void CDrawerDialog::setOutput( TField2DPtr p )
// {
// 	parent->setOutput( p );
// 	parent->dataChanged( parent->getID() );
// }

bool CDrawerDialog::hasDialog() const throw() { return true; }
void CDrawerDialog::setCaption ( const std::string sCaption ) throw( NotPresentException )
{ theDisplay->setCaption( sCaption.c_str() ); }
void CDrawerDialog::showDialog() throw( NotPresentException )
{ theDisplay->show(); }
void CDrawerDialog::hideDialog()  throw( NotPresentException )
{ theDisplay->hide(); }
bool CDrawerDialog::isHidden() throw( NotPresentException )
{ return theDisplay->isHidden(); }
void CDrawerDialog::activateDialog() throw( NotPresentException )
{ return theDisplay->setActiveWindow(); }

void CDrawerDialog::so( TField2DPtr p2 )
{
	notify( shared_ptr<CDrawerChangedEvent>( new CDrawerChangedEvent( this, p2 ) ) );
}

void CDrawerDialog::updateView( TImagePtr tmp ) 
{
FBEGIN;
DBG("Input has data dimension " << tmp->getDataDimension() );

  switch( tmp->getDataDimension() )
  {
    case 1:
      processed.create( tmp->getExtent(0), tmp->getExtent(1), 8, 256 );
      break;
    case 3:
      processed.create( tmp->getExtent(0), tmp->getExtent(1), 32, 16777216 );
      break;
    case 4:
      processed.create( tmp->getExtent(0), tmp->getExtent(1), 32, 16777216 * 256 );
      break;
  }

DBG("Image has data dimension " << tmp->getDataDimension() );
  float intRange = 1.0 / static_cast<float>(tmp->getMaximum() - tmp->getMinimum() + 1) * 256.0;
  if ( tmp->getMaximum() < 2 )
    intRange = 255.0;
  else if ( ( tmp->getMaximum() - tmp->getMinimum() + 1 ) < 256 )
    intRange = 1.0;
DBG("Image intensity range is " << tmp->getMaximum() - tmp->getMinimum() + 1);
DBG("Image intensity range normalization is " << intRange);
  for ( ushort x = 0; x < tmp->getExtent(0); x++ )
    for ( ushort y = 0; y < tmp->getExtent(1); y++ )
    {
      if ( tmp->getDataDimension() == 1 )
      {
        ushort value = static_cast<ushort>(
          static_cast<float>((*tmp)(x,y) - tmp->getMinimum() ) * intRange );
        if ( value < 256 ) processed.setPixel( x, tmp->getExtent(1) - 1 - y, value );
        else
        { alog << LWARN << "\n **** Pixel value " << value << " too high (" <<
          tmp->getMaximum() << ") ****" << endl; }
      }
      else if ( tmp->getDataDimension() == 3 )
        processed.setPixel( x, tmp->getExtent(1) - 1 - y, qRgb( (*tmp)(x,y,0), (*tmp)(x,y,1), (*tmp)(x,y,2) ) );
      else if ( tmp->getDataDimension() == 4 )
        processed.setPixel( x, tmp->getExtent(1) - 1 - y, qRgba(
          (*tmp)(x,y,0), (*tmp)(x,y,1), (*tmp)(x,y,2), (*tmp)(x,y,3) ) );
    }
  theDisplay->iWidth = processed.width() * 2;
  theDisplay->iHeight = processed.height() * 2;
  processed = processed.scale( theDisplay->iWidth, theDisplay->iHeight );

  if ( tmp->getDataDimension() == 1 )
  {
    for ( int i = 0; i < 256; i++ )
    {
      QColor tmp(i,i,i);
      processed.setColor( i, tmp.rgb() );
    }
  }

  processedPixmap.convertFromImage( processed );
  theDisplay->setErasePixmap( processedPixmap );
  theDisplay->setBuffer( processedPixmap );
  theDisplay->setFixedSize( theDisplay->iWidth, theDisplay->iHeight );
FEND;
}

PolyDrawer::PolyDrawer( CDrawerDialog* father_ )
  : father( father_ )
{
}

PolyDrawer::~PolyDrawer()
{
}

void PolyDrawer::mousePressEvent( QMouseEvent* event )
{
  TVector2D v;

  if ( event->button() == LeftButton )
  {
/*    delete father->getOutput();
    father->setOutput( NULL );    */
    v[0] = event->pos().x() / 2;
    v[1] = event->pos().y() / 2;
    polygon.push_back(v);    
  }

  buffer = schwampf;
  QPainter p( &buffer );
  if ( polygon.size() > 0 )
  {
    v = polygon[0];
    v *= 2;
    p.setPen( Qt::blue );
    p.drawLine( static_cast<int>(v[0]) - 2, static_cast<int>(v[1]),
      static_cast<int>(v[0]) + 2, static_cast<int>(v[1]) );
    p.drawLine( static_cast<int>(v[0]), static_cast<int>(v[1]) - 2,
      static_cast<int>(v[0]), static_cast<int>(v[1]) + 2 );      
    p.moveTo( static_cast<int>(v[0]), static_cast<int>(v[1]) );
    for( vector<TVector2D>::iterator it = polygon.begin()+1; it != polygon.end(); ++it )
    {
      p.setPen( Qt::red );      
      p.lineTo( static_cast<int>((*it)[0]) * 2, static_cast<int>((*it)[1]) * 2);
      p.setPen( Qt::blue );
      p.drawLine( ( static_cast<int>((*it)[0]) - 2 ) * 2, static_cast<int>((*it)[1]) * 2,
        ( static_cast<int>((*it)[0]) + 2 ) * 2, static_cast<int>((*it)[1]) * 2 );
      p.drawLine( static_cast<int>((*it)[0]) * 2, ( static_cast<int>((*it)[1]) - 2 ) * 2,
        static_cast<int>((*it)[0]) * 2, ( static_cast<int>((*it)[1]) + 2 ) * 2 );
      p.moveTo( static_cast<int>((*it)[0]) * 2, static_cast<int>((*it)[1]) * 2 );        
    }
    if ( event->button() == RightButton )
    {
      p.setPen( Qt::red );
      p.lineTo( static_cast<int>(v[0]), static_cast<int>(v[1]) );
      if ( event->button() == RightButton )
      {        
        size_t extend[] = { polygon.size() };
        TField2DPtr data ( new TField2D( 1, extend ) );
        for ( uint i = 0; i < polygon.size(); ++i )
        {
          (*data)( polygon.size() - 1 - i ) =  polygon[i];
					(*data)( polygon.size() - 1 - i )[1] = (iHeight/2) - 1 - (*data)( polygon.size() - 1 - i )[1];
        }
        //father->deleteOutput();
        father->so( data );
				//father->dataChanged( father->getID() );
      }      
      polygon.clear();
    }
  }
  paintEvent( NULL );
}

void PolyDrawer::paintEvent( QPaintEvent* )
{
  bitBlt( this, 0, 0, &buffer);
}


