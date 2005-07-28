/************************************************************************
 * File: cconnectorwidget.h                                             *
 * Project: AIPS                                                        *
 * Description: A widget for module connectors                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 10.10.03                                                    *
 * Changed: 22.01.04 Began to overwork and document the source code     *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CCONNECTORWIDGET_H
#define CCONNECTORWIDGET_H

// AIPS includes
#include "cpipeitemwidget.h"

// Qt includes
#include <qwidget.h>
#include <qpainter.h>

/**
 * A widget for module connectors
 */
class CConnectorWidget : public QWidget
{
   Q_OBJECT
private:
  /// Standard constructor
  CConnectorWidget();
  /// Copy constructor
  CConnectorWidget( CConnectorWidget& );
  /// Assignment operator
  CConnectorWidget& operator=( CConnectorWidget& );
public:
/* Structors */
  /// Constructor
  CConnectorWidget(bool bIsOutput_, CPipeItemWidget* parentPtr_, const char *sName = NULL )
    throw();
  /// Destructor
  ~CConnectorWidget()
    throw();
/* Accessors */
  /// Returns the parent item of the connector
  CPipeItemWidget* getParent() const
    throw();
  /// Returns the partner item of the connector
  CPipeItemWidget* getPartner() const
    throw();
  /// Returns the input number of the connector
  ushort getInputNumber() const
    throw();
  /// Returns the output number of the connector
  ushort getOutputNumber() const
    throw();
/* Mutators */
  /// Sets the partner item of the connector
  void setPartner( const CPipeItemWidget* newPartnerPtr )
    throw();
  /// Sets the input number of the connector
  void setInputNumber( const ushort usInputNumber_ )
    throw(); 
  /// Sets the output number of the connector
  void setOutputNumber( const ushort usOutputNumber_ )
    throw(); 
  /// Sets the color of the connector
  void setColor( const QColor& aColor_ )
    throw();
/* Other functions */    
  /// Returns true if connector is an output connector
  bool isOutput() const
    throw();
signals:
  /// Signals the connector has been clicked with the left mousebutton
  void connectorClicked( CConnectorWidget* );
  /// Signals the input connector has been clicked with the right mousebutton
  void connectionDeleted( CConnectorWidget* );
protected:
  /// Overwritten from QWidget
  virtual void paintEvent( QPaintEvent* ) throw();
  /// Overwritten from QWidget
  virtual void mousePressEvent( QMouseEvent* eventPtr ) throw();
private:
  bool bIsOutput;              ///< Is this an output connection?
  CPipeItemWidget* parentPtr;  ///< Parent of this connector
  CPipeItemWidget* partnerPtr; ///< Connection partner (if any)
  ushort usInputNumber;        ///< ID of this input connector in input list 
  ushort usOutputNumber;       ///< ID of this output connector in output list 
  QColor aColor;               ///< Color of the connector (depends on data type)
};

#endif
