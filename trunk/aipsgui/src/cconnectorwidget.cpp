/************************************************************************
 * File: cconnectorwidget.cpp                                           *
 * Project: AIPS                                                        *
 * Description: A widget for module connectors                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 10.10.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cconnectorwidget.h"

using namespace std;

/**
 * Constructor
 * \param bIsOutput_ determines if connector is output (true) or input (false)
 * \param parentPtr_ pointer to parent module
 * \param sName Qt object name
 */
CConnectorWidget::CConnectorWidget( bool bIsOutput_, CPipeItemWidget* parentPtr_,
  const char *sName ) throw()
  : QWidget( parentPtr_, sName ), bIsOutput( bIsOutput_ ),
    parentPtr( parentPtr_ ), partnerPtr( NULL )
{
  setFixedSize( 20, 10 );
}

/** Destructor */
CConnectorWidget::~CConnectorWidget() throw()
{
}

/** \returns the parent item of the connector */
CPipeItemWidget* CConnectorWidget::getParent() const throw()
{
  return parentPtr;
}

/** \returns the partner item of the connector */
CPipeItemWidget* CConnectorWidget::getPartner() const throw()
{
  return partnerPtr;
}

/* \returns the output number of the connector */
ushort CConnectorWidget::getOutputNumber() const throw()
{
  return usOutputNumber;
}

/* \returns the input number of the connector */
ushort CConnectorWidget::getInputNumber() const throw()
{
  return usInputNumber;
}

/**
 * Sets the partner item of the connector
 * \param newPartnerPtr new connection partner
 */
void CConnectorWidget::setPartner( const CPipeItemWidget* newPartnerPtr ) throw()
{
  partnerPtr = const_cast<CPipeItemWidget*>( newPartnerPtr );
}

/**
 * Sets the input number of the connector
 * \param usInputNumber_ input port number
 */
void CConnectorWidget::setInputNumber( const ushort usInputNumber_ ) throw()
{
  usInputNumber = usInputNumber_;
}

/**
 * Sets the output number of the connector
 * \param usOutputNumber_ output port number
 */
void CConnectorWidget::setOutputNumber( const ushort usOutputNumber_ ) throw()
{
  usOutputNumber = usOutputNumber_;
}

/**
 * Sets the color of the connector
 * \param aColor_ the new color of the connector
 */
void CConnectorWidget::setColor( const QColor& aColor_ ) throw()
{
  aColor = aColor_;
}

/** \returns true if connector is an output connector */
bool CConnectorWidget::isOutput() const throw()
{
  return bIsOutput;
}

/**
 * Reimplemented from QWidget.
 * Draws the connector
 * \param QPaintEvent* not used
 */
void CConnectorWidget::paintEvent( QPaintEvent* ) throw()
{
  QPainter p( this );
  p.fillRect( 0, 0, 20, 10, aColor );
  p.end();  
}

/**
 * Reimplemented from QWidget.
 * Clicking the left button results in signal connectorClicked()
 * Clicking the right button on an input connector results in signal connectorDeleted()
 * \param eventPtr a Qt mouse event
 */
void CConnectorWidget::mousePressEvent( QMouseEvent* eventPtr ) throw()
{
  if ( eventPtr->button() == LeftButton )
    emit( connectorClicked( this ) );
  if ( eventPtr->button() == RightButton && !bIsOutput )
    emit( connectionDeleted( this ) );
}
