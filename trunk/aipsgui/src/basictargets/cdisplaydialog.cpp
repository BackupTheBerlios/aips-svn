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

using namespace std;

CDisplayDialog::CDisplayDialog() throw()
 : aips::CModuleDialog()
{
  displayPtr = new CImageDisplay();
  displayPtr->hide();
  displayPtr->setCaption( "2D image viewer" );
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

void CDisplayDialog::updateView( TImage* inputPtr ) throw()
{
BENCHSTART;
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
    static_cast<float>( inputPtr->getMaximum() - inputPtr->getMinimum() + 1 ) * 256.0;
	ushort usMinimum = inputPtr->getMinimum();
  if ( inputPtr->getMaximum() < 2 ) 
	{
		fIntensityRange = 255.0;
		usMinimum = 0;
	}
  for ( ushort x = 0; x < inputPtr->getExtent(0); ++x )
    for ( ushort y = 0; y < inputPtr->getExtent(1); ++y )
    {
      if ( inputPtr->getDataDimension() == 1 )
      {
        ushort value = static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y ) 
					- usMinimum ) * fIntensityRange );
        if ( value < 256 ) processed.setPixel( x, y, value );
#ifdef DEBUG				
        else
        {
          alog << LWARN << "\n **** Pixel value " << (*inputPtr)( x, y ) << "/" << value 
					<< " too high (" << inputPtr->getMaximum() << "/" << usMinimum << ") ****" 
					<< " fInten " << fIntensityRange << endl;
        }
#endif				
      }
      else if ( inputPtr->getDataDimension() == 2 )
        processed.setPixel( x, y, qRgb(	static_cast<ushort>( static_cast<float>( 
					(*inputPtr)( x, y, 0 ) - inputPtr->getMinimum() ) * fIntensityRange ),
					static_cast<ushort>( static_cast<float>( (*inputPtr)( x, y, 1 ) - inputPtr->getMinimum() ) * fIntensityRange ), 0 ) );
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
  displayPtr->setImage( processed );
BENCHSTOP;
}

void CDisplayDialog::updateView( TField* inputPtr ) throw()
{
BENCHSTART;
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
  displayPtr->setImage( processed );
BENCHSTOP;
}

void CDisplayDialog::activateDialog()	throw( NotPresentException )
{
	displayPtr->setActiveWindow();
}
