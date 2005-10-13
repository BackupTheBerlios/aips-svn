//
// C++ Interface: cvectorwindow
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CVECTORWINDOW_H
#define CVECTORWINDOW_H

#ifdef USE_RENDERING

#include <cvtkadapter.h>
#include <cglobalconfig.h>
#include <qvbox.h>
#include <qscrollbar.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qtabwidget.h>

#include <vtkQtRenderWindow.h>
#include <vtkQtRenderWindowInteractor.h>
#include <vtkImageActor.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkImageMapToColors.h>
#include <vtkImageBlend.h>
#include <vtkArrowSource.h>
#include <vtkGlyph2D.h>
#include <vtkStructuredGrid.h>
#include <vtkPolyDataMapper2D.h>

using namespace aips;

/**
@author Hendrik Belitz
*/
class CVectorWindow : public QWidget
{
Q_OBJECT
public:
  CVectorWindow();
  ~CVectorWindow();
  void update();
  vtkRenderer* getRenderer();
  void resizeEvent( QResizeEvent* e )
    throw();
  void loadLookupTable( std::string sFilename = "/home/hendrik/bin/mricro/spectrum.lut", bool bForImage = true );
  void setImageUpperClamp( double dValue );
  void setImageLowerClamp( double dValue );
  void setImage( vtkImageData* anImage );
  vtkImageActor* getImage() { return theImage; }
  void testImageDataRange( double minRange, double maxRange );
  void setOverlayUpperClamp( double dValue );
  void setOverlayLowerClamp( double dValue );
  void setOverlay( vtkStructuredGrid* anOverlay );
  vtkActor2D* getOverlay() { return theOverlay; }
   void testOverlayDataRange( double minRange, double maxRange );
public slots:  
  void updateImageMax( int i );
  void updateImageMin( int i );
  void toggleImageTransparency( int i );
  void toggleImageInterpolation( int i );
  void imageMinDataChanged();
  void imageMaxDataChanged();
  void loadImageLutFile();
  void updateOverlayMax( int i );
  void updateOverlayMin( int i );
  void toggleOverlayTransparency( int i );
  void toggleOverlayInterpolation( int i );
  void overlayMinDataChanged();
  void overlayMaxDataChanged();
  void loadOverlayLutFile();
private:
  vtkQtRenderWindowInteractor* interactor;
  vtkQtRenderWindow* display;
  vtkRenderer* renderer;
  vtkLookupTable* theImageLookupTable;
  vtkLookupTable* theOverlayLookupTable;
  vtkScalarBarActor* theImageColorBar;
  vtkScalarBarActor* theOverlayColorBar;
  vtkFloatingPointType dImageClampValues[2];
  vtkFloatingPointType dOverlayClampValues[2];
  vtkImageActor* theImage;
  vtkActor2D* theOverlay;
  vtkGlyph2D* theFieldGlyphs;
  vtkArrowSource* theGlyphSource;
  vtkImageMapToColors *theImageColorsMapper;
  vtkImageData* theActualImage;
  vtkPolyDataMapper2D* theFieldMapper;
  vtkStructuredGrid* theActualOverlay;
  QPushButton* theImageLutFileButton;
  QScrollBar* theImageDataMin;
  QScrollBar* theImageDataMax;
  QCheckBox* theImageTransparency;
  QCheckBox* theImageInterpolate;
  QLineEdit* theImageMinDisplay;
  QLineEdit* theImageMaxDisplay;
  QPushButton* theOverlayLutFileButton;
  QScrollBar* theOverlayDataMin;
  QScrollBar* theOverlayDataMax;
  QCheckBox* theOverlayTransparency;
  QCheckBox* theOverlayInterpolate;
  QLineEdit* theOverlayMinDisplay;
  QLineEdit* theOverlayMaxDisplay;
  QVBox* aGlobalColumnPtr;
  double dImageLut[2];
  double dOverlayLut[2];
  bool doNotUpdate;
};

#endif


#endif
