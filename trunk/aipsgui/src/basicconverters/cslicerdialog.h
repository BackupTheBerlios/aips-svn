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
#ifndef CSLICERDIALOG_H
#define CSLICERDIALOG_H

#include <cmoduledialog.h>
// Qt includes
#include <qcombobox.h>
#include <qscrollbar.h>
#include <qlcdnumber.h>
#include <qvbox.h>
#include <qhbox.h>
#include <boost/shared_ptr.hpp>

using namespace aips;

class CSliceSelector;

const uint ESliceChangedEvent = 601;
const uint EOrientationChangedEvent = 602;

class CSliceChangedEvent : public CEvent
{
public:
	CSliceChangedEvent( CSubject* generator, int newSlice_ ) throw()
		: CEvent( generator, ESliceChangedEvent, "CSliceChangedEvent", "0.1", "CEvent" ), newSlice( newSlice_ )  {}
	~CSliceChangedEvent() throw() {}
	int getSlice() { return newSlice; }
private:
	int newSlice;
};

class COrientationChangedEvent : public CEvent
{
public:
	COrientationChangedEvent( CSubject* generator, int newOrientation_ ) throw()
		: CEvent( generator, EOrientationChangedEvent, "COrientationChangedEvent", "0.1", "CEvent" ), newOrientation( newOrientation_ )  {}
	~COrientationChangedEvent() throw() {}
	int getOrientation() { return newOrientation; }
private:
	int newOrientation;
};

/**
@author Hendrik Belitz
*/
class CSlicerDialog : public QObject, public CModuleDialog
{
Q_OBJECT
public:
  CSlicerDialog( CSliceSelector* parent_ ) throw();
  ~CSlicerDialog() throw();
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
	void hideGUIElements();
	void showGUIElements();
	void setNumberDisplay( int i );
	void setSlicerMax( int i );
	void setSlicerValue( int i );
	void setOrientation( int i );
/* Other slots */    
public slots:
  /// View a new slice of the dataset
  void updateSlice( int iSlice );
  /// View dataset in a different orientation
  void updateOrientation ( int iOrientation );		
private:
  QWidget* theDisplay;          ///< The dialog window
  QVBox* aColumn;               ///< Layout manager
  QScrollBar* sliceSelector;    ///< Slice selection scrollbar
  QLCDNumber* sliceNumberView;  ///< Displays the number of the actual slice
  QComboBox* orientationChooser;///< To choose the slice plane orientation
	CSliceSelector* parent;
};

#endif
