/************************************************************************
 * File: plugin.cpp                                                     *
 * Project: AIPS basic converters plugin library                        *
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
#include "csliceselector.h"
#include "creducevector.h"
#include "cswapendianess.h"
#include "cfieldtochannel.h"
#include "cchannelsplitter.h"
#include "cchannelcombiner.h"
#include "cfieldtoimage.h"
#include "cimagetofield.h"

using namespace std;
using namespace boost;

vector<string> classNames;
map<string,shared_ptr<CPipelineItem> > factoryMap;

extern "C" {

/** \return library documentation string */
std::string libraryDoc()
{
	return std::string("Some basic conversion modules");
}

/** \return the library version */
std::string libraryID()
{
	return sLibID;
}

void initFactory()
{
	shared_ptr<CPipelineItem> workPtr;
	workPtr.reset( new CSliceSelector( 0 ) );
	factoryMap["CSliceSelector"] = workPtr;
	classNames.push_back("CSliceSelector");
	workPtr.reset( new CReduceVector( 0 ) );
	factoryMap["CReduceVector"] = workPtr;
	classNames.push_back("CReduceVector");	
	workPtr.reset( new CSwapEndianess( 0 ) );
	factoryMap["CSwapEndianess"] = workPtr;
	classNames.push_back("CSwapEndianess");	
	workPtr.reset( new CChannelCombiner( 0 ) );
	factoryMap["CChannelCombiner"] = workPtr;
	classNames.push_back("CChannelCombiner");	
	workPtr.reset( new CChannelSplitter( 0 ) );
	factoryMap["CChannelSplitter"] = workPtr;
	classNames.push_back("CChannelSplitter");	
	workPtr.reset( new CFieldToChannel( 0 ) );
	factoryMap["CFieldToChannel"] = workPtr;
	classNames.push_back("CFieldToChannel");	
	workPtr.reset( new CFieldToImage( 0 ) );
	factoryMap["CFieldToImage"] = workPtr;
	classNames.push_back("CFieldToImage");	
	workPtr.reset( new CImageToField( 0 ) );
	factoryMap["CImageToField"] = workPtr;
	classNames.push_back("CImageToField");	
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

