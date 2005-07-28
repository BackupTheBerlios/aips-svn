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
#include "csynergeticmodeldialog.h"

CSynergeticModelDialog::CSynergeticModelDialog() throw()
 : aips::CModuleDialog()
{
  theDialog = new QLabel( NULL );
  theDialog->setCaption( "Synergetic growth" );
}


CSynergeticModelDialog::~CSynergeticModelDialog() throw()
{
	delete theDialog;
}

/* Dialog functions */

bool CSynergeticModelDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CSynergeticModelDialog::getDialogHandle() const throw( NotPresentException )
{
  return theDialog;
}

void CSynergeticModelDialog::setCaption ( const std::string sCaption ) throw( NotPresentException )
{
  theDialog->setCaption( sCaption.c_str() );
}

void CSynergeticModelDialog::showDialog() throw( NotPresentException )
{
  theDialog->show();
}

void CSynergeticModelDialog::hideDialog() throw( NotPresentException )
{
  theDialog->hide();
}

bool CSynergeticModelDialog::isHidden() throw( NotPresentException )
{
  return theDialog->isHidden();
}

void CSynergeticModelDialog::activateDialog()	throw( NotPresentException )
{
	theDialog->setActiveWindow();
}

void CSynergeticModelDialog::initImage( int w, int h )
{
	image.create( w, h, 32, 256*256*256 );
	theDialog->setFixedSize( w, h );
}

void CSynergeticModelDialog::setImage( TImagePtr inputPtr, TImagePtr outputPtr, ulong ulTTL )
{	
	for ( uint y = 0; y < inputPtr->getExtent(1); y++ )
      for ( uint x = 0; x < inputPtr->getExtent(0); x ++ )
      {
        if ( (*outputPtr)(x,y) == (255 - ulTTL) )
        {
          image.setPixel( x, inputPtr->getExtent(1) - 1 - y, qRgb( (*inputPtr)( x, y ), 255, 255 ) );
        }
        else
        {
          image.setPixel( x, inputPtr->getExtent(1) - 1 - y, qRgb( (*inputPtr)( x, y ),
            (*outputPtr)( x, y )==255?0:((*outputPtr)( x, y )-170)*3,
            (*outputPtr)( x, y )==255?255:0 ) );
        }
      }
    // Show the actual timestep
    QPixmap processedPixmap;
    processedPixmap.convertFromImage( image );
    theDialog->setPixmap( processedPixmap );
    theDialog->repaint();
}

