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
#include "cviewhistogramdialog.h"

using namespace std;

CViewHistogramDialog::CViewHistogramDialog() throw()
 : CModuleDialog()
{
	displayPtr = new QWidget();
  displayPtr->setCaption( "Histogram viewer" );
  displayPtr->setFixedSize(520,300);
  displayPtr->hide();
  aColumnPtr = new QVBox( displayPtr );
  aColumnPtr->setFixedSize(520,300);
  aColumnPtr->setMargin(4);

  histViewPtr = new QLabel ( aColumnPtr );
  aRowPtr = new QHBox( aColumnPtr );
  aRowPtr->setFixedSize(520,30);
  aRowPtr->setMargin(3);

  frontPtr = new QLCDNumber( 5, aRowPtr );
  frontPtr->setFixedWidth(50);
  scrollerPtr = new QScrollBar( 0, 0, 10, 50, 0, Qt::Horizontal, aRowPtr );
  scrollerPtr->setFixedWidth(400);
  backPtr = new QLCDNumber( 5, aRowPtr );
  backPtr->setFixedWidth(50);
	
  connect( scrollerPtr, SIGNAL( valueChanged(int) ),
   this, SLOT( updateShownRange(int) ) );
}


CViewHistogramDialog::~CViewHistogramDialog() throw()
{
  delete displayPtr;
}

/** \returns true */
bool CViewHistogramDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CViewHistogramDialog::getDialogHandle() const throw( NotPresentException )
{
  return displayPtr;
}

/** \param sCaption the new window caption */
void CViewHistogramDialog::setCaption ( const std::string sCaption ) throw()
{
  displayPtr->setCaption( sCaption.c_str() );
}

void CViewHistogramDialog::showDialog() throw( NotPresentException )
{
  displayPtr->show();
}

void CViewHistogramDialog::hideDialog() throw( NotPresentException )
{
  displayPtr->hide();
}

bool CViewHistogramDialog::isHidden() throw( NotPresentException )
{
  return displayPtr->isHidden();
}

void CViewHistogramDialog::activateDialog()	throw( NotPresentException )
{
	displayPtr->setActiveWindow();
}

void CViewHistogramDialog::updateView() throw()
{
BENCHSTART;
	alog << "A" << endl;
  histImage.create( usRangeEnd - usRangeStart + 1, 201, 32, 256*256*256 );
  histImage.fill( Qt::white.rgb() );
  histViewPtr->setFixedWidth( 500 );
  QPixmap processedPixmap;
  processedPixmap.convertFromImage( histImage );  	

  QPainter p( &processedPixmap );

	alog << "Histosize is " << histoVec.size() << "   " << usRangeStart << "   " << usRangeEnd << endl;
	alog << "Histosize is " << histoVec[0].size() << "   " << usRangeStart << "   " << usRangeEnd << endl;
  for ( ushort usRange = usRangeStart; usRange <= usRangeEnd; usRange++ )
  {      
      p.drawLine( usRange - usRangeStart, 200, usRange - usRangeStart,
        200 - static_cast<ushort>( histoVec[0][usRange] * ( 200.0 / dMaxValue ) ) );
  }
  p.setPen( Qt::black );
  p.drawLine( 0, 200, usRangeEnd - usRangeStart + 1, 200 );
  p.end();
  histViewPtr->setEraseColor( Qt::gray );
  histViewPtr->setPixmap( processedPixmap );
BENCHSTOP;
}

void CViewHistogramDialog::updateShownRange( int iRange ) throw()
{
FBEGIN;
  usRangeStart = static_cast<ushort>( iRange );
  usRangeEnd = static_cast<ushort>( iRange ) + 499;
  if ( usRangeEnd > usMaxRange )
    usRangeEnd = usMaxRange;
  frontPtr->display( usRangeStart );
  backPtr->display( usRangeEnd );
  updateView();
FEND;	
}

void CViewHistogramDialog::newImage( ushort maxRange, int iValue, double dMax, 
	std::vector<std::vector<double> >& histVec )
{
FBEGIN;
	usMaxRange = maxRange;
	dMaxValue = dMax;
	histoVec = histVec;
  scrollerPtr->setMaxValue( iValue );	
  updateShownRange(0);
FEND;	
}
