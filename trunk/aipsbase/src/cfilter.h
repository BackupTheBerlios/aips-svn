/************************************************************************
 * File: cfilter.h                                                      *
 * Project: AIPS                                                        *
 * Description: An abstract base class for all image filters            *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.11                                                        *
 * Status : Beta                                                        *
 * Created: 2003-08-26                                                  *
 * Changed: 2003-09-04 Standardized variable names                      *
 *          2003-09-09 Exceptions added, attributes are now private     *
 *                     Constructor initializes constant parameters      *
 *          2003-09-15 Documentation, added dialog methods,             *
 *                     Get/Set-Methods now throw OutOfRangeException    *
 *                     Methods now use NullException instead of         *
 *                     std::runtime_error                               *
 *                     Class moved to dynamic library aips base         *
 *                     Added method newInstance                         *
 *          2003-09-17 Deleted member ulFilterID and corresponding      *
 *                     get/set-Functions                                *
 *                     Added const to all get-Methods                   *
 *                     Moved dialog methods to CPipelineItem            *
 *          2003-09-18 Added dump() method                              *
 *          2003-09-22 All members now use string instead of QString for*
 *                      parameters and return values                    *
 *          2003-09-30 Reorganized file structure, moved documentation  *
 *                     to cfilter.cpp                                   *
 *          2003-11-09 Moved code into namespace aips                   *
 *                     Moved inline members to cpp file                 *
 *          2004-05-01 Removed unneccessary method apply()              *
 *          2004-11-22 Removed setinfo. Updated constructor accordingly *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CFILTER_H
#define CFILTER_H

// AIPS includes
#include "cpipelineitem.h"

namespace aips {

/**
 * An abstract filter base class
 */    
class CFilter : public CPipelineItem
{
public:
/* Structors */
  /// Constructor
  CFilter( ulong ulID_, const std::string &sName_ = "Generic filter",
    const ushort usNoOfInputs = 1, const ushort usNoOfOutputs = 1,
		const std::string &sClassName_ = "CFilter", const std::string &sClassVersion_ = "0.11", 
		const std::string &sDerivedFrom_ = "CPipelineItem" )
    throw();
  /// Destructor 
  virtual ~CFilter()
    throw();
};

}
#endif

