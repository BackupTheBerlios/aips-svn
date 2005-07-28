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
#ifndef CSNAKEDIALOG_H
#define CSNAKEDIALOG_H

#include <cmoduledialog.h>
#include <cimagedisplay.h>

using namespace aips;

/**
@author Hendrik Belitz
*/
class CSnakeDialog : public QObject, public CModuleDialog
{
Q_OBJECT
public:
  CSnakeDialog() throw();
  ~CSnakeDialog() throw();
/* Dialog functions */   
  /// Check if the item has its own dialog window(s)
  virtual bool hasDialog() const
    throw();
  /// Get a handle to the dialog widget
  virtual QWidget* getDialogHandle() const
    throw( NotPresentException );
  /// Sets the window caption
  virtual void setCaption ( const std::string sCaption )
    throw( NotPresentException );
  /// Show the module dialog
  virtual void showDialog()
    throw( NotPresentException );
  /// Hide the module dialog
  virtual void hideDialog()
    throw( NotPresentException );
	virtual bool isHidden()
		throw( NotPresentException );
	virtual void activateDialog()
		throw( NotPresentException );		
	void drawImage( TImagePtr img );
	void setPixmap( QPixmap& theBuffer ) { theDisplay->setPixmap( theBuffer ); }
private:
	CImageDisplay* theDisplay;
/*  QPixmap theBuffer;      ///< Buffer to draw vector field into
	QImage qtmp;*/
};

#endif
