/************************************************************************
 * File: cevent.cpp                                                     *
 * Project: AIPS                                                        *
 * Description: Some classes representing events                        *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2005-03-28                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cevent.h"

using namespace std;
using namespace aips;
using boost::lexical_cast;

/********************************************************************************************************
 * CEvent                                                                                               *
 ********************************************************************************************************/

/*************
 * Structors *
 *************/

/**
 * \param generator_ the subject that generated the actual event
 * \param uiEventType_ the type identifier of the actual event
 * \param className_ name of the class (type information)
 * \param classVersion_ version number of the class (type information)
 * \param derivedFrom_ name of the classes this class is derived from (type information)
 * \throws NullException if generator_ is NULL since each event needs a generator
 */
CEvent::CEvent( CSubject* generator_, uint uiEventType_, const string& className_, const string& classVersion_,
	const string& derivedFrom_ ) throw( NullException )
 : CBase( className_, classVersion_, derivedFrom_ ), generator( generator_ ), uiEventType( uiEventType_ )
{
	if( generator == NULL )
		throw( NullException( SERROR("Generator subject is NULL"), CException::FATAL, ERR_CALLERNULL ) );
}

CEvent::~CEvent() throw()
{
}

/** \param anEvent event to copy */
CEvent::CEvent( const CEvent& anEvent ) throw()
	: CBase( "CEvent", "0.1", "CBase" ), generator( anEvent.generator ), uiEventType( anEvent.uiEventType )
{
}

/*************
 * Operators *
 *************/
 
/** \param anEvent event to copy */
CEvent CEvent::operator=( const CEvent& anEvent ) throw()
{
	if ( &anEvent != this )
	{
		this->generator = anEvent.generator;
		this->uiEventType = anEvent.uiEventType;
	}
	return *this;
}

/*************
 * Accessors *
 *************/
 
/** \returns the generator subject of the actual event */
CSubject* CEvent::getGenerator() const throw()
{
	return generator;
}

/** \returns the type identifier of the actual event */
uint CEvent::getType() const throw()
{
	return uiEventType;
}

/*****************
 * Other methods *
 *****************/

/**
 * \returns information string
 */
const std::string CEvent::dump() const throw()
{
	return "Generator " + lexical_cast<string>( ulong(generator) )+ ", event type " +lexical_cast<string>( uiEventType )
		+ "\n" + CBase::dump();
}

/********************************************************************************************************
 * CNewLogEvent                                                                                         *
 ********************************************************************************************************/

/*************
 * Structors *
 *************/
 
/**
 * \param generator_ the subject that generated the actual event
 * \param sLogString_ the string that was just registered by the logging system
 * \throws NullException if generator_ is NULL since each event needs a generator
 */
CNewLogEvent::CNewLogEvent( CSubject* generator_, const std::string& sLogString_ ) throw( NullException )
	: CEvent( generator_, ENewLogEvent, "CNewLogEvent", "0.3", "CEvent" ),
		sLogString( sLogString_ )
{
	if( generator_ == NULL )
		throw( NullException( SERROR("Generator subject is NULL"), CException::FATAL, ERR_CALLERNULL ) );
}

/** \param anEvent event to copy */
CNewLogEvent::CNewLogEvent( const CNewLogEvent& anEvent ) throw()
	: CEvent( anEvent.getGenerator(), ENewLogEvent, "CNewLogEvent", "0.3", "CEvent" ),
		sLogString( anEvent.sLogString )
{
}

CNewLogEvent::~CNewLogEvent() throw()
{
}

/*************
 * Operators *
 *************/

/** \param anEvent event to copy */
CNewLogEvent CNewLogEvent::operator=( const CNewLogEvent& anEvent ) throw()
{
	if ( this != &anEvent )
		this->sLogString = anEvent.sLogString;
	return *this;
}

/*************
 * Accessors *
 *************/

/** \returns the actual logging string */
std::string CNewLogEvent::getLogString()
{
	return sLogString;
}

/*****************
 * Other methods *
 *****************/

/**
 * \returns information string
 */
const std::string CNewLogEvent::dump() const throw()
{
	return "LogString <" + sLogString + ">"	+ "\n" + CEvent::dump();
}

/********************************************************************************************************
 * CDataChangedEvent                                                                                    *
 ********************************************************************************************************/

/*************
 * Structors *
 *************/
 
/**
 * \param generator_ the subject that generated the actual event
 * \throws NullException if generator_ is NULL since each event needs a generator
 */
CDataChangedEvent::CDataChangedEvent( CSubject* generator_ ) throw( NullException )
	: CEvent( generator_, EDataChangedEvent, "CDataChangedEvent", "0.1", "CEvent" )
{
	if( generator_ == NULL )
		throw( NullException( SERROR("Generator subject is NULL"), CException::FATAL, ERR_CALLERNULL ) );
}

/** \param anEvent event to copy */
CDataChangedEvent::CDataChangedEvent( const CDataChangedEvent& anEvent ) throw()
	: CEvent( anEvent.getGenerator(), EDataChangedEvent, "CDataChangedEvent", "0.1", "CEvent" )
{
}

