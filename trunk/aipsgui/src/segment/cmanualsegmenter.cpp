/************************************************************************
 * File: cmanualsegmenter.cpp                                           *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Created: Mit Jan 14 2004                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cmanualsegmenter.h"

using namespace std;
using namespace boost;

CManualSegmenter::CManualSegmenter( ulong ulID_ ) throw()
  : CFilter( ulID_, "Polyselect", 1, 1, "CManualSegmenter", "0.1", "CFilter" ), CObserver( "CManualSegmenter", "0.1", "CFilter" )
{
  setModuleName( "Polyselector" );
  setModuleID( sLibID );

  sDocumentation = "** Input ports:\n"
                   " 0: A scalar multi-channel 2D data set\n"
                   "** Output ports:\n"
                   " 0: A 1D-Array of 2D-vectors representing the polygon";
                   
  inputsVec[0].portType = CPipelineItem::IO2DInteger;
  outputsVec[0].portType = CPipelineItem::IO1DVector;
	parameters.initLong("Bla",0,0,10);
/* HB 28-06-05 */	
	
	theDialog.reset( new CDrawerDialog( this ) );
	theDialog->attachObserver( this, 999 );
	setModuleDialog( theDialog );
}

CManualSegmenter::~CManualSegmenter() throw()
{
}

void CManualSegmenter::apply() throw()
{
FBEGIN;
	bModuleReady = false;
  TImagePtr tmp = static_pointer_cast<TImage>( getInput() );
  if ( !tmp || ( tmp->getDataDimension() != 1
    && tmp->getDataDimension() != 3 && tmp->getDataDimension() != 4 ) )
  {
    alog << LWARN << "\n CDisplay2D can only display 2D images with 1 (gray), "
      << "3 (RGB) or 4 (RGBA) channels.\n" << endl;
    return;
  }
	bModuleReady = true;
  theDialog->updateView( tmp );
FEND;  
}

CPipelineItem* CManualSegmenter::newInstance( ulong ulID_ ) const throw()
{
  return new CManualSegmenter( ulID_ );
}

/** Update the view */
