/************************************************************************
 * File: cpipelineiteminlines.h                                         *
 * Project: AIPS                                                        *
 * Description: A base class for possible pipeline module dialogs       *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-09-17                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cmoduledialog.h"

namespace aips {

CModuleDialog::CModuleDialog() throw()
	: CSubject( "CModuleDialog", "0.1", "CBase" )
{
}

CModuleDialog::CModuleDialog( const CModuleDialog& ) throw() 
	: CSubject("CModuleDialog","0.1","CBase")
{
}

CModuleDialog::~CModuleDialog() throw()
{
}

/***************************
 * Dialog member functions *
 ***************************/

/**
 * \returns true if a dialog is present (false is the default return value)
 */
inline bool CModuleDialog::hasDialog() const throw()
{
  return false;
}

/**
 * \param sCaption The new window caption
 * \exception NotPresentException if no dialog exists
 */
inline void CModuleDialog::setCaption ( const std::string ) throw( NotPresentException )
{
  throw ( NotPresentException( SERROR( "No dialog available" ),
    CException::RECOVER, ERR_DIALOGNOTPRESENT ) );
}

/**
 * \exception NotPresentException if no dialog exists (default)
 */
inline void CModuleDialog::showDialog() throw( NotPresentException )
{
  throw ( NotPresentException( SERROR( "No dialog available" ),
    CException::RECOVER, ERR_DIALOGNOTPRESENT ) );
}

/**
 * \exception NotPresentException if no dialog exists (default)
 */
inline void CModuleDialog::hideDialog() throw( NotPresentException )
{
  throw ( NotPresentException( SERROR( "No dialog available" ),
    CException::RECOVER, ERR_DIALOGNOTPRESENT ) );
}

inline bool CModuleDialog::isHidden() throw( NotPresentException )
{
 	throw ( NotPresentException( SERROR( "No dialog available" ),
    CException::RECOVER, ERR_DIALOGNOTPRESENT ) );
}

/**
 * \exception NotPresentException if no dialog exists (default)
 */
void CModuleDialog::activateDialog()	throw( NotPresentException )
{
  throw ( NotPresentException( SERROR( "No dialog available" ),
    CException::RECOVER, ERR_DIALOGNOTPRESENT ) );
}
		
CModuleDialog CModuleDialog::operator=( const CModuleDialog& otherModule ) 
throw()
{
	return *this;
}

};
