/************************************************************************
 * File: ctarget.h                                                      *
 * Project: AIPS                                                        *
 * Description: A base class for the display/storage of datasets        *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 17.09.03                                                    *
 * Changed: 18.09.03 Added dump() method                                *
 *          22.09.03 All members now use string instead of QString for  *
 *                    parameters and return values                      *
 *          30.09.03 Reorganized file structure, moved documentation    *
 *                   to ctarget.cpp                                     *
 *          11.11.03 Moved inline members to cpp file.                  *
 *                   Added class to namespace aips                      *
 *          22.01.04 Made source code look prettier                     *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CTARGET_H
#define CTARGET_H

// AIPS includes
#include "cpipelineitem.h"

namespace aips {

/**
 * A base class for the display/storage of datasets
 */
class CTarget : public CPipelineItem
{
private:
  /// Standard constructor
  CTarget();
  /// Copy constructor
  CTarget( CTarget& );
  /// Assignment operator
  CTarget& operator=( CTarget& );  
public:
/* Structors */
  /// Constructor. 
  CTarget( ulong ulID, ushort usNoOfInputs = 1, const std::string &sClassName_ = "CTarget",
		const std::string &sClassVersion_ = "0.3", const std::string &sDerivedFrom_ = "CPipelineItem" )
    throw();
  /// Destructor. 
  ~CTarget()
    throw();
};

}
#endif
