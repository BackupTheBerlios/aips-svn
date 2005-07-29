/************************************************************************
 * File: cchannelcombiner.h                                             *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Combines one or more images in a new multi-channel      *
 *              image.                                                  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status: Alpha                                                        *
 * Created: 2004-02-04                                                  *
 * Changed: 2004-05-05 Module can now combine up to four images         *
 *          2004-06-16 Updated documentation and error output           *
 *          2004-01-13 Module now works on all set input types          *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CCHANNELCOMBINER_H
#define CCHANNELCOMBINER_H

#include <cconverter.h>
#include <aipsdatatraits.h>

#include "libid.h"
using namespace aips;

/**
 * Combines two images into a new image with more channels
 */
class CChannelCombiner : public CConverter
{
private:
	/// Standard constructor
	CChannelCombiner();
	/// Copy constructor
	CChannelCombiner( CChannelCombiner& );
	/// Assignment operator
	CChannelCombiner& operator=( CChannelCombiner& );
public: 
/* Structors */
	/// Constructor
  CChannelCombiner( ulong ulID )
    throw();
	/// Destructor
  ~CChannelCombiner()
    throw();
/* Functions from CConverter implemented here */
	/// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
	virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw(); 
private:
	template<typename T, bool compare> 
	class combine
	{};
	template<typename T> 
	class combine<T,true>
	{
		public:
		bool operator() ( CChannelCombiner* parent ) throw();
	};
	template<typename T> 
	class combine<T,false>
	{
		public:
		bool operator() ( CChannelCombiner* parent ) throw();
	};
	template<unsigned int index> void call() throw();
};

#endif
