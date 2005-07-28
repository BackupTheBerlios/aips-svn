/************************************************************************
 * File: cdraglistbox.cpp                                               *
 * Project: AIPS                                                        *
 * Description: A listbox which allows dragging of its elements         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 10.09.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdraglistbox.h"
#include <iostream>
/**
 * Constructor
 * \param iID box id number (unique)
 * \param parentPtr Qt parent widget
 * \param sName Qt object name
 */
CDragListBox::CDragListBox( int typeID_, int libID_, QWidget* parentPtr, const char* sName ) throw()
  : QListBox( parentPtr, sName ), iID( libID_ ), tID( typeID_ )
{
}

/** Destructor */
CDragListBox::~CDragListBox() throw()
{  
}

/**
 * Reimplemented from QListBox.
 * Signals dragItem( int, int ) if left mousebutton is pressed
 */
void CDragListBox::mouseMoveEvent ( QMouseEvent* eventPtr ) throw()
{
  if ( eventPtr->state() & LeftButton )
  {
    emit( dragItem( tID, iID, currentItem() ) );
  }
}

void CDragListBox::setLibraryName( const std::string& sLibName_ )
{
	sLibName = sLibName_;
}

const std::string CDragListBox::getLibraryName() const
{
	return sLibName;
}
