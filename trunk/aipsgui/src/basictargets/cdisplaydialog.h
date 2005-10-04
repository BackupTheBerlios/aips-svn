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
#ifndef CDISPLAYDIALOG_H
#define CDISPLAYDIALOG_H

#include <cmoduledialog.h>
#include <aipsnumeric.h>
#include <cvtkadapter.h>
#include <qvbox.h>
#include <qscrollbar.h>
#include <qcheckbox.h>
#include <qlcdnumber.h>
#include <qfiledialog.h>
#include <qlabel.h>

#include <vtkQtRenderWindow.h>
#include <vtkQtRenderWindowInteractor.h>
#include <vtkImageActor.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkImageMapToColors.h>

using namespace aips;

/**
@author Hendrik Belitz
*/

class CDisplayWindow : public QWidget
{
Q_OBJECT
public:
  CDisplayWindow();
  ~CDisplayWindow();
  void update();
  vtkRenderer* getRenderer();
  void resizeEvent( QResizeEvent* e )
    throw();
  void loadLookupTable( std::string sFilename = "/home/hendrik/bin/mricro/spectrum.lut" );
  void setUpperClamp( double dValue );
  void setLowerClamp( double dValue );
  void setImage( vtkImageData* anImage );
  vtkImageActor* getImage() { return theImage; }
  void testDataRamge( double minRange, double maxRange );
public slots:  
  void updateMax( int i );
  void updateMin( int i );
  void toggleTransparency( int i );
  void toggleInterpolation( int i );
private:
  vtkQtRenderWindowInteractor* interactor;
  vtkQtRenderWindow* display;
  vtkRenderer* renderer;
  vtkLookupTable* theLut;
private:
  vtkLookupTable* theLookupTable;
  vtkScalarBarActor* theColorBar;
  vtkFloatingPointType dClampValues[2];
  vtkImageActor* theImage;
  vtkImageMapToColors *colorsMapper;
  QVBox* aColumnPtr;
  QScrollBar* dataMin;
  QScrollBar* dataMax;
  QCheckBox* transparency;
  QCheckBox* interpolate;
  QLabel* doc1;
  QLabel* doc2;
  QLabel* doc3;
  QLabel* doc4;
  double lutmin,lutmax;
  bool doNotUpdate;
};

class CDisplayDialog : public CModuleDialog, vtkObject
{
public:
    CDisplayDialog() throw();

    ~CDisplayDialog() throw();
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
  void updateView( TImagePtr inputPtr )
    throw();	
	void updateView( TFieldPtr inputPtr )
    throw();
  
private:
  CDisplayWindow* displayPtr; ///< The display window
  vtkImageActor* myActor;
  uint width, height;
};

#endif
