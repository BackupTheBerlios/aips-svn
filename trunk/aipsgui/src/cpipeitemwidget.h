/************************************************************************
 * File: cpipeitemwidget.h                                              *
 * Project: AIPS                                                        *
 * Description: A widget to display pipeline modules                    *
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

#ifndef CPIPEITEMWIDGET_H
#define CPIPEITEMWIDGET_H

// AIPS includes
#include <cpipelineitem.h>

// Qt includes
#include <qwidget.h>
#include <qpainter.h>


using namespace aips;

/** A widget to display pipeline modules */
class CConnectorWidget;
                  
class CPipeItemWidget : public QWidget
{
  Q_OBJECT
private:
  /// Standard constructor
  CPipeItemWidget();
  /// Copy constructor
  CPipeItemWidget( CPipeItemWidget& );
  /// Assignment operator
  CPipeItemWidget& operator=( CPipeItemWidget& );
public:
/* Structors */
  /// Constructor
  CPipeItemWidget( ulong ulID_, std::string sTitle_, ushort usFanIn_, ushort usFanOut_,
    ulong ulType_, ulong ulLib_, ulong ulModule_, QWidget* parentPtr = NULL, const char* sName =
NULL	) throw();
  /// Destructor
  virtual ~CPipeItemWidget() throw();
/* Accessors */
  /// Returns the corresponding pipeline item
  boost::shared_ptr<CPipelineItem> getItem() const throw();
  /// Returns the pipeline item type
  const ulong getType() const throw();
	const ulong getLib() const throw();
	const ulong getModule() const throw();
  /// Returns the pipeline item fanin
  const ushort getFanIn() const throw();
  /// Returns the pipeline item fanout
  const ushort getFanOut() const throw();
  /// Returns the pipeline item id
  const ulong getID() const throw();
  /// Returns the widget title
  const std::string getTitle() const throw();
/* Mutators */  
  /// Sets the connection partner for the given input port
  void setConnectionPartner( ushort usInputPort, CPipeItemWidget* partnerPtr ) throw();
  /// Sets the corresponding CPipelineItem
  void setItem( boost::shared_ptr<CPipelineItem> anItem ) throw();
  /// Sets the widget title
  void setTitle( const std::string sTitle_ ) throw();
/* Other methods */
signals:
  /// Signals a connector has been clicked
  void connectorClicked( CConnectorWidget* );
  /// Signals a connection has been deleted
  void connectionDeleted( CConnectorWidget* );
protected:
  /// Reimplemented from QWidget
  virtual void paintEvent( QPaintEvent* ) throw();
  /// Reimplemented from QWidget
  virtual void mousePressEvent( QMouseEvent* eventPtr ) throw();
private:
  std::vector<CConnectorWidget*> inputConnectorsPtrVec;  ///< All input connector widgets
  std::vector<CConnectorWidget*> outputConnectorsPtrVec; ///< All output connector widgets
  boost::shared_ptr<CPipelineItem> itemPtr; ///< Corresponding CPipelineItem
  ulong ulID;             ///< Item id
  std::string sTitle;     ///< Widget title
  ushort usFanIn;         ///< Item fanin
  ushort usFanOut;        ///< Item fanout
  ulong ulType;           ///< Item type
	ulong ulLib;
	ulong ulModule;
};

#endif
