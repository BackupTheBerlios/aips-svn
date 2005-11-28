/************************************************************************
 * File: cpipescroller.cpp                                              *
 * Project: AIPS                                                        *
 * Description: A scrollview that shows a processing pipeline           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 10.09.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#include "cpipescroller.h"

using namespace std;

/**
 * Constructor
 * \param parentPtr Qt parent widget
 * \param sName Qt object name
 */
CPipeScroller::CPipeScroller(QWidget *parentPtr, const char *sName ) throw()
  : QScrollView( parentPtr, sName ), QDropSite( this ), CBase( "CPipeScroller","0.6","CBase" )
{
  resizeContents( 1000, 1000 );
  setAcceptDrops( true );
  ulMaxID = 0;
    
  itemMenuPtr = new QPopupMenu ( this );
  itemMenuPtr->insertItem( "No Item", 0 );
  itemMenuPtr->insertSeparator();
  itemMenuPtr->insertItem( "Rename", 1 );
  itemMenuPtr->insertItem( "Parameters", 2 );
  itemMenuPtr->insertItem( "Module dialog", 3 );  
  itemMenuPtr->insertItem( "Disconnect all", 4 );
  itemMenuPtr->insertItem( "Delete", 5 );
  itemMenuPtr->insertItem( "Documentation", 6 );
	itemMenuPtr->insertSeparator();
	itemMenuPtr->insertItem( "CacheDummy", 7 );
  itemMenuPtr->hide();
  itemMenuPtr->setCheckable( false );
  itemMenuPtr->connectItem( 1, this, SLOT( renameItem() ) );
  itemMenuPtr->connectItem( 2, this, SLOT( showItemParameters() ) );
  itemMenuPtr->connectItem( 3, this, SLOT( toggleModuleDialog() ) );
  itemMenuPtr->connectItem( 4, this, SLOT( disconnectItem() ) );
  itemMenuPtr->connectItem( 5, this, SLOT( deleteItem() ) );
  itemMenuPtr->connectItem( 6, this, SLOT( showDocumentation() ) );
	itemMenuPtr->connectItem( 7, this, SLOT( toggleCache() ) );

  setMouseTracking( true );
  viewport()->setMouseTracking( true );
}

/** Destructor */
CPipeScroller::~CPipeScroller() throw()
{
  // Clean up connection and item list
  deletePipeline();
  delete( itemMenuPtr );
}

/**
 * React on mouse events
 * Reimplemented from QScrollView
 * \param eventPtr pointer to QT mouse event
 */
void CPipeScroller::contentsMousePressEvent( QMouseEvent* eventPtr ) throw()
{
  // Remember position where user pressed the left button and an item was found
  if ( eventPtr->button() == LeftButton &&
    viewport()->childAt( contentsToViewport( eventPtr->pos() ) ) != NULL )
  {
    lastPosition = eventPtr->pos();
  }
  // Open context menu
  else if ( eventPtr->button() == RightButton &&
    viewport()->childAt( contentsToViewport( eventPtr->pos() ) ) != NULL )
  {
    tmpItemPtr = static_cast<CPipeItemWidget*>(
      viewport()->childAt( contentsToViewport( eventPtr->pos() ) ) );
    if ( tmpItemPtr == NULL )
      return;
    // Determine position where the menu should pop up
    lastPosition = mapToGlobal( tmpItemPtr->pos() );
    // Adapt popup menu to actual item
    itemMenuPtr->changeItem( 0, tmpItemPtr->getItem()->getModuleName().c_str() );
		if ( tmpItemPtr->getItem()->isOutputCached() )
			itemMenuPtr->changeItem( 7, "Disable cache" );
		else
			itemMenuPtr->changeItem( 7, "Enable cache" );
    if ( tmpItemPtr->getItem()->getModuleDialog() && tmpItemPtr->getItem()->getModuleDialog()->hasDialog() )
      itemMenuPtr->setItemEnabled( 3, true );
    else
      itemMenuPtr->setItemEnabled( 3, false );
    if ( tmpItemPtr->getItem()->getNoOfParameters() != 0 )
      itemMenuPtr->setItemEnabled( 2, true );
    else
      itemMenuPtr->setItemEnabled( 2, false );
    // Show menu
    itemMenuPtr->popup( lastPosition );
  }
}

