/************************************************************************
 * File: cimagedisplay.h                                                *
 * Project: AIPS                                                        *
 * Description: A display class for 2D images, used by CDisplay         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Created: 28.01.04                                                    *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CIMAGEDISPLAY_H
#define CIMAGEDISPLAY_H

// AIPS includes
#include <aipsnumeric.h>
#include <cbase.h>

// Qt includes
#include <qlabel.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcolor.h>
#include <qslider.h>

namespace aips {
  
/**
 * A display class for 2D images, used by CDisplay
 */
class CImageDisplay : public QWidget, public CBase
{
  Q_OBJECT
private:
  /// Copy constructor
  CImageDisplay( CImageDisplay& );
  /// Assignment operator
  CImageDisplay& operator=( CImageDisplay& );
public:
/* Structors */
  /// Standard constructor
  CImageDisplay()
    throw();
  /// Destructor
  virtual ~CImageDisplay()
    throw();
/* Mutators */
  /// Set the image palette
  void setImagePalette( std::vector<QColor> paletteVec_ )
    throw();
  /// Set the image
  void setImage( const QImage& processedImage )
    throw();
	/// Set the image as a pixmap
	void setPixmap( const QPixmap& processedPixmap )
		throw();
/* Other functions */
  /// Reimplemented from QWdiget
  virtual void resizeEvent( QResizeEvent* )
    throw();
  /// Reimplemented from QWidget
  virtual QSize sizeHint() const
    throw();
private slots:
  /// Updates the view if something has changed
  void updateView();
private:
  std::vector<QColor> paletteVec; ///< The palette to use
  QImage image;                   ///< The image to show
  // Widgets
  QSlider* slicerPtr;
  QComboBox* orientationBoxPtr;   ///< Combo box 
  QVBox* aColumnPtr;              ///< Layout manager
  QLabel* imageLabelPtr;          ///< Label to display the image
  QLabel* zoomBoxLabelPtr;        ///< Label for zoom box
  QHBox* aRowPtr;                 ///< Layout manager
  QScrollView* processedPicPtr;   ///< Scrollview to display image
  QComboBox* zoomFactorBoxPtr;    ///< Combo box to determine image zoom
  QCheckBox* colorMapPtr;         ///< Check box to activate alternate palette
};

inline void CImageDisplay::setPixmap( const QPixmap& processedPixmap ) throw()
{
	imageLabelPtr->setPixmap( processedPixmap );
}

}
#endif
