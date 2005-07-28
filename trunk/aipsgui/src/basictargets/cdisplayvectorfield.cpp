/************************************************************************
 * File: cdisplayvectorfield.cpp                                        *
 * Project: AIPS                                                        *
 * Description: A display for 2D vector fields                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Created: 2003-12-10                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "libid.h"
#include "cdisplayvectorfield.h"

using namespace std;
using namespace boost;

/**
 * \param ulID unique module ID
 */
CDisplayVectorField::CDisplayVectorField( ulong ulID ) throw()
  : CTarget( ulID, 2, "CDisplayVectorField", "0.3", "CTarget" )
{
  setModuleName( "Vector field output" );
  setModuleID( sLibID );

  sDocumentation = "Displays a 2D vector field\n"
									 "** Input ports:\n"
                   " 0: A vector mono-channel 2D data set\n"
                   " 1: A scalar multi-channel 2D data set (optional)\n"
                   "** Output ports:\n"
                   " none";
  
  inputsVec[0].portType = CPipelineItem::IO2DVector;
  inputsVec[1].portType = CPipelineItem::IO2DInteger;
/* HB 28-06-05 */	
  
	theDisplay.reset( new CDisplayVectorDialog );
	setModuleDialog( theDisplay );
}

CDisplayVectorField::~CDisplayVectorField() throw()
{
}

void CDisplayVectorField::apply() throw()
{
FBEGIN;
BENCHSTART;
  // First check the input fields
	bModuleReady = false;
	TField2DPtr inputPtr = static_pointer_cast<TField2D>( getInput() );
	if ( !inputPtr )
	{
		alog << LWARN << SERROR("No input") << endl;
		return;
	}
/*	if( !checkType<TField2D>( *inputPtr ) )
	{
		alog << inputPtr->getType().name() << endl;
		alog << typeid(TVector2D).name() << endl;
		alog << typeid(dataTraits<TField2D>::dataType).name() << endl;
		alog << LWARN << SERROR("Illegal input") << endl;
		return;
	}*/
  TField2D* fieldPtr = static_cast<TField2D*>( inputPtr.get() );
  if ( fieldPtr == NULL
    || fieldPtr->getDataDimension() != 1 )
  {
    alog << LWARN << SERROR("Given dataset is no 2D vector field!") << endl;
    return;
  }
  TImage* imagePtr = static_cast<TImage*>( getInput( 1 ).get() );
// Dunno why this crashes sometimes at this point;
  if ( imagePtr == NULL 
  	|| ( imagePtr->getDataDimension() != 1
    && imagePtr->getDataDimension() != 3
    && imagePtr->getDataDimension() != 4 ) 
    || !checkType<TImage>( *imagePtr ) )
  {
    alog << LWARN << SERROR(" CDisplayVectorField can only display 2D images with 1 (gray),")
      << " 3 (RGB) or 4 (RGBA) channels." << endl;
    return;
  }   
	bModuleReady = true;
  theDisplay->updateView( imagePtr, fieldPtr );
BENCHSTOP;	
FEND;
}

const std::string CDisplayVectorField::dump() const throw()
{
/*  ostringstream os;
  os << "displayPtr: " << displayPtr << "\nbufferPixmap dimensions: "
    << bufferPixmap.width() << " x " << bufferPixmap.height() << "\n";*/
  return CTarget::dump();// + os.str();
}

CPipelineItem* CDisplayVectorField::newInstance( ulong ulID ) const throw()
{
  return ( new CDisplayVectorField( ulID ) );
}
