/************************************************************************
 * File: plugin.cpp                                                     *
 * Project: AIPS basic filters plugin library                           *
 * Description: Library functions to access the plugin modules          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Beta                                                        *
 * Created: 27.08.03                                                    *
 * Changed: 29.01.04 Documented and clarified code                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

#include "libid.h"
#include "cparticlesnake.h"
#include "cmanualsegmenter.h"
#include "csynergeticmodel.h"
#include "cclosewm.h"
#include "cextractmask.h"
#include "cextractregion.h"
#include "cpartsnakestart.h"
#include "cgeneratestartcontour.h"
#include "cregiongrowing.h"
#include "cfindcenterline.h"
#include "cpointlistfrommask.h"
#include "cdeletenarrows.h"
#include "ccompletevt.h"
using namespace std;
using namespace boost;
using namespace aips;

vector<string> classNames;
map<string,shared_ptr<CPipelineItem> > factoryMap;

extern "C" {

/** \return library documentation string */
std::string libraryDoc()
{
	return std::string("Segmentation algorithms and helper modules");
}

/** \return the library version */
std::string libraryID()
{
	return sLibID;
}

void initFactory()
{
	shared_ptr<CPipelineItem> workPtr;
	workPtr.reset( new CParticleSnake( 0 ) );
	factoryMap["CParticleSnake"] = workPtr;
	classNames.push_back("CParticleSnake");	
	workPtr.reset( new CManualSegmenter( 0 ) );
	factoryMap["CManualSegmenter"] = workPtr;
	classNames.push_back("CManualSegmenter");
	workPtr.reset( new CSynergeticModel( 0 ) );
	factoryMap["CSynergeticModel"] = workPtr;
	classNames.push_back("CSynergeticModel");
	workPtr.reset( new CCloseWM( 0 ) );
	factoryMap["CCloseWM"] = workPtr;
	classNames.push_back("CCloseWM");
	workPtr.reset( new CExtractMask( 0 ) );
	factoryMap["CExtractMask"] = workPtr;
	classNames.push_back("CExtractMask");
	workPtr.reset( new CExtractRegion( 0 ) );
	factoryMap["CExtractRegion"] = workPtr;
	classNames.push_back("CExtractRegion");
	workPtr.reset( new CPartSnakeStart( 0 ) );
	factoryMap["CPartSnakeStart"] = workPtr;
	classNames.push_back("CPartSnakeStart");
	workPtr.reset( new CGenerateStartContour( 0 ) );
	factoryMap["CGenerateStartContour"] = workPtr;
	classNames.push_back("CGenerateStartContour");	
	workPtr.reset( new CRegionGrowing( 0 ) );
	factoryMap["CRegionGrowing"] = workPtr;
	classNames.push_back("CRegionGrowing");	
	workPtr.reset( new CFindCenterLine( 0 ) );
	factoryMap["CFindCenterLine"] = workPtr;
	classNames.push_back("CFindCenterLine");	
	workPtr.reset( new CPointListFromMask( 0 ) );
	factoryMap["CPointListFromMask"] = workPtr;
	classNames.push_back("CPointListFromMask");
	workPtr.reset( new CDeleteNarrows( 0 ) );
	factoryMap["CDeleteNarrows"] = workPtr;
	classNames.push_back("CDeleteNarrows");
	workPtr.reset( new CCompleteVT( 0 ) );
	factoryMap["CCompleteVT"] = workPtr;
	classNames.push_back("CCompleteVT");
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
