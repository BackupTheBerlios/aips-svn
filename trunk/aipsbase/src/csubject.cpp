/************************************************************************
 * File: csubject.cpp                                                   *
 * Project: AIPS                                                        *
 * Description: An subject base class implementing the                  *
 *              "Subject/Observer"-Pattern                              *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 2005-03-28                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "csubject.h"

using namespace std;
using namespace aips;
using boost::shared_ptr;

/****************************************************************************************************
 * CSubject::SObserverListEntry                                                                     *
 ****************************************************************************************************/

CSubject::SObserverListEntry::SObserverListEntry( CObserver* anObserverPtr_,	unsigned int uiInterestType_ )
	throw()	: anObserverPtr( anObserverPtr_ ), uiInterestType( uiInterestType_ )
{
}

/****************************************************************************************************
 * CSubject::CheckIdentityFunctor                                                                   *
 ****************************************************************************************************/

/**
 * \param anEntry list element to test
 * \param anObserverPtr pointer to observer to look out for
 * \return true if the given list elements contains the given observer
 */
bool CSubject::CheckIdentityFunctor::operator()( SObserverListEntry anEntry, CObserver* anObserverPtr ) const
{
	return( anEntry.anObserverPtr == anObserverPtr );
}

/****************************************************************************************************
 * CSubject                                                                                         *
 ****************************************************************************************************/

/*************
 * Structors *
 *************/

/**
 * \param className_ name of the class (type information)
 * \param classVersion_ version number of the class (type information)
 * \param derivedFrom_ name of the classes this class is derived from (type information)
 */
CSubject::CSubject( const string& className_, const string& classVersion_, const string& derivedFrom_ )
	throw()	: CBase( className_, classVersion_, derivedFrom_ )
{
}

CSubject::~CSubject() throw()
{
}

/*****************
 * Other methods *
 *****************/

/**
 * If you want to register an observer for several interest types, you need to call this method for every interest
 * \param anObserverPtr pointer to observer to register
 * \param uiInterestType interest type to register
 * \throws NullException if given observer pointer is NULL
 */
void CSubject::attachObserver( CObserver* const anObserverPtr, unsigned int uiInterestType ) throw( NullException )
{
	if ( anObserverPtr == NULL )
		throw( NullException( SERROR("Observer is NULL"), CException::RECOVER, ERR_CALLERNULL ) );
		
DBG2( "Adding " << anObserverPtr->getClassName() << " to subscriber list of " << this->getClassName() );

	observerPtrList.push_back( SObserverListEntry( anObserverPtr, uiInterestType ) );
}

/**
 * The observer is removed for every interest he registered with
 * \param anObserverPtr pointer to observer to register
 * \throws NullException if given observer pointer is NULL
 */
void CSubject::detachObserver( CObserver* const anObserverPtr ) throw( NullException )
{
	if ( anObserverPtr == NULL )
		throw( NullException( SERROR("Observer is NULL"), CException::RECOVER, ERR_CALLERNULL ) );
	
DBG2( "Removing " << anObserverPtr->getClassName() << " from subscriber list of " << this->getClassName() );

	observerPtrList.remove_if( std::bind2nd( CheckIdentityFunctor(), anObserverPtr ) );
}

/**
 * The observer is removed for every interest he registered with
 * \param anEventSPtr event to deploy to all registered observers with the appropiate interest
 * \throws NullException if given event is NULL
 */
void CSubject::notify( boost::shared_ptr<CEvent> anEventSPtr ) throw( NullException )
{
	if ( !anEventSPtr )
		throw( NullException( SERROR("Event is NULL"), CException::RECOVER, ERR_CALLERNULL ) );
	for( std::list<SObserverListEntry>::iterator it = observerPtrList.begin(); it != observerPtrList.end(); ++it )
	{
		if ( it->uiInterestType == anEventSPtr->getType() )
		{
DBG2( "Calling	" << anEventSPtr->dump() );//<< " for " << it->anObserverPtr->dump() );
			
			it->anObserverPtr->execute( anEventSPtr );
		}
	}
}
