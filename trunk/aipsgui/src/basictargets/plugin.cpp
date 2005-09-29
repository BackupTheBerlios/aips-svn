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
#include "cimagewriter.h"
#include "cviewhistogram.h"
#include "cdisplayvectorfield.h"
#include "cdisplay.h"

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
	classNames.clear();
	factoryMap.clear();
	shared_ptr<CPipelineItem> workPtr;
	workPtr.reset( new CDisplay( 0 ) );
	factoryMap["CDisplay"] = workPtr;
	classNames.push_back("CDisplay");	
	workPtr.reset( new CDisplayVectorField( 0 ) );
	factoryMap["CDisplayVectorField"] = workPtr;
	classNames.push_back("CDisplayVectorField");	
	workPtr.reset( new CImageWriter( 0 ) );
	factoryMap["CImageWriter"] = workPtr;
	classNames.push_back("CImageWriter");	
	workPtr.reset( new CViewHistogram( 0 ) );
	factoryMap["CViewHistogram"] = workPtr;
	classNames.push_back("CViewHistogram");	
}

void unloadFactory()
{
	factoryMap.clear();
	classNames.clear();
}

/** \return the number of modules in the library */
uint numberOfModules()
{
	cerr << "<DBG: Class names: " << endl;
	for( vector<string>::iterator it = classNames.begin(); it != classNames.end(); ++it )
	cerr << "     " << *it << endl;
	cerr << ">" << endl;
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
