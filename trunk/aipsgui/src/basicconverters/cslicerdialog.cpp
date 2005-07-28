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
#include "cslicerdialog.h"

CSlicerDialog::CSlicerDialog( CSliceSelector* parent_ ) throw() : parent( parent_ )
{
	theDisplay = new QWidget();  
  theDisplay->hide();
  theDisplay->setFixedSize(200,160);
  aColumn = new QVBox( theDisplay );
  aColumn->setFixedSize(200,160);
  aColumn->setMargin(8);

  sliceSelector = new QScrollBar( 0, 180, 1, 10, 60, Qt::Horizontal, aColumn );
  sliceNumberView = new QLCDNumber( 3, aColumn );
  orientationChooser = new QComboBox( aColumn );

  sliceSelector->setMinimumWidth( 100 );
  sliceSelector->setValue( 0 );
  sliceSelector->setRange(0, 0);
  sliceNumberView->setFixedHeight( 80 );
  sliceNumberView->setFixedWidth( 80 );
  sliceNumberView->display( 0 );
  orientationChooser->insertItem( "Transversal( xy )" );
  orientationChooser->insertItem( "Coronal( xz )" );
  orientationChooser->insertItem( "Saggital( yz )" );

  connect( orientationChooser, SIGNAL( activated( int ) ),
    this, SLOT( updateOrientation( int ) ) );
  connect ( sliceSelector, SIGNAL( valueChanged( int ) ),
    sliceNumberView, SLOT( display( int ) ) );
  connect ( sliceSelector, SIGNAL( valueChanged( int ) ),
    this, SLOT( updateSlice( int ) ) );
}

CSlicerDialog::~CSlicerDialog() throw()
{
	delete theDisplay;
}

void CSlicerDialog::updateSlice( int i )
{
DS("Update slice to " << i)	
	notify( boost::shared_ptr<CSliceChangedEvent>(new CSliceChangedEvent( this, i ) ) );
}

void CSlicerDialog::updateOrientation( int i )
{
	notify( boost::shared_ptr<COrientationChangedEvent>(new COrientationChangedEvent( this, i ) ) );
}

void CSlicerDialog::hideGUIElements()
{
	orientationChooser->hide();
  sliceSelector->hide();
}

void CSlicerDialog::showGUIElements()
{
	orientationChooser->show();
  sliceSelector->show();
}

void CSlicerDialog::setNumberDisplay( int i )
{
	sliceNumberView->display( i );
}

void CSlicerDialog::setSlicerMax( int i )
{
	sliceSelector->setRange( 0, i );
}

void CSlicerDialog::setSlicerValue( int i )
{
	sliceSelector->setValue( i );
}

void CSlicerDialog::setOrientation( int i )
{
	orientationChooser->setCurrentItem( i );
}

/* Dialog functions */

bool CSlicerDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CSlicerDialog::getDialogHandle() const throw( NotPresentException )
{
  return theDisplay;
}

void CSlicerDialog::setCaption ( const std::string sCaption ) throw( NotPresentException )
{
  theDisplay->setCaption( sCaption.c_str() );
}

void CSlicerDialog::showDialog() throw( NotPresentException )
{
  theDisplay->show();
}

void CSlicerDialog::hideDialog() throw( NotPresentException )
{
  theDisplay->hide();
}

bool CSlicerDialog::isHidden() throw( NotPresentException )
{
  return theDisplay->isHidden();
}

void CSlicerDialog::activateDialog()	throw( NotPresentException )
{
	theDisplay->setActiveWindow();
}

//#include "cslicerdialog.moc"
