/************************************************************************
 * File: csource.cpp                                                    *
 * Project: AIPS                                                        *
 * Description: A source for image data.                                *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 17.09.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "csource.h"

using namespace std;
using namespace aips;

/**
 * \param uiID unique module ID
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 */
CSource::CSource(  ulong ulID, ushort usFanOut, const string &sClassName_, 
		const string &sClassVersion_, const string &sDerivedFrom_ ) throw()
  : CPipelineItem ( ulID, 0, usFanOut, sClassName_, sClassVersion_, sDerivedFrom_ )
{
  setModuleName( "CSource" );
  setType( CPipelineItem::ITypeSource );

  outputsVec[0].portType = CPipelineItem::IOInteger;
}

CSource::~CSource() throw()
{
}

void CSource::apply() throw()
{
	bModuleReady = true;
}
