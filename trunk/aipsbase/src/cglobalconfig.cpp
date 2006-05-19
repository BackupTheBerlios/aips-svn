//
// C++ Implementation: cglobalconfig
//
// Description: 
//
//
// Author: Hendrik Belitz <hbelitz@users.berlios.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include<cglobalconfig.h>
namespace aips{
CTypedMap& getGlobalConfiguration() throw()
{
  static aips::CTypedMap theConfiguration;
  return theConfiguration;
}
}
