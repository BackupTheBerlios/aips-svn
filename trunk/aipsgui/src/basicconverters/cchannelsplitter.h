/************************************************************************
 * File: cchannelsplitter.h                                             *
 * Project: AIPS                                                        *
 * Description: Splits a multichannel image into several singlechannels *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Created: 2004-09-07                                                  *
 * Status: Alpha                                                        *
 * Changed: 2005-13-01 Module now supports all types of datasets        *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef CCHANNELSPLITTER_H
#define CCHANNELSPLITTER_H

#include <cconverter.h>

#include "libid.h"
using namespace aips;

/** Splits a multichannel image into several single-channel images */
class CChannelSplitter : public CConverter
{
private:
	/// Standard constructor
	CChannelSplitter();
	/// Copy constructor
	CChannelSplitter( CChannelSplitter& );
	/// Assignment operator
	CChannelSplitter& operator=( CChannelSplitter& );
public: 
/* Structors */
	/// Constructor
  CChannelSplitter( ulong ulID )
    throw();
	/// Destructor
  ~CChannelSplitter()
    throw();
/* Functions from CConverter implemented here */
	/// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
	virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
private:
	template<typename T> void splitup() throw();
};

#endif
