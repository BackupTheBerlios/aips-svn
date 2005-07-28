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
#ifndef CVIEWHISTOGRAMDIALOG_H
#define CVIEWHISTOGRAMDIALOG_H

#include <cmoduledialog.h>
#include <aipsnumeric.h>

// Qt includes
#include <qlabel.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qscrollbar.h>
#include <qlcdnumber.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qcolor.h>
#include <qpainter.h>

using namespace aips;
/**
@author Hendrik Belitz
*/
class CViewHistogramDialog : public QObject, public CModuleDialog
{
Q_OBJECT
public:
   CViewHistogramDialog() throw();
   ~CViewHistogramDialog() throw();
/* Dialog methods */		
  /// Returns true because CDisplay always has a dialog window
  virtual bool hasDialog() const
    throw();
  /// Reimplemented from CPipelineItem 
  virtual QWidget* getDialogHandle() const
    throw( NotPresentException );
  /// To set the window caption
  void setCaption ( const std::string sCaption )
    throw();
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
  /// Update the view 
  void updateView()
    throw();	
	void newImage( ushort maxRange, int iValue, double dMax, std::vector<std::vector<double> >& histVec );
private slots:
	void updateShownRange( int iRange) throw();
private:
  // Widgets
  QWidget* displayPtr;     ///< The display window
  QVBox* aColumnPtr;       ///< Layout manager
  QHBox* aRowPtr;          ///< Layout manager
  QLabel* histViewPtr;     ///< Picture label
  QImage histImage;    ///< Image that is shown
  QScrollBar* scrollerPtr; ///< Scrollbar to move the visible intensity range
  QLCDNumber* frontPtr;    ///< Display beginning of shown intensity range
  QLCDNumber* backPtr;     ///< Display end of shown intensity range	
  ushort usRangeStart;     ///< Current start of shown intensity range
  ushort usRangeEnd;       ///< Current end of shown intensity range
  ushort usMaxRange;       ///< Maximum intensity in image	
	double dMaxValue;
	std::vector<std::vector<double> > histoVec;
};

#endif
