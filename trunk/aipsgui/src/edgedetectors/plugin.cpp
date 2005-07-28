/************************************************************************
 * File: plugin.cpp                                                     *
 * Project: AIPS edge detectors plugin library                          *
 * Description: Library functions to access the plugin modules          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Beta                                                        *
 * Created: 10.05.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include <map>
#include <boost/shared_ptr.hpp>

#include "libid.h"
#include "cfoerstner.h"
#include "ccentraldifference.h"
#include "csobelgradient.h"
#include "chysteresis.h"
#include "cgaussderivative.h"

using namespace std;
using namespace boost;

vector<string> classNames;
map<string,shared_ptr<CPipelineItem> > factoryMap;

extern "C" {

/** \return library documentation string */
std::string libraryDoc()
{
	return std::string("Edge and corner detectors");
}

/** \return the library version */
std::string libraryID()
{
	return sLibID;
}

void initFactory()
{
	shared_ptr<CPipelineItem> workPtr;
	workPtr.reset( new CFoerstner( 0 ) );
	factoryMap["CFoerstner"] = workPtr;
	classNames.push_back("CFoerstner");	
	workPtr.reset( new CSobelGradient( 0 ) );
	factoryMap["CSobelGradient"] = workPtr;
	classNames.push_back("CSobelGradient");	
	workPtr.reset( new CCentralDifference( 0 ) );
	factoryMap["CCentralDifference"] = workPtr;
	classNames.push_back("CCentralDifference");	
	workPtr.reset( new CGaussDerivative( 0 ) );
	factoryMap["CGaussDerivative"] = workPtr;
	classNames.push_back("CGaussDerivative");	
	workPtr.reset( new CHysteresis( 0 ) );
	factoryMap["CHysteresis"] = workPtr;
	classNames.push_back("CHysteresis");	
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

