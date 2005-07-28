/************************************************************************
 * File: csource.h                                                      *
 * Project: AIPS                                                        *
 * Description: A source for image data.                                *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.6.1                                                       *
 * Status : Beta                                                        *
 * Created: 17.09.03                                                    *
 * Changed: 18.09.03 Added dump() method                                *
 *          22.09.03 All members now use string instead of QString for  *
 *                    parameters and return values                      *
 *          24.09.03 Added dialog window                                *
 *          30.09.03 Reorganized file structure, moved documentation    *
 *                   to csource.cpp                                     *
 *          08.10.03 Mirroring check boxes are now working              *
 *                   selectNewFile won't crash anymore if file dialog   *
 *                    is cancelled.                                     *
 *                   updateData won't load a file without a file name   *
 *                   Error windows are now working                      *
 *                   Corrected handling of illegal input files          *
 *          15.10.03 Adapted for the new CDataSet class                 *
 *          11.11.03 Moved inline members to cpp file.                  *
 *                   Added class to namespace aips                      *
 *          04.12.03 Made class abstract. Code moved to CFileSource     *
 *          22.01.04 Removed unneccessary code                          *
 *        2004-11-22 Removed setinfo. Updated constructor accordingly   *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CSOURCE_H
#define CSOURCE_H

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
  CSource( ulong ulID, ushort usFanOut = 1, const std::string &sClassName_ = "CSource",
		const std::string &sClassVersion_ = "0.6.1", const std::string &sDerivedFrom_ = "CPipelineItem" )
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