/**
 * React on mouse events
 * Reimplemented from QScrollView
 * \param eventPtr pointer to QT mouse event
 */
void CPipeScroller::contentsMouseMoveEvent( QMouseEvent* event ) throw()
{
  // If left button was pressed over an item and mouse was moved start a drag
  if ( ( event->state() & LeftButton )
    && ( viewport()->childAt(contentsToViewport(event->pos())) != NULL ) 
    && ( QABS( event->pos().x() - lastPosition.x() ) >= 3  ||
          QABS( event->pos().y() - lastPosition.y() ) >= 3  ) )
    {
      tmpItemPtr = dynamic_cast<CPipeItemWidget*>(
        viewport()->childAt( contentsToViewport( event->pos() ) ) );
      if ( tmpItemPtr == NULL )
        return;
      CDragPipeItem anItem( tmpItemPtr->getItem().get(), 0, 0, 0 );
      QStoredDrag* drag = new QStoredDrag ( "obj/pipeitem", this );
      drag->setEncodedData( anItem.encodedData("") );
      drag->dragMove();
    }
  else // just repaint
  {  
    viewport()->repaint();
  }
}

/**
 * Accepts entering drags which should be encoded pipeline items
 * Reimplemented from QScrollView
 */
void CPipeScroller::dragEnterEvent( QDragEnterEvent* eventPtr ) throw()
{
  eventPtr->accept();
}

/**
 * Creates a new widget for the item, connect its signals and adds to the item list
 * This function is only used to recreate a formerly saved pipeline
 * \param ulID item ID (unique)
 * \param itemPtr the corresponding item
 * \param iX x-position to insert widget
 * \param iY y-position to insert widget
 * \param sName item name
 */
void CPipeScroller::addItem( ulong ulID, CPipelineItem* itemPtr, int iX, int iY,
  std::string sName, ulong ulType, ulong ulLib, ulong ulModule ) throw()
{
	cerr << ">> CPipeScroller::addItem" << endl;
  // New id is greater than our actual max id
  if ( ulID > ulMaxID )
    ulMaxID = ulID;

  // Create widget
  CPipeItemWidget* itemWidgetPtr = new CPipeItemWidget( ulID, itemPtr->getModuleName().c_str(),
    itemPtr->getFanIn(), itemPtr->getFanOut(), ulType, ulLib, ulModule, this->viewport() );
  moveChild( itemWidgetPtr, iX, iY );
  itemWidgetPtr->show();
  itemWidgetPtr->setTitle( sName );

  // Connect signals
  connect( itemWidgetPtr, SIGNAL( connectorClicked( CConnectorWidget* ) ),
    this, SLOT( newConnection( CConnectorWidget* ) ) );
  connect( itemWidgetPtr, SIGNAL( connectionDeleted( CConnectorWidget* ) ),
    this,SLOT( deleteConnection( CConnectorWidget* ) ) );

  // Add to map
  itemPtrMap.insert( make_pair( ulID, itemWidgetPtr ) );
  // Send signals
  emit( itemAdded( itemWidgetPtr ) );
  emit( itemRenamed( ulID, sName ) );
  cerr << "<< CPipeScroller::addItem" << endl;
FEND;
}

/**
 * Drops a drag object into the pipeline view
 * Reimplemented from QScrollView
 */
