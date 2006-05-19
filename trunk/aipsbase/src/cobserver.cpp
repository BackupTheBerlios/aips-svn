/************************************************************************
 * File: cobserver.cpp                                                  *
 * Project: AIPS                                                        *
 * Description: An observer base class for the implementation of the    *
 *              "Subject/Observer"-pattern                              *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 2005-03-28                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cobserver.h"

using namespace std;
using namespace aips;

/**
 * \param className_ name of the class (type information)
 * \param classVersion_ version number of the class (type information)
 * \param derivedFrom_ name of the classes this class is derived from (type information)
 */
CObserver::CObserver( const string& className_, const string& classVersion_, const string& derivedFrom_ )
	throw() : CBase( className_, classVersion_, derivedFrom_ )
{
}

CObserver::~CObserver() throw()
{
}
