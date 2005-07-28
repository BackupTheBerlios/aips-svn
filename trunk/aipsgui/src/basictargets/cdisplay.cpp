/***********************************************************************
 * File: cdisplay.cpp                                                  *
 * Project: QtVis                                                      *
 * Description: A class to display a voxel dataset on screen           *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 2003-09-17                                                 *
 ***********************************************************************/

#include "cdisplay.h"
#include <cimagedisplay.h>

using namespace std;

/** \param ulID unique module ID */
CDisplay::CDisplay( ulong ulID ) throw()
 : CTarget( ulID, 1, "CDisplay", "0.4", "CTarget" )
{
  setModuleName( "2D image viewer" );
  setModuleID( sLibID );

  sDocumentation = "Displays a scalar 2D image\n"
									 "** Input ports:\n"
                   " 0: A scalar multi-channel 2D data set\n"
                   "** Output ports:\n"
                   " none";

  inputsVec[0].portType = IO2DInteger;
/* HB 28-06-05 */	
	myDialog.reset( new CDisplayDialog() );
 	setModuleDialog( myDialog );
}

CDisplay::~CDisplay() throw()
{
}

void CDisplay::apply() throw()
{
  // Test for correct input
	bModuleReady = false;
	if ( !getInput() )
		return;
	if ( getInput()->getType() == typeid( dataTraits<TImage>::dataType ) )
	{
  	TImage* inputPtr = static_cast<TImage*>( getInput().get() );
  	if ( inputPtr != NULL && inputPtr->getDataDimension() >= 1
    	&& inputPtr->getDataDimension() < 5 )
	  {
			bModuleReady = true;
  		myDialog->updateView( inputPtr );
	  }
	}
	else if ( getInput()->getType() == typeid( dataTraits<TField>::dataType ) )
	{
		TField* fieldPtr = static_cast<TField*>( getInput().get() );
		if ( fieldPtr != NULL && fieldPtr->getDataDimension() >= 1
    	&& fieldPtr->getDataDimension() < 5 )
		{
	  	bModuleReady = true;
	  	myDialog->updateView( fieldPtr );
		}
  }
  else
  {
    alog << LWARN << SERROR(" Can only display 2D images with 1 (gray), 2, ")
      << " 3 (RGB) or 4 (RGBA) channels.\n" << endl;
   	return;
	}
}

const string CDisplay::dump() const throw()
{
/*  ostringstream os;
  os << "displayPtr: " << displayPtr << "\n";*/
  return CTarget::dump();// + os.str();
}


/** \param ulID unique module id */
CPipelineItem* CDisplay::newInstance( ulong ulID ) const throw()
{
  return ( new CDisplay( ulID ) );
}
