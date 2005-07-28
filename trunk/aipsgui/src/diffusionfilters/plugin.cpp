/************************************************************************
 * File: plugin.cpp                                                     *
 * Project: AIPS basic targets plugin library                           *
 * Description: Library functions to access the plugin modules          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Beta                                                        *
 * Created: 27.08.03                                                    *
 * Changed: 28.01.04 Documented and clarified code                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include <map>
#include <boost/shared_ptr.hpp>

#include "libid.h"
#include "cweickertfilter.h"
#include "chuberfilter.h"
#include "cpmad2filter.h"
#include "cpmad1filter.h"
#include "ctukeyfilter.h"

using namespace std;
using namespace boost;

vector<string> classNames;
map<string,shared_ptr<CPipelineItem> > factoryMap;

extern "C" {

/** \return library documentation string */
std::string libraryDoc()
{
	return std::string("Basic viewers and writers");
}

/** \return the library version */
std::string libraryID()
{
	return sLibID;
}

void initFactory()
{
	shared_ptr<CPipelineItem> workPtr;
	workPtr.reset( new CWeickertFilter( 0 ) );
	factoryMap["CWeickertFilter"] = workPtr;
	classNames.push_back("CWeickertFilter");	
	workPtr.reset( new CTukeyFilter( 0 ) );
	factoryMap["CTukeyFilter"] = workPtr;
	classNames.push_back("CTukeyFilter");	
	workPtr.reset( new CPMAD2Filter( 0 ) );
	factoryMap["CPMAD2Filter"] = workPtr;
	classNames.push_back("CPMAD2Filter");	
	workPtr.reset( new CPMAD1Filter( 0 ) );
	factoryMap["CPMAD1Filter"] = workPtr;
	classNames.push_back("CPMAD1Filter");	
	workPtr.reset( new CHuberFilter( 0 ) );
	factoryMap["CHuberFilter"] = workPtr;
	classNames.push_back("CHuberFilter");	
}

void unloadFactory()
{
	factoryMap.clear();
	classNames.clear();
}

/** \return the number of modules in the library */
uint numberOfModules()
{
  return classNames.size();
}

/**
 * Create and retrieve a new module
 * \param uiID number of module
 * \return new module instance
 */
shared_ptr<CPipelineItem> createModuleByNumber( const uint uiID )
{
	if ( uiID < classNames.size() )
	{		
		return (factoryMap[ classNames[ uiID ] ]);
	}
	else
	{
		shared_ptr<CPipelineItem> dummy; dummy.reset();
		return dummy;
	}
}

shared_ptr<CPipelineItem> createModuleByString( const string& sID )
{
	if ( factoryMap.find( sID ) != factoryMap.end() )
		return (factoryMap[ sID ]);
	else
	{
		shared_ptr<CPipelineItem> dummy; dummy.reset();
		return dummy;
	}
}

}
