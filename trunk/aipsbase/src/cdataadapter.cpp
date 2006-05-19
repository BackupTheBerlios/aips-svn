/************************************************************************
 * File: cdataadapter.cpp                                               *
 * Project: AIPS                                                        *
 * Description: An abstract base class for adapters to external libs    *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 22.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdataadapter.h"

using namespace aips;

/*************
 * Structors *
 *************/
 
/**
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 */
CDataAdapter::CDataAdapter( const std::string &sClassName_, const std::string &sClassVersion_,
    const std::string &sDerivedFrom_ ) throw()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ) 
{
}

CDataAdapter::~CDataAdapter() throw()
{  
}
