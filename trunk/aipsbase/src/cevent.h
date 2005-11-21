/************************************************************************
 * File: cevent.h                                                       *
 * Project: AIPS                                                        *
 * Description: Some classes representing events                        *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status:  Beta                                                        *
 * Created: 2005-03-28                                                  *
 * Changed: 2005-03-30 Corrected some minor bugs.                       *
 *                     Added getType() method and supporting stuff      *
 *          2005-03-31 Added documentation                              *
 *          2005-03-31 Updated documentation                            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSCEVENT_H
#define AIPSCEVENT_H
#define AIPSCEVENT_VERSION "0.4"

// Standard includes
#include <string>
#include <iostream>
// Boost includes
#include <boost/shared_ptr.hpp>
// AIPS includes
#include <cexception.h>

namespace aips
{

class CSubject;

/**
 * \brief A base class for more specific events.
 *
 * You can use this if no derived class seems to be appropiate and you don't want to create a new derived class either.
 * \todo Write test cases and example code
 * \todo Add missing dump functions
 * \todo By some way destroy that nasty log-event-mutual exclusion circle
 */
class CEvent : public CBase
{
private:
	CEvent(); // Events always require a generator subject so this is private.
public:
	/// Some basic event types which are defined within this header file
	enum EventType {
		ENone = 0,
		ENewLogEvent = 1,
		EDataChangedEvent = 100,
#ifndef NOPROGRESS		
		EProgressEvent = 200, EProgressStartEvent = 201, EProgressResetEvent = 202
#endif		
	};
/** \name Structors */
  //@{
	/// Constructor
  CEvent( CSubject* generator_, uint uiEventType_, const std::string& className_ = "CEvent",
		const std::string& classVersion_ = AIPSCEVENT_VERSION,
    const std::string& derivedFrom_ = "CBase" ) throw( NullException );
	/// Copy constructor
	CEvent( const CEvent& anEvent )
		throw();
	/// Destructor
  ~CEvent()
  	throw();
  //@} 
/** \name Operators */
  //@{
	/// Assignment operator  
	CEvent operator=( const CEvent& anEvent )
		throw();
  //@}  
/** \name Accessors */
  //@{
	/// Returns the generator subject of this event	
	CSubject* getGenerator() const
		throw();
	/// Returns the type id of this event
	uint getType() const
		throw();
  //@}  
/** \name Other methods */
  //@{
  /// Produces an information string about the actual object. This is mainly meant for debugging purposes
  virtual const std::string dump() const
    throw();
  //@}
private:
	CSubject* generator; ///< Generator subject of the event
	uint uiEventType;    ///< Type identifier of the actual event
};

typedef boost::shared_ptr<CEvent> TEventPtr;

/**
 * \brief A class to represent a new logging event.
 *
 * This event is emitted each time a new log string is written to the log buffer.
 */
class CNewLogEvent : public CEvent
{
private:
	CNewLogEvent(); // Events always require a generator subject so this is private.
public:
/** \name Structors */
	/// Constructor
  CNewLogEvent( CSubject* generator_, const std::string& sLogString_ )
  	throw( NullException );
  /// Copy constructor
	CNewLogEvent( const CNewLogEvent& anEvent )
		throw();
	/// Destructor
	~CNewLogEvent()
		throw();
/** \name  Operators */
	/// Assignment operator	
	CNewLogEvent operator=( const CNewLogEvent& anEvent )
		throw();
/** \name  Accessors */
	/// Returns the actual string that was logged 
	std::string getLogString();
/** \name  Other methods */	
  /// Produces an information string about the actual object. This is mainly meant for debugging purposes
  virtual const std::string dump() const
    throw();
private:
	std::string sLogString; ///< The string that was just written by the logging system
};

/**
 * \brief An event that gets emitted each time that a change of data occured
 */
class CDataChangedEvent : public CEvent
{
private:
	CDataChangedEvent(); // Events always require a generator subject so this is private.
public:
/** \name Structors */
  //@{
	/// Constructor
  CDataChangedEvent( CSubject* generator_ )
  	throw( NullException );
  /// Copy constructor
	CDataChangedEvent( const CDataChangedEvent& anEvent )
		throw();
	/// Destructor
  ~CDataChangedEvent()
  	throw();
  //@} 
};

#ifndef NOPROGRESS
/**
 * \brief An event to signal that some processing has started
 */
class CProgressStartEvent : public CEvent
{
private:
	CProgressStartEvent(); // Events always require a generator subject so this is private.
public:
/** \name  Structors */
  //@{
	/// Constructor
	CProgressStartEvent( CSubject* generator_, uint uiMaxProgress_ )
		throw( NullException );
	/// Copy constructor
	CProgressStartEvent( const CProgressStartEvent& anEvent )
		throw();
	/// Destructor
  ~CProgressStartEvent() throw();
  //@}
/** \name  Operators */
  //@{
	/// Assignment operator	  
  CProgressStartEvent operator=( const CProgressStartEvent& anEvent )
  	throw();
  //@} 
/** \name  Accessors */
  //@{
	/// Returns the maximum number for progression  	
	uint getMaxProgress() const
   throw();
  //@}
/** \name  Other methods */
  //@{
  /// Produces an information string about the actual object. This is mainly meant for debugging purposes
  virtual const std::string dump() const
    throw();
  //@}
private:
	uint uiMaxProgress; ///< The maximum number for progression
};

/**
 * \brief An event to signal that some processing has ended
 */
class CProgressResetEvent : public CEvent
{
private:
	CProgressResetEvent(); // Events always require a generator subject so this is private.
public:
/** \name  Structors */
  //@{
  /// Constructor
  CProgressResetEvent( CSubject* generator_ )
  	throw( NullException );
	/// Copy constructor
	CProgressResetEvent( const CProgressResetEvent& anEvent )
		throw();
	/// Destructor
  ~CProgressResetEvent()
  	throw();
  //@} 
};

/**
 * \brief An event to signal that some processing is in progress and just got actualized
 */
class CProgressEvent : public CEvent
{
private:
	CProgressEvent(); // Events always require a generator subject so this is private.
public:
/** \name  Structors */
  //@{
	/// Constructor
  CProgressEvent( CSubject* generator_, uint uiValue_ )
  	throw( NullException );
	/// Copy constructor
	CProgressEvent( const CProgressEvent& anEvent )
		throw();
	/// Destructor
  ~CProgressEvent()
    throw();
  //@}
/** \name  Operators */
  //@{
	/// Assignment operator	  
	CProgressEvent operator=( const CProgressEvent& anEvent )
		throw();
  //@}  
/** \name  Accessors */
  //@{
	/// Returns the actual progression value
	uint getValue() const
   throw();
  //@} 
/** \name  Other methods */
  //@{
  /// Produces an information string about the actual object. This is mainly meant for debugging purposes
  virtual const std::string dump() const
    throw();
  //@}
private:
	uint uiValue; ///< The actual progression value
};
#endif

};

#endif
