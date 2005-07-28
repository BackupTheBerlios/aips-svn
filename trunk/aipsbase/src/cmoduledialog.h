/************************************************************************
 * File: cmoduledialog.h                                                *
 * Project: AIPS                                                        *
 * Description: A base class for possible pipeline module dialogs       *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Pre-Alpha                                                   *
 * Created: 2004-09-17                                                  *
 * Changed:                                                             *
 * ToDo: operator= needs to be fully implemented                        *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSCMODULEDIALOG_H
#define AIPSCMODULEDIALOG_H

#include <cexception.h>
#include <csubject.h>

namespace aips {

/**
 * Provides an abstraction of a possible pipeline module dialog window
 */
class CModuleDialog : public CSubject
{
public:
	/// Constructor
  CModuleDialog() 
		throw();
	/// Copy constructor
	CModuleDialog( const CModuleDialog& ) 
		throw();
	/// Assignment operator
	virtual CModuleDialog operator=( const CModuleDialog& otherModule ) 
		throw();
	/// Destructor
  virtual~ CModuleDialog() 
		throw();
	/// Check if the item has its own dialog window(s)
  virtual bool hasDialog() const
    throw();
  /// Sets the window caption
  virtual void setCaption ( const std::string sCaption )
    throw( NotPresentException );   
  /// Show the item dialog
  virtual void showDialog()
    throw( NotPresentException );
  /// Hide the item dialog
  virtual void hideDialog()
    throw( NotPresentException );
	/// Returns true if the dialog is currently hidden
	virtual bool isHidden()
		throw( NotPresentException );
	/// Makes the dialog the active window
	virtual void activateDialog()
		throw( NotPresentException );
};

};

#endif