void CPipeScroller::dropEvent( QDropEvent* eventPtr ) throw()
{
  // Get a pointer to the item
alog << LFIXME << "Reinterpreting a long value as an pointer adress is not that fine..."
  << endl;
  ulong ulAddress; ulong moduleid, libid, typeId;
  if ( !CDragPipeItem::decode( eventPtr, ulAddress, moduleid, libid, typeId ) )
    return;
  CPipelineItem* itemPtr = reinterpret_cast<CPipelineItem*>( ulAddress );

  // Determine drop position
  QPoint p = viewportToContents( eventPtr->pos() );
  p.setX( p.x() - p.x() % 10 );
  p.setY( p.y() - p.y() % 10 );
  if ( p.x() < 0 ) p.setX( 0 );
  if ( p.y() < 0 ) p.setY( 0 );
  // Item id greater zero means an existing item has been moved
  if ( itemPtr->getID() != 0 )
  {
    moveChild( tmpItemPtr, p.x() - ( tmpItemPtr->width() / 2 ),
      p.y() - ( tmpItemPtr->height() / 2 ) );
  }
  else
  {
    // Create a new item    
    ulMaxID++;
    CPipeItemWidget* itemWidgetPtr = new CPipeItemWidget( ulMaxID,
      itemPtr->getModuleName().c_str(), itemPtr->getFanIn(), itemPtr->getFanOut(), 
			typeId, libid, moduleid, this->viewport() );
    moveChild( itemWidgetPtr, p.x() - ( itemWidgetPtr->width() / 2 ),
      p.y() - ( itemWidgetPtr->height() / 2 ) );
    itemWidgetPtr->show();

    // Connect signals
    connect( itemWidgetPtr, SIGNAL( connectorClicked( CConnectorWidget* ) ),
      this, SLOT( newConnection( CConnectorWidget* ) ) );
    connect( itemWidgetPtr, SIGNAL( connectionDeleted( CConnectorWidget* ) ),
      this,SLOT( deleteConnection( CConnectorWidget* ) ) );

    // Add to list
    itemPtrMap.insert( make_pair( ulMaxID, itemWidgetPtr ) );
    emit( itemAdded( itemWidgetPtr ) );
  }
  this->viewport()->repaint();
}

/**
 * Checks if one of the given IO types is a child of the other
 * \param first first type
 * \param second second type
 * \returns true if the types match
 */
bool CPipeScroller::typeMatch( CPipelineItem::EIOTypes first, CPipelineItem::EIOTypes second )
  throw()
{
  int c = ( first < second ) ? first : second;
  int d = ( second >= first ) ? second : first;
  return( ( d % c ) == 0 );
}

/**
 * React on a possible new connection creation
 * \param connectorPtr connector that was clicked
 */
void CPipeScroller::newConnection( CConnectorWidget* connectorPtr ) throw()
{
  if ( !connectorPtr->isOutput() )
  {
    // Clicked connector is an input port, so this will be an end point
    if ( connectorPtr->getPartner() != NULL )
      return;
    actualConnection.endingPoint = connectorPtr->pos();
    actualConnection.targetPtr = connectorPtr->getParent();
    actualConnection.usInputPort = connectorPtr->getInputNumber() - 1;
  }
  else
  {
    // Connector is an output port, so this will be an starting point
    actualConnection.startingPoint = connectorPtr->pos();
    actualConnection.sourcePtr = connectorPtr->getParent();
    actualConnection.usOutputPort = connectorPtr->getOutputNumber() - 1;
  }

  // We have both points, so we can create a connection
  if ( ( !actualConnection.endingPoint.isNull() )
    && ( !actualConnection.startingPoint.isNull() ) )
  {
    // Ya shall never create recursive connection on one item :-)
    if ( actualConnection.sourcePtr == actualConnection.targetPtr )
    {
      // Reset points
      actualConnection.startingPoint *= 0;
      actualConnection.endingPoint *= 0; 
DBG ( "Start equals end for connection" );
      return;
    }

    // Only allow connection of matching data types
    if ( !typeMatch(
      actualConnection.sourcePtr->getItem()->getOutputType( actualConnection.usOutputPort ), 
      actualConnection.targetPtr->getItem()->getInputType( actualConnection.usInputPort ) ) )
    {
      // Reset points
      actualConnection.startingPoint *= 0;
      actualConnection.endingPoint *= 0;
DBG( "Incompatible types for connection: "
  << actualConnection.sourcePtr->getItem()->getOutputType( actualConnection.usOutputPort )
  << " != "
  << actualConnection.targetPtr->getItem()->getInputType( actualConnection.usInputPort ) );
      return;
    }

    // Create a new connection object and add it to the list
    CConnection* connectionPtr = new CConnection;
    connectionPtr->sourcePtr = actualConnection.sourcePtr;
    connectionPtr->targetPtr = actualConnection.targetPtr;
    connectionPtr->startingPoint = actualConnection.startingPoint;
    connectionPtr->endingPoint = actualConnection.endingPoint;
    connectionPtr->usInputPort = actualConnection.usInputPort;
    connectionPtr->usOutputPort = actualConnection.usOutputPort;
    connectionPtr->targetPtr->setConnectionPartner( actualConnection.usInputPort,
      connectionPtr->sourcePtr );
    connectionPtrList.push_back( connectionPtr );

    emit( connectionAdded( actualConnection.sourcePtr->getID(),
      actualConnection.targetPtr->getID(), actualConnection.usInputPort,
      actualConnection.usOutputPort ) );

    // Reset click points
    actualConnection.startingPoint *= 0;
    actualConnection.endingPoint *= 0;
#ifdef DBG2
  dumpGraph();
#endif
    this->viewport()->repaint();
  }
}

