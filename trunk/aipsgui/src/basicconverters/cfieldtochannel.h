/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 2004-06-21                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CFIELDTOCHANNEL_H
#define CFIELDTOCHANNEL_H

#include <cconverter.h>
#include <cglobalprogress.h>
#include "libid.h"

using namespace aips;
 
class CFieldToChannel : public CConverter
{
private:
  /// Standard constructor
  CFieldToChannel();
  /// Copy constructor
  CFieldToChannel( CFieldToChannel& );
  /// Assignment operator
  CFieldToChannel& operator=( CFieldToChannel& );
public:
/* Structors */
  /// Constructor
  CFieldToChannel( ulong ulID )
    throw();
  /// Destructor
  ~CFieldToChannel()
    throw();
/* Other member functions */
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
};
#endif
