/************************************************************************
 * File: csource.h                                                      *
 * Project: AIPS                                                        *
 * Description: A source for image data.                                *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.11                                                        *
 * Status : Beta                                                        *
 * Created: 2003-09-17                                                  *
 * Changed: 2003-09-18 Added dump() method                              *
 *          2003-09-22 All members now use string instead of QString    *
 *                      for parameters and return values                *
 *          2003-09-24 Added dialog window                              *
 *          2003-09-30 Reorganized file structure, moved documentation  *
 *                     to csource.cpp                                   *
 *          2003-10-08 Mirroring check boxes are now working            *
 *                     selectNewFile won't crash anymore if file dialog *
 *                      is cancelled.                                   *
 *                     updateData won't load a file without a file name *
 *                     Error windows are now working                    *
 *                     Corrected handling of illegal input files        *
 *          2003-10-15 Adapted for the new CDataSet class               *
 *          2003-11-11 Moved inline members to cpp file.                *
 *                     Added class to namespace aips                    *
 *          2003-12-04 Made class abstract. Code moved to CFileSource   *
 *          2004-01-22 Removed unneccessary code                        *
 *          2004-11-22 Removed setinfo. Updated constructor accordingly *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CSOURCE_H
#define CSOURCE_H

#define CSOURCE_VERSION "0.11"

// AIPS includes
#include "cpipelineitem.h"

namespace aips {

/**
 * A source for image data. 
 */
class CSource : public CPipelineItem
{      
private:
  /// Standard constructor
  CSource();
  /// Copy constructor
  CSource( CSource& );
  /// Assignment operator
  CSource& operator=( CSource& );
public:
/* Structors */
  /// Constructor.
  CSource( unsigned long ulID, unsigned short usNoOfOutputs = 1, 
    const std::string &sClassName_ = "CSource",
		const std::string &sClassVersion_ = CSOURCE_VERSION, 
		const std::string &sDerivedFrom_ = "CPipelineItem" )
    throw();
  /// Destructor 
  virtual ~CSource() 
    throw() = 0;
/* Other methods */
	/// Apply the source. Does nothing but set the module state to "ready"
	virtual void apply() 
		throw();
};

}
#endif
