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
#include "crescalehistogram.h"
#include "cintensityshift.h"
#include "chistogramequalization.h"
#include "chistocompressor.h"
#include "ccropintensityrange.h"
#include "cbinarization.h"
#include "chistogram2d.h"

using namespace std;
using namespace boost;

vector<string> classNames;
map<string,shared_ptr<CPipelineItem> > factoryMap;

extern "C" {

/** \return library documentation string */
std::string libraryDoc()
{
	return std::string("Histogram operations");
}

/** \return the library version */
std::string libraryID()
{
	return sLibID;
}

void initFactory()
{
	shared_ptr<CPipelineItem> workPtr;
	workPtr.reset( new CRescaleHistogram( 0 ) );
	factoryMap["CRescaleHistogram"] = workPtr;
	classNames.push_back("CRescaleHistogram");	
	workPtr.reset( new CIntensityShift( 0 ) );
	factoryMap["CIntensityShift"] = workPtr;
	classNames.push_back("CIntensityShift");	
	workPtr.reset( new CHistogramEqualization( 0 ) );
	factoryMap["CHistogramEqualization"] = workPtr;
	classNames.push_back("CHistogramEqualization");	
	workPtr.reset( new CHistoCompressor( 0 ) );
	factoryMap["CHistoCompressor"] = workPtr;
	classNames.push_back("CHistoCompressor");	
	workPtr.reset( new CCropIntensityRange( 0 ) );
	factoryMap["CCropIntensityRange"] = workPtr;
	classNames.push_back("CCropIntensityRange");	
	workPtr.reset( new CBinarization( 0 ) );
	factoryMap["CBinarization"] = workPtr;
	classNames.push_back("CBinarization");		
	workPtr.reset( new CHistogram2D( 0 ) );
	factoryMap["CHistogram2D"] = workPtr;
	classNames.push_back("CHistogram2D");
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

