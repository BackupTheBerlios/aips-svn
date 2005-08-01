/************************************************************************
 * File: cconverter.h                                                   *
 * Project: AIPS                                                        *
 * Description: Base class for all conversion modules                   *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 2004-01-28                                                  *
 * Changed:                                                             *
 *        2004-11-22 Removed setinfo. Updated constructor accordingly   *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CCONVERTER_H
#define CCONVERTER_H

#include "cpipelineitem.h"

namespace aips 
{

/**
 * Abstract base class for all conversion modules
 */
class CConverter : public CPipelineItem
{
private:
  /// Standard constructor
  CConverter();
  /// Copy constructor
  CConverter( CConverter& );
  /// Assignment operator
  CConverter& operator=( CConverter& );
public:
/* Structors */
  /// Constructor
  CConverter( ulong ulID, ushort usFanIn = 1, ushort usFanOut = 1, 
		const std::string &sClassName_ = "CConverter", const std::string &sClassVersion_ = "0.1.1", 
		const std::string &sDerivedFrom_ = "CPipelineItem" ) throw();
  /// Destructor
  virtual ~CConverter()
    throw();
};

}
#endif
