/************************************************************************
 * File: cviewhistogram.cpp                                             *
 * Project: AIPS - Basic targets plugin library                         * 
 * Description: A viewer for image histograms                           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2003-12-19                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cviewhistogram.h"
#include "libid.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/** \param ulID Unique module ID */
CViewHistogram::CViewHistogram( ulong ulID ) throw()
 : CTarget( ulID, 1, "CViewHistogram", "0.2", "CTarget" )
{
  setModuleName( "Histogram viewer" );
  setModuleID( sLibID );
  
  sDocumentation = "Displays the histogram of an integer image\n"
									 "** Input ports:\n"
                   " 0: A scalar multi-channel 2D or 3D integer data set\n"
                   "** Output ports:\n"
                   " none\n";

  inputsVec[0].portType = IOInteger;  
/* HB 28-06-05 */		
	theDialog.reset( new CViewHistogramDialog );
	setModuleDialog( theDialog );
}

CViewHistogram::~CViewHistogram() throw()
{
}



/*****************
 * Other methods *
 *****************/

void CViewHistogram::apply() throw()
{
	FBEGIN;
	bModuleReady = false;
	if ( !getInput() || getInput()->getType() != typeid( dataTraits<TImage>::dataType ) )
		return;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 4 )
  {
    alog << LWARN << SERROR("Given dataset is no 2D or 3D image!") << endl;
    return;
  }	
	bModuleReady = true;
  calculateHistogram( inputPtr );
  int iValue = static_cast<int>( inputPtr->getDataRange().getMaximum() ) - 500;
  if ( iValue < 0 )
    iValue = 0;
	theDialog->newImage( inputPtr->getDataRange().getMaximum(), iValue, dMaxValue, histogramVec );
	FEND;
}

const string CViewHistogram::dump() const throw()
{
/*  ostringstream os;
  os << "usRangeStart " << usRangeStart << " usRangeEnd " << usRangeEnd
    << " usMaxRange " << usMaxRange << "\ndisplayPtr " << displayPtr << "\n";*/
  return CTarget::dump();// + os.str();
}

CPipelineItem* CViewHistogram::newInstance( ulong ulID ) const throw()
{
  return ( new CViewHistogram( ulID ) );
}
