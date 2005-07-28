/************************************************************************
 * File: cdragpipeitem.h                                                *
 * Project: AIPS                                                        *
 * Description: A drag and drop item storing pipeline items             *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status : Beta                                                        *
 * Created: 10.09.03                                                    *
 * Changed: 22.01.04 Began to overwork and document the source code     *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CDRAGPIPEITEM_H
#define CDRAGPIPEITEM_H

// AIPS includes
#include <cpipelineitem.h>

// Qt includes
#include <qwidget.h>
#include <qdragobject.h>
#include <qcstring.h>
#include <qdatastream.h>


using namespace aips;

class CDragPipeItem : public QStoredDrag
{
Q_OBJECT
private:
  /// Standard constructor
  CDragPipeItem();
  /// Copy constructor
  CDragPipeItem( CDragPipeItem& );
  /// Assignment operator
  CDragPipeItem& operator=( CDragPipeItem& );
public:
/* Structors */
  /// Constructor
  CDragPipeItem( CPipelineItem* itemPtr_, ulong moduleID, ulong libID, ulong typeID, QWidget*
parentPtr = NULL, const char *sName = NULL )
    throw();
  /// Destructor
  virtual ~CDragPipeItem()
    throw();
/* Other methods */
  /// Reimplemented from QStoredDrag
  static bool decode ( const QMimeSource* mineSourcePtr, ulong &ulAddress, ulong &moduleID, ulong
&libID, ulong &typeID);
  /// Reimplemented from QStoredDrag
  virtual const char* format ( int = 0 ) const;
  /// Reimplemented from QStoredDrag
  virtual QByteArray encodedData( const char* ) const;
private:
  QString objName;        ///< Name of the object
  CPipelineItem* itemPtr; ///< Pointer to corresponding CPipelineItem
	ulong moduleid, libid, typeId;
};

#endif
