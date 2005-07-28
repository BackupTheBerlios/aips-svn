/************************************************************************
 * File: cgraphedge.h                                                   *
 * Project: AIPS                                                        *
 * Description: Representation of an processing graph edge              *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Alpha                                                       *
 * Created: 24.10.03                                                    *
 * Changed: 24.10.03 Creation of class                                  *
 *          09.11.03 Code is now conform with coding scheme             *
 *                   Moved code into namespace aips                     *
 *          13.11.03 Added documentation                                *
 *          20.01.04 Made the code look prettier                        *
 *          01.05.04 Sorted code and updated documentation              *
 *        2004-11-22 Class now uses boost::shared_ptr                   *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CGRAPHEDGE_H
#define CGRAPHEDGE_H

// AIPS includes
#include "cpipelineitem.h"
#include "clog.h"

namespace aips {

class CPipelineItem;

/** Representation of an processing graph edge */
class CGraphEdge : public CBase
{
public:
/* Structors */
  /// Constructor
  CGraphEdge( boost::shared_ptr<CPipelineItem> sourcePtr_, boost::shared_ptr<CPipelineItem> targetPtr_,
    const ushort usInputNumber_, const ushort usOutputNumber_ ) throw();
  /// Destructor
  virtual ~CGraphEdge()
    throw();
/* Accessors */
  /// Returns the activity state of the edge
  const bool getActive() const
    throw();
  /// Returns the source module
  boost::shared_ptr<CPipelineItem> getSource() const
    throw( NullException ); 
  /// Returns the target module
  boost::shared_ptr<CPipelineItem> getTarget() const
    throw( NullException ); 
  /// Returns the input number of the target module
  ushort getTargetInputNumber() const
    throw(); 
  /// Returns the output number of the source module
  ushort getSourceOutputNumber() const
    throw();		
/* Mutators */
  /// Sets the source module of the edge
  void setSource( boost::shared_ptr<CPipelineItem> aNodePtr )
    throw( NullException ); 
  /// Sets the target module of the edge
  void setTarget( boost::shared_ptr<CPipelineItem> aNodePtr )
    throw( NullException );
  /// Set activity state of the edge
  void setActive( bool bActive_ )
    throw();
/* Other methods */
  /// Reimplemented from CBase
  virtual const std::string dump() const
    throw();
private:
  boost::shared_ptr<CPipelineItem> sourcePtr; ///< Source item
  boost::shared_ptr<CPipelineItem> targetPtr; ///< Target item
  ushort usInputNumber;     ///< Input port used
  ushort usOutputNumber;    ///< Output port used
  bool bActive;             ///< Edge transports legal data
};

}
#endif
