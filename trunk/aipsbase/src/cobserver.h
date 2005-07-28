/************************************************************************
 * File: cobserver.h                                                    *
 * Project: AIPS                                                        *
 * Description: An observer base class for the implementation of the    *
 *              "Subject/Observer"-pattern                              *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Alpha                                                       *
 * Created: 2005-03-28                                                  *
 * Changed: 2005-03-31 Added documentation                              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSCOBSERVER_H
#define AIPSCOBSERVER_H

// Boost includes
#include <boost/shared_ptr.hpp>
// AIPS includes 
#include <cevent.h>

namespace aips {

/**
 * An abstract observer base class for the implementation of the Subject/Observer-Pattern
 */
class CObserver : public CBase
{
public:
/* Structors */
	/// Constructor
  CObserver( const std::string& className_ = "CObserver",
		const std::string& classVersion_ =  "0.2", const std::string& derivedFrom_ = "CBase" ) throw();
	/// Destructor
  ~CObserver() throw();
/* Other methods */
	/// Execute is called by the observed subject if an suitable event was generated. Pure virtual method.  
	virtual void execute( TEventPtr anEvent ) = 0;
};

};

#endif
