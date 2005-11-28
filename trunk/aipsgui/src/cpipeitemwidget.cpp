/************************************************************************
 * File: cpipeitemwidget.cpp                                            *
 * Project: AIPS                                                        *
 * Description: A widget to display pipeline modules                    *
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

#include "cpipeitemwidget.h"
#include "cconnectorwidget.h"

using namespace std;

/**
 * Constructor
 * \param ulID_ unique item id in pipeline
 * \param sTitle_ widget title that will be displayed on screen
 * \param usFanIn_ fanin of the item
 * \param usFanOut_ fanout of the item
 * \param ulType_ type of the item
 * \param parentPtr Qt parent widget
 * \param sName Qt object name
 */
CPipeItemWidget::CPipeItemWidget( ulong ulID_, std::string sTitle_, ushort usFanIn_,
  ushort usFanOut_, ulong ulType_, ulong ulLib_, ulong ulModule_, QWidget* parentPtr, 
	const char* sName ) throw()
  : QWidget( parentPtr, sName ), ulID( ulID_ ), sTitle( sTitle_ ), usFanIn( usFanIn_ ),
    usFanOut( usFanOut_ ), ulType( ulType_ ), ulLib( ulLib_), ulModule( ulModule_ )
{
	cerr << ">> CPipeItemWidget::CPipeItemWidget" << endl;
  setFixedSize(140,40);
  if ( usFanOut > 0 )
  {
    int iStart = width() / 2 - 10;
    if ( usFanOut > 1 )
      iStart = iStart - ( ( usFanOut - 1 ) * 15 );
    for ( ushort i = 0; i < usFanOut; i++ )
    {
      CConnectorWidget* connectorPtr = new CConnectorWidget ( true, this, "ConnOut" );
      connectorPtr->setOutputNumber( i + 1 );
      outputConnectorsPtrVec.push_back( connectorPtr );
      connectorPtr->move( iStart + 30 * i, height() - 10 );
      connect( connectorPtr, SIGNAL( connectorClicked( CConnectorWidget* ) ),
        this, SIGNAL( connectorClicked( CConnectorWidget* ) ) );
    }
  }

  if ( usFanIn > 0 )
  {
    int iStart = width() / 2 - 10;
    if ( usFanIn > 1 )
      iStart = iStart - ( ( usFanIn - 1 ) * 15 );
    for ( ushort i = 0; i < usFanIn; i++ )
    {
      CConnectorWidget* connectorPtr = new CConnectorWidget ( false, this, "ConnIn" );
      connectorPtr->setInputNumber( i + 1 );
      inputConnectorsPtrVec.push_back( connectorPtr );
      connectorPtr->move( iStart + 30 * i, 0 );
      connect( connectorPtr, SIGNAL( connectorClicked( CConnectorWidget* ) ),
        this, SIGNAL( connectorClicked( CConnectorWidget* ) ) );
      connect( connectorPtr, SIGNAL( connectionDeleted( CConnectorWidget* ) ),
        this, SIGNAL( connectionDeleted( CConnectorWidget* ) ) );
    }
  }
  cerr << "<< CPipeItemWidget::CPipeItemWidget" << endl;
}

/* Destructor */
CPipeItemWidget::~CPipeItemWidget() throw()
{
  vector<CConnectorWidget*>::iterator it = inputConnectorsPtrVec.begin();
  while( it != inputConnectorsPtrVec.end() )
  {
    delete (*it);
    inputConnectorsPtrVec.erase( it );
  }
  it = outputConnectorsPtrVec.begin();
  while( it != outputConnectorsPtrVec.end() )
  {
    delete (*it);
    outputConnectorsPtrVec.erase( it );
  }
}

/** \returns the corresponding pipeline item */
boost::shared_ptr<CPipelineItem> CPipeItemWidget::getItem() const throw()
{
  return itemPtr;
}

/** \returns the pipeline item type */
const ulong CPipeItemWidget::getType() const throw()
{
  return ulType;
}

const ulong CPipeItemWidget::getLib() const throw()
{
  return ulLib;
}

const ulong CPipeItemWidget::getModule() const throw()
{
  return ulModule;
}

/** \returns the pipeline item fanin */
const ushort CPipeItemWidget::getFanIn() const throw()
{
  return usFanIn;
}
      
/** \returns the pipeline item fanout */
const ushort CPipeItemWidget::getFanOut() const throw()
{
  return usFanOut;
}

/** \returns the pipeline item id */
const ulong CPipeItemWidget::getID() const throw()
{
  return ulID;
}

/** \returns the widget title */
const std::string CPipeItemWidget::getTitle() const throw()
{
  return sTitle;
}

/**
 * Sets the connection partner for the given input port
 * \param usInputPort port number
 * \param partnerPtr pointer to connection partner widget
 */
void CPipeItemWidget::setConnectionPartner( ushort usInputPort, CPipeItemWidget* partnerPtr )
  throw()
{
  inputConnectorsPtrVec[usInputPort]->setPartner( partnerPtr );
}

/**
 * Sets the widget title
 * \param sTitle_ new widget title
 */
void CPipeItemWidget::setTitle( const std::string sTitle_ ) throw()
{
  sTitle = sTitle_;
}

