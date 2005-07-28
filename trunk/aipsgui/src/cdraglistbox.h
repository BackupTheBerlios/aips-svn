/************************************************************************
 * File: cdraglistbox.h                                                 *
 * Project: AIPS                                                        *
 * Description: A listbox which allows dragging of its elements         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Beta                                                        *
 * Created: 10.09.03                                                    *
 * Changed: 22.01.04 Began to overwork and document the source code     *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CDRAGLISTBOX_H
#define CDRAGLISTBOX_H

#include <string>

// Qt includes
#include <qwidget.h>
#include <qlistbox.h>
#include <qdragobject.h>

/**
 * A listbox which allows dragging of its elements
 */
class CDragListBox : public QListBox
{
Q_OBJECT
private:
  /// Standard constructor
  CDragListBox();
  /// Copy constructor
  CDragListBox( CDragListBox& );
  /// Assignment operator
  CDragListBox& operator=( CDragListBox& );
public:
/* Structors */
  /// Constructor
  CDragListBox( int typeID_, int libID_, QWidget* parentPtr = NULL, const char* sName = NULL )
throw();
  /// Destructor
  virtual ~CDragListBox() throw();
	const std::string getLibraryName() const;
	void setLibraryName( const std::string& sLibName );
signals:
  /// Signals an item (second int) has been dragged from this box (first int)
  void dragItem ( int, int, int );
protected:
  /// Reimplemented from QListBox
  virtual void mouseMoveEvent ( QMouseEvent* eventPtr ) throw();
private:
  int iID; ///< ID number of the drag listbox
	int tID; ///< ID number of module type
	std::string sLibName;
};

#endif
