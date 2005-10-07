//
// C++ Implementation: cglobalconfig
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
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