/**
 * Sets the corresponding CPipelineItem
 * \param itemPtr_ corresponding CPipelineItem
 */
void CPipeItemWidget::setItem( boost::shared_ptr<CPipelineItem> itemPtr_ ) throw()
{
  itemPtr = itemPtr_;
  for ( int i = 0; i < usFanIn; i++ )
  {
    switch ( itemPtr->getInputType(i) )
    {
      case CPipelineItem::IOPoint:
        inputConnectorsPtrVec[i]->setColor( Qt::white ); break;
      case CPipelineItem::IO1DInteger:
        inputConnectorsPtrVec[i]->setColor( Qt::magenta ); break;
      case CPipelineItem::IO1DFloat:
        inputConnectorsPtrVec[i]->setColor( Qt::yellow ); break;
      case CPipelineItem::IO1DVector:
        inputConnectorsPtrVec[i]->setColor( Qt::cyan ); break;
      case CPipelineItem::IO2DInteger:
        inputConnectorsPtrVec[i]->setColor( Qt::darkRed ); break;
      case CPipelineItem::IO2DFloat:
        inputConnectorsPtrVec[i]->setColor( Qt::darkGreen ); break;
      case CPipelineItem::IO3DInteger:
        inputConnectorsPtrVec[i]->setColor( Qt::darkMagenta ); break;
      case CPipelineItem::IO3DFloat:
        inputConnectorsPtrVec[i]->setColor( Qt::darkYellow ); break;
      case CPipelineItem::IO2DVector:
        inputConnectorsPtrVec[i]->setColor( Qt::darkBlue ); break;
      case CPipelineItem::IO3DVector:
        inputConnectorsPtrVec[i]->setColor( Qt::darkCyan ); break;
      case CPipelineItem::IOInteger:
        inputConnectorsPtrVec[i]->setColor( Qt::red ); break;
      case CPipelineItem::IOFloat:
        inputConnectorsPtrVec[i]->setColor( Qt::green ); break;
      case CPipelineItem::IOVector:
        inputConnectorsPtrVec[i]->setColor( Qt::blue ); break;
      case CPipelineItem::IOOther:
        inputConnectorsPtrVec[i]->setColor( Qt::black ); break;
    }// END switch
  }// END for
  for ( int i = 0; i < usFanOut; i++ )
  {
    cerr << i << endl;        
    switch ( itemPtr_->getOutputType(i) )
    {
      case CPipelineItem::IOPoint:
        outputConnectorsPtrVec[i]->setColor( Qt::white ); break;
      case CPipelineItem::IO1DInteger:
        outputConnectorsPtrVec[i]->setColor( Qt::magenta ); break;
      case CPipelineItem::IO1DFloat:
        outputConnectorsPtrVec[i]->setColor( Qt::yellow ); break;
      case CPipelineItem::IO1DVector:
        outputConnectorsPtrVec[i]->setColor( Qt::cyan ); break;
      case CPipelineItem::IO2DInteger:
        outputConnectorsPtrVec[i]->setColor( Qt::darkRed ); break;
      case CPipelineItem::IO2DFloat:
        outputConnectorsPtrVec[i]->setColor( Qt::darkGreen ); break;
      case CPipelineItem::IO3DInteger:
        outputConnectorsPtrVec[i]->setColor( Qt::darkMagenta ); break;
      case CPipelineItem::IO3DFloat:
        outputConnectorsPtrVec[i]->setColor( Qt::darkYellow ); break;
      case CPipelineItem::IO2DVector:
        outputConnectorsPtrVec[i]->setColor( Qt::darkBlue ); break;
      case CPipelineItem::IO3DVector:
        outputConnectorsPtrVec[i]->setColor( Qt::darkCyan ); break;
      case CPipelineItem::IOInteger:
        outputConnectorsPtrVec[i]->setColor( Qt::red ); break;
      case CPipelineItem::IOFloat:
        outputConnectorsPtrVec[i]->setColor( Qt::green ); break;
      case CPipelineItem::IOVector:
        outputConnectorsPtrVec[i]->setColor( Qt::blue ); break;
      case CPipelineItem::IOOther:
        outputConnectorsPtrVec[i]->setColor( Qt::black ); break;
    }//END switch
  }//END for
}

/**
 * Reimplemented from QWidget
 * Paints the item
 * \param QPaintEvent* not used
 */
void CPipeItemWidget::paintEvent( QPaintEvent* ) throw()
{
  QFont myFont ( "Helvetica", 10 );
  QPainter p( this );
  QString sTmp( itemPtr->getModuleName().c_str() );
  p.setPen( Qt::black );
  p.drawRect( 0, 0, width(), height() );
  p.drawRect( 10, 10, width() - 20, height() - 20 );
  p.setFont( myFont );
  p.drawText( 15, 22, sTitle.substr(0,14).c_str() );
  p.end();
}

/**
 * Reimplemented from QWidget
 * Reacts on mouse events, which will be ignored (== transferred to connectors) in this case
 * \param eventPtr Qt mouse event
 */
void CPipeItemWidget::mousePressEvent( QMouseEvent* eventPtr ) throw()
{
  eventPtr->ignore();
}