/**
 * Draws all connections
 * Reimplemented from QScrollbar
 */
void CPipeScroller::drawContents( QPainter* p, int , int , int , int ) throw()
{
  p->setPen( Qt::white );
  // Iterate the whole connection list and draw each connection
  for( list<CConnection*>::iterator it = connectionPtrList.begin();
    it != connectionPtrList.end(); ++it )
  {
    QPoint sp = viewportToContents( (*it)->sourcePtr->pos() );
    QPoint delta ( (*it)->sourcePtr->width() / 2
      - ( 10 + ( (*it)->sourcePtr->getFanOut() - 1 ) * 15 )
      + 30 * ( (*it)->usOutputPort ) + 10, (*it)->sourcePtr->height() );
    sp += delta;
    QPoint tp = viewportToContents( (*it)->targetPtr->pos() );
    QPoint delta2 ( (*it)->targetPtr->width() / 2
      - ( 10 + ( (*it)->targetPtr->getFanIn() - 1 ) * 15 )
      + 30 * ( (*it)->usInputPort ) + 10, 0 );
    tp += delta2;
    if ( sp.y() <= tp.y() )
    {
      QPoint t1( sp.x(), sp.y() + ( tp.y()-sp.y() ) / 2 );
      QPoint t2( tp.x(), sp.y() + ( tp.y()-sp.y() ) / 2 );
      p->drawLine( sp, t1 );
      p->drawLine( t1, t2 );
      p->drawLine( t2, tp );
    }
    else
    {
      QPoint t1( sp.x(), sp.y() + 10 );
      QPoint t2( tp.x() + ( sp.x() - tp.x() ) / 2, sp.y() + 10 );
      QPoint t3( tp.x() + ( sp.x() - tp.x() ) / 2, tp.y() - 10 );
      QPoint t4( tp.x(), tp.y() - 10 );
      p->drawLine( sp, t1 );
      p->drawLine( t1, t2 );
      p->drawLine( t2, t3 );
      p->drawLine( t3, t4 );
      p->drawLine( t4, tp );
    }
  }
  p->end();
}

/**
 * a debug function to print out the whole visual pipeline graph
 */
void CPipeScroller::dumpGraph() throw()
{
  alog << LDEBUG << "------------ Visual pipeline dump (BEG) ------------" << endl;
  for ( map<ulong, CPipeItemWidget*>::iterator it = itemPtrMap.begin();
    it != itemPtrMap.end(); ++it )
    alog << "Item id " << (*it).second->getID() << ", type "
      << (*it).second->getType() << "\n";
  alog << "---------------" << endl;
  int i = 0;
  for( list<CConnection*>::iterator it = connectionPtrList.begin();
    it != connectionPtrList.end(); ++it, ++i )  
    alog << "Connection " << i << ": " << (*it)->sourcePtr->getID() << " -> "
      << (*it)->targetPtr->getID() << "\n";
  alog << "------------ Visual pipeline dump (END) ------------" << endl;
}

/** Displays the documentation of the current item */
void CPipeScroller::showDocumentation() throw()
{
FBEGIN;
DBG("Processing " << tmpItemPtr->getID());
  QMessageBox::information( this, tmpItemPtr->getTitle().c_str(),
    tmpItemPtr->getItem()->getDocumentation().c_str(), "O.K." );
FEND;
}

