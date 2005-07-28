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
#ifndef CDRAWERDIALOG_H
#define CDRAWERDIALOG_H

#include <cmoduledialog.h>
#include <aipsnumeric.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qpainter.h>
#include <boost/shared_ptr.hpp>
using namespace aips;
using namespace boost;

class PolyDrawer;
class CManualSegmenter;  

class CDrawerChangedEvent : public CEvent
{
public:
	CDrawerChangedEvent( CSubject* generator) throw()
		: CEvent( generator, 999 ) {}
	CDrawerChangedEvent( CSubject* generator, TField2DPtr field_ ) throw()
		: CEvent( generator, 999 ), field( field_ ) {}
	~CDrawerChangedEvent() throw() {}
	TField2DPtr getField() { return field; }
private:
	TField2DPtr field;
};

class CDrawerDialog : public CModuleDialog
{
public:
  CDrawerDialog( CManualSegmenter* parent_ ) throw();
  ~CDrawerDialog() throw();
  /// Check if the item has its own dialog window(s)
  virtual bool hasDialog() const
    throw();
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
	void updateView( TImagePtr );
	void so( TField2DPtr p2 );
	CManualSegmenter* parent;
private:
  PolyDrawer* theDisplay; ///< The display window
  QImage processed;           ///< Image that is shown
  QPixmap processedPixmap;  
};

class PolyDrawer : public QWidget
{
public:
  PolyDrawer( CDrawerDialog* );
  ~PolyDrawer();
  void setBuffer( QPixmap p ) { schwampf = p; }
	int iWidth;
	int iHeight;
protected:
  virtual void mousePressEvent( QMouseEvent* );
  virtual void paintEvent( QPaintEvent* );
private:
  CDrawerDialog* father;
  std::vector<TVector2D> polygon;
  QPixmap buffer;
  QPixmap schwampf;
};
			
#endif
