/***********************************************************************
 * File: cdisplay.h                                                    *
 * Project: AIPS basic targets plugin library                          *
 * Description: A class to display a voxel dataset on screen           *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.4                                                        *
 * Status : Beta                                                       *
 * Created: 2003-09-17                                                 *
 * Changed: 2003-09-18 Added dump() method                             *
 *                     Added signals, made updateView a public slot    *
 *                     Documentation and exception handling added      *
 *          2003-09-30 Reorganized file structure, moved documentation *
 *                      to cdisplay.cpp                                *
 *          2003-10-02 Corrected an error in updateView (flashing      *
 *                     widgets for 2D images)                          *
 *          2003-10-08 Display now uses an optional color map          *
 *          2004-01-16 Corrected the crashing checkbox error           *
 *                     Display window will can now be smart placed     *
 *                     Display window is now resizeable and has        *
 *                      optional scrollbars for large images           *
 *          2004-01-28 Moved inline members to cpp file                *
 *                     Documented and clarified source code            *
 *                     Moved class CImageDisplay to seperate file in   *
 *                      library aipsbase                               *
 *          2004-02-06 Removed uneccessary consrtuctor param sCaption  *
 *          2004-05-05 Class now also supports multichannel data with  *
 *                      values greater than 8 bit                      *
 *          2004-06-16 Updated documentation and error output          *
 ***********************************************************************/

#ifndef CDISPLAY_H
#define CDISPLAY_H

// AIPS includes
#include <ctarget.h>
#include <cdisplaydialog.h>
// lib includes
#include "libid.h"

using namespace aips;

/** A module to display a dataset on screen */
class CDisplay : public CTarget
{
private:
	/// Standard constructor
	CDisplay();
	/// Copy constructor
	CDisplay( CDisplay& );
	/// Assignment operator
	CDisplay& operator=( CDisplay& );
public:
/* Structors */
  /// Constructor
  CDisplay( ulong ulID )
    throw();
  /// Destructor 
  virtual ~CDisplay()
    throw();
/* Other methods */		
  /// Reimplemented from CTarget  
  virtual void apply()
    throw();
  /// Reimplemented from CTarget 
  virtual const std::string dump() const
    throw();
  /// Reimplemented from CTarget
  CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
private:
	boost::shared_ptr<CDisplayDialog> myDialog;
};


#endif