void CPipeScroller::toggleCache() throw()
{
FBEGIN;
DBG("Processing " << tmpItemPtr->getID());
	tmpItemPtr->getItem()->cacheOutput( !tmpItemPtr->getItem()->isOutputCached() );
FEND;
}


/** Rename the actual item */
void CPipeScroller::renameItem() throw()
{
FBEGIN;
DBG("Processing " << tmpItemPtr->getID() );
  // Open a dialog at the iem position
  lastPosition = mapToGlobal( tmpItemPtr->pos() );
  QDialog aDialog( this, "Rename pipeline item", true );
  QString sTmp ("Renaming ");
  sTmp += tmpItemPtr->getItem()->getModuleName().c_str();
  aDialog.setCaption( sTmp );
  aDialog.setGeometry( lastPosition.x(), lastPosition.y(), 220,70);
  QVBox* row = new QVBox( &aDialog );
  row->setFixedSize(220,70);
  QHBox* col = new QHBox( row );
  col->setFixedSize(220,35);
  // Display old name in input box
  QLineEdit* line = new QLineEdit( tmpItemPtr->getItem()->getModuleName().c_str(), col );
  QHBox* col2 = new QHBox( row );
  col2->setFixedSize(220,35);
  QPushButton* ok = new QPushButton( "&O.K.", col2);
  ok->setFixedSize(80,30);
  QPushButton* cancel = new QPushButton( "&Cancel", col2 );
  cancel->setFixedSize(80,30);
  connect( ok, SIGNAL( clicked() ), &aDialog, SLOT( accept() ) );
  connect( cancel, SIGNAL( clicked() ), &aDialog, SLOT( reject() ) );
  // Set new name
  if ( aDialog.exec() == QDialog::Accepted )
  {
    tmpItemPtr->getItem()->setModuleName( line->text().ascii() );
    tmpItemPtr->setTitle( line->text().ascii() );
  }
  // Repaint
  tmpItemPtr->repaint();
  // Emit signal so the main window will also be informed
  emit( itemRenamed( tmpItemPtr->getID(), tmpItemPtr->getItem()->getModuleName() ) );
FEND;
}

/**
 * Displays the parameter setting dialog for the actual item
 */
