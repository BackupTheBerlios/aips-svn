/************************************************************************
 * File: ctarget.h                                                      *
 * Project: AIPS                                                        *
 * Description: A base class for the display/storage of datasets        *
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

#include "ctarget.h"

using namespace std;
using namespace aips;

/** 
 * \param ulID module ID
 * \param usNoOfInputs number of input ports (must be greater zero)
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 * The type of the first input port is set to IOOther by default
 */
CTarget::CTarget( ulong ulID, ushort usNoOfInputs, const std::string &sClassName_, 
		const std::string &sClassVersion_, const std::string &sDerivedFrom_ ) throw()
  : CPipelineItem( ulID, usNoOfInputs, 0, sClassName_, sClassVersion_, sDerivedFrom_  )
{
  setType( ITypeTarget );
  inputsVec[0].portType = IOOther;
}

CTarget::~CTarget() throw()
{
}
