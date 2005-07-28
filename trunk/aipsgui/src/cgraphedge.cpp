/************************************************************************
 * File: cgraphedge.cpp                                                 *
 * Project: AIPS                                                        *
 * Description: Representation of an processing graph edge              *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 24.10.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#include "cgraphedge.h"
#include "aipsnumeric.h"

using namespace std;
using namespace aips;

/*************
 * Structors *
 *************/

/**
 * \param sourcePtr_ Source module in processing graph
 * \param targetPtr_ Target module in processing graph
 * \param usInputNumber_ Target input port number
 * \param usOutputNumber_ Target output port number
 */
CGraphEdge::CGraphEdge( boost::shared_ptr<CPipelineItem> sourcePtr_, boost::shared_ptr<CPipelineItem> targetPtr_,
  const ushort usInputNumber_, const ushort usOutputNumber_ ) throw()
  : CBase ( "CGraphEdge", "0.3", "CBase" ), sourcePtr( sourcePtr_ ), targetPtr( targetPtr_ ),
  usInputNumber( usInputNumber_ ), usOutputNumber( usOutputNumber_ ), bActive( false )
{
}

/** Destructor */
CGraphEdge::~CGraphEdge() throw()
{
}

/*************
 * Accessors *
 *************/

/** \returns the activity state of the edge */
const bool CGraphEdge::getActive() const throw()
{
  return bActive;
}

/**
 * \returns the source module
 * \exception NullException if there is no actual source module set
 */
boost::shared_ptr<CPipelineItem> CGraphEdge::getSource() const throw( NullException )
{
  if ( sourcePtr == NULL )
    throw( NullException( SERROR( "sourcePtr is NULL" ), CException::RECOVER, ERR_REQUESTNULL ) );
  return sourcePtr;
} 

/**
 * \returns the target module
 * \exception NullException if there is no actual target module set
 */
boost::shared_ptr<CPipelineItem> CGraphEdge::getTarget() const throw( NullException )
{
  if ( targetPtr == NULL )
    throw( NullException( SERROR( "targetPtr is NULL" ), CException::RECOVER, ERR_REQUESTNULL ) );
  return targetPtr;
}

/** \returns the input number of the target module */
ushort CGraphEdge::getTargetInputNumber() const throw()
{
  return usInputNumber;
}

/** \returns the output number of the source module */
ushort CGraphEdge::getSourceOutputNumber() const throw()
{
  return usOutputNumber;
}

/************
 * Mutators *
 ************/

/**
 * \param aNodePtr the new source module
 * \exception NullException if aNode is NULL
 */
void CGraphEdge::setSource( boost::shared_ptr<CPipelineItem> aNodePtr ) throw( NullException )
{
  if ( aNodePtr == NULL )
    throw( NullException( SERROR( "aNodePtr is NULL" ), CException::RECOVER, ERR_CALLERNULL ) );
  sourcePtr = aNodePtr;
}

/**
 * \param aNodePtr the new target module
 * \exception NullException if aNode is NULL
 */
void CGraphEdge::setTarget( boost::shared_ptr<CPipelineItem> aNodePtr ) throw( NullException )
{
  if ( aNodePtr == NULL )
    throw( NullException( SERROR( "aNodePtr is NULL" ), CException::RECOVER, ERR_CALLERNULL ) );
  targetPtr = aNodePtr;
}

/**
 * \param bActive_ new activity state
 */
void CGraphEdge::setActive( bool bActive_ ) throw()
{
  bActive = bActive_;
  if ( bActive == true )
  {
DBG3("Setting input " << targetPtr->getID() << "(" << usInputNumber << ") from "
<< sourcePtr->getID() << "(" << usOutputNumber << ")");
    targetPtr->setInput( sourcePtr->getOutput( usOutputNumber ),
			usInputNumber );
  }
}

/*****************
 * Other methods *
 *****************/

const string CGraphEdge::dump() const throw()
{
  ostringstream os;
  os << " sourcePtr " << static_cast<void*>( sourcePtr.get() )
    << " theTarget " << static_cast<void*>( targetPtr.get() )
    << "\n usINumber " << usInputNumber << " usONumber " << usOutputNumber << "\n";
  return CBase::dump() + os.str();
}
