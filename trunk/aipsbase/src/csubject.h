/************************************************************************
 * File: csubject.h                                                     *
 * Project: AIPS                                                        *
 * Description: An subject base class implementing the                  *
 *              "Subject/Observer"-Pattern                              *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status:  Beta                                                        *
 * Created: 2005-03-28                                                  *
 * Changed: 2005-03-30 Corrected some minor bugs.                       *
 *          2005-03-31 Added documentation                              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CSUBJECT_H
#define CSUBJECT_H

#define CSUBJECT_VERSION "0.3"

// Standard includes
#include <iostream>
#include <list>
#include <algorithm>

// AIPS includes
#include <aipsmacros.h>
#include <cobserver.h>
#include <clog.h>

namespace aips
{

/**
 * An subject base class implementing the "Subject/Observer"-Pattern
 */
class CSubject : public CBase
{
public:
/* Structors */
	/// Constructor
  CSubject( const std::string& className_ = "CSubject",	
  	const std::string& classVersion_ = CSUBJECT_VERSION,
  	const std::string& derivedFrom_ = "CBase" ) throw();
  /// Copy constructor
	CSubject( const CSubject& );
  /// Destructor
  ~CSubject()
  	throw();
/* Operators */
	/// Assignment operator  	
	CSubject operator=( const CSubject& );
/* Other methods */  	
  /// Attaches an observer to the observer list
	void attachObserver( CObserver* const anObserverPtr, uint uiInterestType )
		throw( NullException );
	/// Removes an observer to the observer list
	void detachObserver( CObserver* const anObserverPtr )
		throw( NullException );
	/// Notifies all registered observers with the appropiate interest that a new event has occured in the subject
	void notify( boost::shared_ptr<CEvent> anEventSPtr )
		throw( NullException );
private:
	/// A struct representing a tuple of an observer and an event type which it is interested in
	struct SObserverListEntry
	{
		/// Constructor
		SObserverListEntry( CObserver* anObserverPtr_,	uint uiInterestType_ ) throw();
		/* Data members */		
		CObserver* const anObserverPtr; ///< Registered observer
		uint uiInterestType;         ///< Type identifier for observed interest
	};
	/// Binary predicate functor to enable searching for specific observers in a list
	struct CheckIdentityFunctor : public std::binary_function<SObserverListEntry, CObserver*, bool>
	{
		bool operator()( SObserverListEntry anEntry, CObserver* anObserverPtr ) const;
	};
	
	std::list<SObserverListEntry> observerPtrList; ///< List of all attached observers
};

};

#endif
