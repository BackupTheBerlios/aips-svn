/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Belitz                                  *
 *   h.belitz@fz-juelich.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CDISPLAYDIALOG_H
#define CDISPLAYDIALOG_H

#include <cglobalconfig.h>
#include <cmoduledialog.h>
#include <aipsnumeric.h>

#include <cimagedisplay.h>

using namespace aips;

class CDisplayDialog : public CModuleDialog
{
public:
    CDisplayDialog() throw();

    ~CDisplayDialog() throw();
/* Dialog methods */    
  /// Returns true because CDisplay always has a dialog window
  virtual bool hasDialog() const
    throw();
  /// Reimplemented from CPipelineItem 
  virtual QWidget* getDialogHandle() const
    throw( NotPresentException );
  /// To set the window caption
  void setCaption ( const std::string sCaption )
    throw();
  /// Reimplemented from CPipelineItem 
  virtual void showDialog()
    throw( NotPresentException );
  /// Reimplemented from CPipelineItem 
  virtual void hideDialog()
    throw( NotPresentException );
  virtual bool isHidden()
    throw( NotPresentException );
  virtual void activateDialog()
    throw( NotPresentException );
  /// Update the view 
  void updateView( TImagePtr inputPtr, bool bImage = true )
    throw();  
  void updateView( TFieldPtr inputPtr, bool bImage = true )
    throw();
  
private:
  
  CImageDisplay* displayPtr; ///< The display window
  uint width, height;
};


#endif
