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
#ifndef CIMAGEWRITERDIALOG_H
#define CIMAGEWRITERDIALOG_H

#include <cmoduledialog.h>
// Qt includes
#include <qwidget.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <boost/shared_ptr.hpp>

class CImageWriter;
using namespace aips;
using namespace std;
using namespace boost;

const uint EFileNameChangedEvent = 501;
const uint ELoadActivatedEvent = 502;

class CFileNameChangedEvent : public CEvent
{
public:
	CFileNameChangedEvent( CSubject* generator, string newFilename_ ) throw()
		: CEvent( generator, EFileNameChangedEvent, "CFileNameChangedEvent", "0.1", "CEvent" ), newFilename( newFilename_ )  {}
	~CFileNameChangedEvent() throw() {}
	string getFilename() { return newFilename; }
private:
	string newFilename;
};

class CLoadActivatedEvent : public CEvent
{
public:
	CLoadActivatedEvent( CSubject* generator ) throw()
		: CEvent( generator, ELoadActivatedEvent, "CCLoadActivatedEventEvent", "0.1", "CEvent" ) {}
	~CLoadActivatedEvent() throw() {}
};

/**
@author Hendrik Belitz
*/
class CImageWriterDialog : public QObject, public CModuleDialog
{
Q_OBJECT
public:
   CImageWriterDialog( CImageWriter* parent_ ) throw();
   ~CImageWriterDialog() throw();
/* Dialog handling functions */
  /// Returns true because CSource always has a dialog window
  virtual bool hasDialog() const
    throw();
  /// Reimplemented from CPipelineItem
  virtual QWidget* getDialogHandle() const
    throw( NotPresentException );
  /// Reimplemented from CPipelineItem
  virtual void setCaption( std::string sCaption )
    throw( NotPresentException );
  /// Reimplemented from CPipelineItem
  virtual void showDialog()
    throw( NotPresentException );
  /// Reimplemented from CPipelineItem
  virtual void hideDialog()
    throw( NotPresentException );
  virtual bool isHidden()
    throw( NotPresentException );		
	virtual void activateDialog()
		throw( NotPresentException );		
	void setPath( string path_ ) { path = path_; }
	void setFileTypes( string ft ) { filetypes = ft; }
public slots:
  /// Opens a file dialog to allow selection of new source data
  void selectNewFile();
  /// Loads the new dataset
  void updateData();		
private:
  // Widgets
	string filetypes;
	string filename;
	string path;
// Widgets
  QWidget* theDisplay;            ///< The display window
  QVBox* aColumnPtr;              ///< Layout manager
	QHBox* aRow;                  ///< Layout manager
  QPushButton* fileNameButtonPtr; ///< Button to select file name
  QPushButton* saveDataButtonPtr; ///< Button to save file
/*  QCheckBox* checkSwapEndianess;///< Checkbox to swap image data endianess
  QCheckBox* checkMirrorX;      ///< Checkbox to swap image data X-Axis
  QCheckBox* checkMirrorY;      ///< Checkbox to swap image data Y-Axis
  QCheckBox* checkMirrorZ;      ///< Checkbox to swap image data Z-Axis
  QLineEdit* linearShift;       ///< Shift intensities in file by this amount
  QLabel* linearShiftLabel;     ///< Label for the above*/
	CImageWriter* parent;
};

#endif
