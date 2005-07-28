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
#include "csnakedialog.h"
#include <iostream>

using namespace std;

CSnakeDialog::CSnakeDialog()throw()
{
	cerr << "CDrawerDialog::CDrawerDialog" << endl;
  theDisplay = new CImageDisplay();
  theDisplay->setCaption( "2D Particle Snake" );	
}


CSnakeDialog::~CSnakeDialog()throw()
{
  delete theDisplay;

}

/* Dialog functions */

bool CSnakeDialog::hasDialog() const throw()
{
  return true;
}

QWidget* CSnakeDialog::getDialogHandle() const throw( NotPresentException )
{
  return theDisplay;
}

void CSnakeDialog::setCaption ( const std::string sCaption ) throw( NotPresentException )
{
  theDisplay->setCaption( sCaption.c_str() );
}

void CSnakeDialog::showDialog() throw( NotPresentException )
{
  theDisplay->show();
}

void CSnakeDialog::hideDialog() throw( NotPresentException )
{
  theDisplay->hide();
}

bool CSnakeDialog::isHidden() throw( NotPresentException )
{
  return theDisplay->isHidden();
}

void CSnakeDialog::activateDialog()	throw( NotPresentException )
{
	theDisplay->setActiveWindow();
}