void CPipeScroller::showItemParameters() throw()
{
  boost::shared_ptr<CPipelineItem> itemPtr = tmpItemPtr->getItem();
  if ( itemPtr->getNoOfParameters() == 0 ) return;
  
  QDialog paramDialog( this, "Parameter Dialog", true );
  paramDialog.setCaption( itemPtr->getModuleName().c_str() );
  paramDialog.setGeometry( lastPosition.x(), lastPosition.y(), 300,
    30 * itemPtr->getNoOfParameters() + 30);
  QGrid* gridPtr = new QGrid( 3, Qt::Horizontal, &paramDialog );
  gridPtr->setFixedWidth( 300 );
  gridPtr->setFixedHeight(paramDialog.height());

  QLabel* labelPtr; vector<QLineEdit*> linePtrArr;
	CTypedMap* parameters = itemPtr->getParameters();
  vector<string> keyList = parameters->getKeyList();
	vector<QString> labelNames;
	uint j = 0;
  for ( uint i = 0; i < keyList.size(); i++ )
  {    
		if ( keyList[i].rfind( "_Default" ) != string::npos
			|| keyList[i].rfind( "_Minimum" ) != string::npos
			|| keyList[i].rfind( "_Maximum" ) != string::npos )
		{
			continue;		
		}
		labelNames.push_back( QString( keyList[i].c_str() ) );
    labelPtr = new QLabel( keyList[i].c_str(), gridPtr );
    QString sTmp = "("; QString t2;
		if ( parameters->getValueType(keyList[i]) == typeid( long ) )
		{
      sTmp += "Signed Integer)"; 
			t2.setNum( parameters->getLong(keyList[i]) );			
		}
		else if ( parameters->getValueType(keyList[i]) == typeid( ulong ) )
		{
      sTmp += "Unsigned Integer)"; 
			t2.setNum( parameters->getUnsignedLong(keyList[i]) );			
		}
		else if ( parameters->getValueType(keyList[i]) == typeid( double ) )
		{
      sTmp += "Floating point)"; 
			t2.setNum( parameters->getDouble(keyList[i]) );			
		}
		else if ( parameters->getValueType(keyList[i]) == typeid( bool ) )
		{
      sTmp += "Boolean)"; 
			t2.setNum( parameters->getBool(keyList[i]) );			
		}
		else if ( parameters->getValueType(keyList[i]) == typeid( std::string ) )
		{
      sTmp += "String)"; 
			t2 = parameters->getString(keyList[i]).c_str();			
		}

    labelPtr = new QLabel( sTmp, gridPtr );
    linePtrArr.push_back( new QLineEdit( t2, gridPtr ) ); j++;
  }
	keyList = parameters->getKeyList();
  
  QPushButton* ok = new QPushButton( "&O.K.", gridPtr );
  ok->setFixedSize(80,30);
  QPushButton* cancel = new QPushButton( "&Cancel", gridPtr );
  cancel->setFixedSize(80,30);

  connect( ok, SIGNAL( clicked() ), &paramDialog, SLOT( accept() ) );
  connect( cancel, SIGNAL( clicked() ), &paramDialog, SLOT( reject() ) );

  if ( paramDialog.exec() == QDialog::Accepted )
  {
		keyList = parameters->getKeyList();
for( uint i = 0; i < keyList.size(); ++i )
{
	cerr <<  "[" << i << "] <" << keyList[i] << "> " << endl;
}
    for ( uint i = 0; i < labelNames.size(); i++ )
    {
      QString sTmp;
      sTmp = linePtrArr[i]->text();
			string sLabel = labelNames[i].ascii();
			if ( parameters->getValueType( sLabel ) == typeid( long ) 
				&& sTmp.toLong() <= parameters->getLong(sLabel+"_Maximum")
				&& sTmp.toLong() >= parameters->getLong(sLabel+"_Minimum") )
			{	
				parameters->setLong( sLabel, sTmp.toLong() );
			}
			else if ( parameters->getValueType(sLabel) == typeid( ulong ) 
				&& sTmp.toULong() <= parameters->getUnsignedLong(sLabel+"_Maximum")
				&& sTmp.toULong() >= parameters->getUnsignedLong(sLabel+"_Minimum") )
			{
				parameters->setUnsignedLong( sLabel, sTmp.toULong() );
			}
			else if ( parameters->getValueType(sLabel) == typeid( double ) 
				&& sTmp.toDouble() <= parameters->getDouble(sLabel+"_Maximum")
				&& sTmp.toDouble() >= parameters->getDouble(sLabel+"_Minimum") )
			{
				parameters->setDouble( sLabel, sTmp.toDouble() );
			}
			else if ( parameters->getValueType(sLabel) == typeid( bool ) )
			{
				parameters->setBool( sLabel, static_cast<bool>( sTmp.toUInt() ) );
			}
			else if ( parameters->getValueType(sLabel) == typeid( std::string ) )
			{
				parameters->setString( sLabel, sTmp.ascii() );
			}
			else // This should NEVER happen
			{
				alog << LERR << "Uh, something went really wrong here. We cannot determine the type of "
					<< sLabel << endl;
				//terminate();
			}
    }
   	itemPtr->forceRecomputation();
   	emit( showItem( tmpItemPtr->getID() ) );
    
		linePtrArr.clear();
  }
}

/**
 * Erase all connections of the actual item
 */
void CPipeScroller::disconnectItem() throw()
{
  for( list<CConnection*>::iterator it = connectionPtrList.begin();
    it != connectionPtrList.end(); ++it )
  {
    if ( ( (*it)->sourcePtr == tmpItemPtr ) || ( (*it)->targetPtr == tmpItemPtr ) )
    {
      (*it)->targetPtr->setConnectionPartner( (*it)->usInputPort, NULL );
      emit( connectionDeleted( (*it)->sourcePtr->getID(), (*it)->targetPtr->getID(),
        (*it)->usInputPort, (*it)->usOutputPort ) );
      delete (*it);
      list<CConnection*>::iterator beforeIt = it; beforeIt--;
      connectionPtrList.erase( it );
      it = beforeIt;      
    }
  }
  this->viewport()->repaint();
}