CDataChangedEvent::~CDataChangedEvent() throw()
{
}

#ifndef NOPROGRESS

/********************************************************************************************************
 * CProgressStartEvent                                                                                  *
 ********************************************************************************************************/

/*************
 * Structors *
 *************/
 
/**
 * \param generator_ the subject that generated the actual event
 * \param uiMaxProgress_ maximum number of progress steps
 * \throws NullException if generator_ is NULL since each event needs a generator
 */
CProgressStartEvent::CProgressStartEvent( CSubject* generator_, uint uiMaxProgress_ ) throw( NullException )
	: CEvent( generator_, EProgressStartEvent, "CProgressStartEvent", "0.3", "CEvent" ), uiMaxProgress( uiMaxProgress_ )
{
	if( generator_ == NULL )
		throw( NullException( SERROR("Generator subject is NULL"), CException::FATAL, ERR_CALLERNULL ) );
}

/** \param anEvent event to copy */
CProgressStartEvent::CProgressStartEvent( const CProgressStartEvent& anEvent ) throw()
	: CEvent( anEvent.getGenerator(), EProgressStartEvent, "CProgressStartEvent", "0.3", "CEvent" ),
		uiMaxProgress( anEvent.uiMaxProgress )
{
}

CProgressStartEvent::~CProgressStartEvent() throw()
{
}

/*************
 * Operators *
 *************/

/** \param anEvent event to copy */
CProgressStartEvent CProgressStartEvent::operator=( const CProgressStartEvent& anEvent ) throw()
{
	if ( this != &anEvent )
		this->uiMaxProgress = anEvent.uiMaxProgress;
	return *this;
}

/*************
 * Accessors *
 *************/

/** \returns the actual maximum progress value */
uint CProgressStartEvent::getMaxProgress() const throw()
{
	return uiMaxProgress;
}

/*****************
 * Other methods *
 *****************/

/**
 * \returns information string
 */
const std::string CProgressStartEvent::dump() const throw()
{
	return "uiMaxProgress " + lexical_cast<string>( uiMaxProgress )	+ "\n" + CEvent::dump();
}

/********************************************************************************************************
 * CProgressResetEvent                                                                                  *
 ********************************************************************************************************/

/*************
 * Structors *
 *************/
 
/**
 * \param generator_ the subject that generated the actual event
 * \throws NullException if generator_ is NULL since each event needs a generator
 */
CProgressResetEvent::CProgressResetEvent( CSubject* generator_ ) throw( NullException )
	: CEvent( generator_, EProgressResetEvent, "CProgressResetEvent", "0.1", "CEvent" )
{
	if( generator_ == NULL )
		throw( NullException( SERROR("Generator subject is NULL"), CException::FATAL, ERR_CALLERNULL ) );
}

/** \param anEvent event to copy */
CProgressResetEvent::CProgressResetEvent( const CProgressResetEvent& anEvent ) throw()
	: CEvent( anEvent.getGenerator(), EProgressResetEvent, "CProgressResetEvent", "0.1", "CEvent" )
{
}

CProgressResetEvent::~CProgressResetEvent() throw()
{
}

/********************************************************************************************************
 * CProgressEvent                                                                                       *
 ********************************************************************************************************/

/*************
 * Structors *
 *************/
 
/**
 * \param generator_ the subject that generated the actual event
 * \param uiValue_ actual progress number
 * \throws NullException if generator_ is NULL since each event needs a generator
 */
CProgressEvent::CProgressEvent( CSubject* generator_, uint uiValue_ ) throw( NullException )
	: CEvent( generator_, EProgressEvent, "CProgressEvent", "0.3", "CEvent" ), uiValue( uiValue_ )
{
	if( generator_ == NULL )
		throw( NullException( SERROR("Generator subject is NULL"), CException::FATAL, ERR_CALLERNULL ) );
}

/** \param anEvent event to copy */
CProgressEvent::CProgressEvent( const CProgressEvent& anEvent ) throw()
	: CEvent( anEvent.getGenerator(), EProgressEvent, "CProgressEvent", "0.3", "CEvent" ), uiValue( anEvent.uiValue )
{
}

CProgressEvent::~CProgressEvent() throw()
{
}

/*************
 * Operators *
 *************/

/** \param anEvent event to copy */
CProgressEvent CProgressEvent::operator=( const CProgressEvent& anEvent )	throw()
{
	this->uiValue = anEvent.uiValue;
	return *this;
}

/*************
 * Accessors *
 *************/

/** \returns the actual progress value */
uint CProgressEvent::getValue() const throw()
{
	return uiValue;
}

/*****************
 * Other methods *
 *****************/

/**
 * \returns information string
 */
const std::string CProgressEvent::dump() const throw()
{
	return "uiValue " + lexical_cast<string>( uiValue )	+ "\n" + CEvent::dump();
}
#endif
