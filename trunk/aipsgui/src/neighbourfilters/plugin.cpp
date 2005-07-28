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
#include "cwagenknechthomogenity.h"
#include "cmorphology.h"
#include "cmedianfilter.h"
#include "clowpassfilter.h"
#include "clocalmaxima.h"
#include "chomogenity.h"
#include "chighpassfilter.h"
#include "cfillcentralregion.h"
#include "ccheckmask.h"
#include "cbinarymorphology.h"

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
	workPtr.reset( new CBinaryMorphology( 0 ) );
	factoryMap["CBinaryMorphology"] = workPtr;
	classNames.push_back("CBinaryMorphology");	
	workPtr.reset( new CMedianFilter( 0 ) );
	factoryMap["CMedianFilter"] = workPtr;
	classNames.push_back("CMedianFilter");	
	workPtr.reset( new CLowPassFilter( 0 ) );
	factoryMap["CLowPassFilter"] = workPtr;
	classNames.push_back("CLowPassFilter");	
	workPtr.reset( new CHighPassFilter( 0 ) );
	factoryMap["CHighPassFilter"] = workPtr;
	classNames.push_back("CHighPassFilter");	
	workPtr.reset( new CLocalMaxima( 0 ) );
	factoryMap["CLocalMaxima"] = workPtr;
	classNames.push_back("CLocalMaxima");	
	workPtr.reset( new CHomogenity( 0 ) );
	factoryMap["CHomogenity"] = workPtr;
	classNames.push_back("CHomogenity");	
	workPtr.reset( new CWagenknechtHomogenity( 0 ) );
	factoryMap["CWagenknechtHomogenity"] = workPtr;
	classNames.push_back("CWagenknechtHomogenity");	
	workPtr.reset( new CMorphology( 0 ) );
	factoryMap["CMorphology"] = workPtr;
	classNames.push_back("CMorphology");	
	workPtr.reset( new CFillCentralRegion( 0 ) );
	factoryMap["CFillCentralRegion"] = workPtr;
	classNames.push_back("CFillCentralRegion");	
	workPtr.reset( new CCheckmask( 0 ) );
	factoryMap["CCheckmask"] = workPtr;
	classNames.push_back("CCheckmask");	
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