/**
 * Delete the actual item
 */
void CPipeScroller::deleteItem() throw()
{
FBEGIN;
DBG("Delete " << tmpItemPtr->getID() );

  disconnectItem();
  removeChild(tmpItemPtr);
//  bool bErased = false;
//  for( vector<CPipeItemWidget*>::iterator it = itemPtrVec.begin();
//    it != itemPtrVec.end() && !bErased; ++it )
//  {
//    if ( (*it) == tmpItemPtr )
//    {
//      itemPtrVec.erase(it);  bErased = true;
//    }
//  }
  itemPtrMap.erase( tmpItemPtr->getID() );

  emit( itemDeleted( tmpItemPtr->getID() ) );
  delete tmpItemPtr;
  tmpItemPtr = NULL;
FEND;
}

/**
 * Delete the clicked connection
 */
void CPipeScroller::deleteConnection( CConnectorWidget* aConnector ) throw()
{
FBEGIN;
  for( list<CConnection*>::iterator it = connectionPtrList.begin();
    it != connectionPtrList.end(); ++it )
  {    
    if ( ( (*it)->targetPtr == aConnector->getParent() )
      && ( (*it)->sourcePtr == aConnector->getPartner() )
      && ( (*it)->usInputPort + 1 == aConnector->getInputNumber() ) )
    {
      aConnector->setPartner( NULL );
      emit( connectionDeleted( (*it)->sourcePtr->getID(), (*it)->targetPtr->getID(),
        (*it)->usInputPort, (*it)->usOutputPort ) );
      delete (*it);
      list<CConnection*>::iterator beforeIt = it; beforeIt--;
      connectionPtrList.erase( it );
      it = beforeIt;
    }
  }
  dumpGraph();  
  this->viewport()->repaint();
FEND;  
}

/** Toggles the module dialog of the actual item */
void CPipeScroller::toggleModuleDialog() throw()
{
/*  if ( tmpItemPtr->getItem()->getModuleDialog()->isHidden() )
    tmpItemPtr->getItem()->getModuleDialog()->showDialog();
  else
    tmpItemPtr->getItem()->getModuleDialog()->hideDialog();*/
  emit( toggleDialog( tmpItemPtr->getID() ) );
}

/**
 * Returns the position of the item with the given id number
 * \param ulID the id number of the item
 * \returns item position in scroll view
 */
QPoint CPipeScroller::getItemPosition( ulong ulID ) throw()
{
	QPoint pos( itemPtrMap[ulID]->pos().x() + contentsX(),
		itemPtrMap[ulID]->pos().y() + contentsY() );
  return pos;
}

/**
 * Adds a new connection to the pipeline
 * \param iSourceID id of source item
 * \param iOutputPort used port number of source item
 * \param iTargetID id of target item
 * \param iInputPort used port number of target item
 */
void CPipeScroller::addConnection( int iSourceID, int iOutputPort, int iTargetID,
  int iInputPort ) throw()
{
  CConnection* aConnection = new CConnection;
  aConnection->sourcePtr = itemPtrMap[iSourceID];
  aConnection->targetPtr = itemPtrMap[iTargetID];
  aConnection->usInputPort = iInputPort;
  aConnection->usOutputPort = iOutputPort;
  aConnection->targetPtr->setConnectionPartner( iInputPort, aConnection->sourcePtr );
  connectionPtrList.push_back( aConnection );

  emit( connectionAdded( iSourceID, iTargetID, iInputPort, iOutputPort ) );
}

/** Deletes the whole visual pipeline */
void CPipeScroller::deletePipeline() throw()
{
  while( itemPtrMap.begin() != itemPtrMap.end() )
  {
    tmpItemPtr = (*(itemPtrMap.begin())).second;
    deleteItem();
  }
  ulMaxID = 0;
}
