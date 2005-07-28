/************************************************************************
 * File: cpipescroller.h                                                *
 * Project: AIPS                                                        *
 * Description: A scrollview that shows a processing pipeline           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.6                                                         *
 * Status : Alpha                                                       *
 * Created: 10.09.03                                                    *
 * Changed: 22.01.04 Began to overwork and document the source code     *
 *          23.01.04 Items are now stored in a std::map                 *
 *                   CPipeScroller now also is derived from CBase       *
 *          14.05.04 Corrected a bug in getItemPos(), which now returns *
 *                   the absolute and not the viewport position         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CPIPESCROLLER_H
#define CPIPESCROLLER_H

// Standard includes
#include <list>
#include <map>

// AIPS includes
#include "cdragpipeitem.h"
#include "cconnectorwidget.h"
#include <cpipelineitem.h>

// Qt includes
#include <qwidget.h>
#include <qscrollview.h>
#include <qdragobject.h>
#include <qdropsite.h>
#include <qpopupmenu.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qgrid.h>
#include <qvalidator.h>
#include <qmessagebox.h>


/**
 * A scrollview that shows a processing pipeline
 */
class CPipeScroller : public QScrollView, public QDropSite, public CBase
{
  Q_OBJECT
private:
  /// Standard constructor
  CPipeScroller();
  /// Copy constructor
  CPipeScroller( CPipeScroller& );
  /// Assignment operator
  CPipeScroller& operator=( CPipeScroller& );
public:
/* Structors */
  /// Consrtuctor
  CPipeScroller( QWidget* parentPtr = NULL, const char *sName = NULL )
    throw();
  /// Destructor
  ~CPipeScroller()
    throw();
/* Other methods */
  /// Returns the position of the item with the given id number
  QPoint getItemPosition( ulong ulID )
    throw();
  /// Creates a new widget for the item, connect its signals and adds to the item list
  void addItem( ulong ulID, CPipelineItem* itemPtr, int iX, int iY, std::string sName,
		ulong ulType, ulong ulLib, ulong ulModule  )
    throw();
  /// Adds a new connection to the pipeline
  void addConnection( int, int, int ,int )
    throw();
  /// Deletes the whole visual pipeline
  void deletePipeline()
    throw();
signals:
  /// Signals a new item has been added to the pipeline
  void itemAdded( CPipeItemWidget* );
  /// Signals the id of an item that was deleted
  void itemDeleted( ulong );
  /// Signals source id, target id, input port and output port of a newly created connection
  void connectionAdded( int, int, int, int );
  /// Signals source id, target id, input port and output port of a deleted connection
  void connectionDeleted( int, int, int, int );
  /// Signals the item with the given id should be shown
  void showItem( ulong );
  /// Signals the item with the given id should be renamed to the given string
  void itemRenamed( ulong, std::string );
  /// Signals the dialog of the item with the given id should be toggled
  void toggleDialog( int ); 
protected:
  /// React on mouse events  
  virtual void contentsMousePressEvent( QMouseEvent* eventPtr )
    throw();
  /// React on mouse events  
  virtual void contentsMouseMoveEvent( QMouseEvent* eventPtr )
    throw();
  /// Accepts entering drags which should be encoded pipeline items
  virtual void dragEnterEvent( QDragEnterEvent* eventPtr )
    throw();
  /// Drops a drag object into the pipeline view
  virtual void dropEvent( QDropEvent* eventPtr )
    throw();
  /// Draws all connections
  virtual void drawContents( QPainter* p, int, int, int, int )
    throw();
private slots:
  /// React on a possible new connection creation
  void newConnection( CConnectorWidget* connectorPtr )
    throw();
  /// Erase all connections of the actual item
  void disconnectItem()
    throw();
  /// Delete the actual item
  void deleteItem()
    throw();
  /// Displays the documentation of the current item 
  void showDocumentation()
    throw();
  /// Delete the clicked connection
  void deleteConnection( CConnectorWidget* connectorPtr )
    throw();
  /// Displays the parameter setting dialog for the actual item
  void showItemParameters()
    throw(); 
  /// Rename the actual item
  void renameItem()
    throw();
  /// Toggles the module dialog of the actual item
  void toggleModuleDialog()
    throw();
	void toggleCache()
		throw();
private:
  /// A debug function to print out the whole visual pipeline graph
  void dumpGraph()
    throw();
  /// Checks if one of the given IO types is a child of the other
  bool typeMatch( CPipelineItem::EIOTypes first, CPipelineItem::EIOTypes second )
    throw();
  /// Structure representing a connection in the visual processing graph
  struct CConnection
  {
    CPipeItemWidget* sourcePtr; ///< Pointer to source item widget
    CPipeItemWidget* targetPtr; ///< Pointer to target item widget
    ushort usInputPort;         ///< Used input port
    ushort usOutputPort;        ///< Used output port
    QPoint startingPoint;       ///< Starting point of connection
    QPoint endingPoint;         ///< Ending point of connection
  };
  QPoint lastPosition;          ///< Position of last mouse press
  CConnection actualConnection; ///< Stores all data for a new connection
  CPipeItemWidget* tmpItemPtr;  ///< Pointer to the current item
  ulong ulMaxID;                ///< Maximum item id used
  std::list<CConnection*> connectionPtrList;
    ///< List of all established connections
  std::map<ulong, CPipeItemWidget*> itemPtrMap;
    ///< Map of all items in the visual pipeline, indexed by their id number
  QPopupMenu* itemMenuPtr;      ///< POinter to an item context menu
};

#endif
