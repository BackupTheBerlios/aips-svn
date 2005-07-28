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
#include "cimagewriterdialog.h"

CImageWriterDialog::CImageWriterDialog( CImageWriter* parent_ ) throw()
 : CModuleDialog(), parent( parent_ )
{
	theDisplay = new QWidget();
  theDisplay->setCaption( "Image writer" );
  theDisplay->setFixedSize( 300, 200 );
  theDisplay->hide();
  aColumnPtr = new QVBox( theDisplay );
  aColumnPtr->setGeometry(0,0,300,200);
  fileNameButtonPtr = new QPushButton( "...", aColumnPtr );
/*  checkSwapEndianess = new QCheckBox( "Save as big endian", aColumnPtr );
  checkMirrorX = new QCheckBox( "Mirror on x-Axis", aColumnPtr );
  checkMirrorY = new QCheckBox( "Mirror on y-Axis", aColumnPtr );
  checkMirrorZ = new QCheckBox( "Mirror on z-Axis", aColumnPtr );*/
  aRow = new QHBox( aColumnPtr );
/*  linearShiftLabel = new QLabel( "Linear Shift", aRow );
  linearShift = new QLineEdit( "0", aRow );*/
  saveDataButtonPtr = new QPushButton( "Save", aColumnPtr );
//   checkMirrorX->setChecked( false );
//   checkMirrorY->setChecked( false );
//   checkMirrorZ->setChecked( false );
//   checkSwapEndianess->setChecked( false );

  connect( saveDataButtonPtr, SIGNAL( clicked() ),
    this, SLOT( updateData() ) );
  connect( fileNameButtonPtr, SIGNAL( clicked() ),
    this, SLOT( selectNewFile() ) );
}


CImageWriterDialog::~CImageWriterDialog() throw()
{
	delete theDisplay;
}

/* Dialog methods */

bool CImageWriterDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CImageWriterDialog::getDialogHandle() const throw( NotPresentException )
{
  return theDisplay;
}

void CImageWriterDialog::setCaption ( const std::string sCaption ) throw( NotPresentException )
{
  theDisplay->setCaption( sCaption.c_str() );
}

void CImageWriterDialog::showDialog() throw( NotPresentException )
{
  theDisplay->show();
}

void CImageWriterDialog::hideDialog() throw( NotPresentException )
{
  theDisplay->hide();
}

bool CImageWriterDialog::isHidden() throw( NotPresentException )
{
  return theDisplay->isHidden();
}

void CImageWriterDialog::selectNewFile()
{
  QString sTmp = QFileDialog::getSaveFileName( path.c_str(),
    filetypes );
  if ( sTmp == QString::null ) 
		return;
	filename = sTmp.ascii();	
	notify( shared_ptr<CFileNameChangedEvent>( new CFileNameChangedEvent( this, filename ) ) );
  fileNameButtonPtr->setText( sTmp );
}

void CImageWriterDialog::updateData()
{
  if ( filename == "" )
  {
    QMessageBox::warning( 0, "Warning", "Could not load file with no name" );
    return;
  }
	notify( shared_ptr<CLoadActivatedEvent>( new CLoadActivatedEvent( this ) ) );
}

void CImageWriterDialog::activateDialog()	throw( NotPresentException )
{
	theDisplay->setActiveWindow();
}
