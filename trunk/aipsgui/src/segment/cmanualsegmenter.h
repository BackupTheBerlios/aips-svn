/************************************************************************
 * File: cmanualsegmenter.h                                                     *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                                           *
 *                                                                      *
 * Version: 0.1                                                         *
 * Created: Mit Jan 14 2004                                                      *
 * Based on:                                                            *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CMANUALSEGMENTER_H
#define CMANUALSEGMENTER_H

#include <boost/shared_ptr.hpp>
#include <cfilter.h>
#include <aipsnumeric.h>

#include "libid.h"
#include "cdrawerdialog.h"


using namespace aips;

/**
  * Input: An image
  * Output: Polygon ( 1D Dataset of CVector2D )
  */
  

class CManualSegmenter : public CFilter, public CObserver
{
public: 
  CManualSegmenter( ulong ulID_ ) throw();
  virtual ~CManualSegmenter() throw();
  virtual void apply() throw();
  virtual CPipelineItem* newInstance( ulong ulID_ = 0 ) const throw();  
	virtual void execute( shared_ptr<CEvent> anEvent )
	{
		shared_ptr<CDrawerChangedEvent> e = dynamic_pointer_cast<CDrawerChangedEvent>( anEvent );
		if( e.get() )
		{
			setOutput( e->getField() );
			notify( shared_ptr<CDataChangedEvent>( new CDataChangedEvent( this ) ) );
		}
	}
private:
	boost::shared_ptr<CDrawerDialog> theDialog;
};

#endif
